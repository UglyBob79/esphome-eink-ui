#pragma once
#include "bound_input.h"
#include "esphome/components/api/api_server.h"
#include "esphome/components/api/api_pb2.h"

namespace esphome {
namespace page {

class CheckboxInput : public BoundInput {
 public:
  explicit CheckboxInput(lv_obj_t *cb) : cb_(cb) {}

  void set_entity_id(const char *entity_id) { this->entity_id_ = entity_id; }

  void subscribe_ha_state() override {
    if (!this->entity_id_ || !api::global_api_server)
      return;
    api::global_api_server->subscribe_home_assistant_state(
        this->entity_id_, nullptr, [this](StringRef state) {
          std::string s = state.str();
          this->ha_value_ = (s == "on" || s == "ON" || s == "true" || s == "1");
          if (this->page_active_ && *this->page_active_)
            this->sync_from_cache();
        });
  }

  void sync_from_cache() override {
    this->value_ = this->ha_value_;
    if (this->value_)
      lv_obj_add_state(this->cb_, LV_STATE_CHECKED);
    else
      lv_obj_clear_state(this->cb_, LV_STATE_CHECKED);
  }

  bool is_activatable() const override { return true; }

  void set_focused(bool f) override {
    lv_obj_set_style_outline_width(this->cb_, f ? 3 : 0, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(this->cb_, lv_color_make(0, 0, 0), (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(this->cb_, 2, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
  }
  void set_editing(bool) override {}

  void activate() override {
    this->value_ = !this->value_;
    if (this->value_)
      lv_obj_add_state(this->cb_, LV_STATE_CHECKED);
    else
      lv_obj_clear_state(this->cb_, LV_STATE_CHECKED);
  }

  void save() override {
    if (!this->entity_id_ || !api::global_api_server)
      return;
    api::HomeassistantActionRequest req;
    req.service = StringRef(this->value_ ? "homeassistant.turn_on" : "homeassistant.turn_off");
    req.data.init(1);
    auto &kv = req.data.emplace_back();
    kv.key = StringRef("entity_id");
    kv.value = StringRef(this->entity_id_);
    api::global_api_server->send_homeassistant_action(req);
  }

 protected:
  lv_obj_t *cb_;
  bool value_{false};
  bool ha_value_{false};
  const char *entity_id_{nullptr};
};

}  // namespace page
}  // namespace esphome
