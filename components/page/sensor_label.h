#pragma once
#include "bound_input.h"
#include "esphome/core/helpers.h"
#include "esphome/components/api/api_server.h"

namespace esphome {
namespace page {

class SensorLabel : public BoundInput {
 public:
  SensorLabel(lv_obj_t *label, const char *format) : label_(label), format_(format) {}

  void set_entity_id(const char *entity_id) { this->entity_id_ = entity_id; }

  void subscribe_ha_state() override {
    if (!this->entity_id_ || !api::global_api_server)
      return;
    api::global_api_server->subscribe_home_assistant_state(
        this->entity_id_, nullptr, [this](StringRef state) {
          auto val = parse_number<float>(state.c_str());
          if (val.has_value()) {
            this->ha_value_ = *val;
            this->has_value_ = true;
          }
          if (this->page_active_ && *this->page_active_)
            this->sync_from_cache();
        });
  }

  void sync_from_cache() override {
    if (!this->has_value_)
      return;
    char buf[64];
    snprintf(buf, sizeof(buf), this->format_, this->ha_value_);
    lv_label_set_text(this->label_, buf);
  }

  void set_focused(bool) override {}
  void set_editing(bool) override {}

 protected:
  lv_obj_t *label_;
  const char *format_;
  const char *entity_id_{nullptr};
  float ha_value_{0.0f};
  bool has_value_{false};
};

}  // namespace page
}  // namespace esphome
