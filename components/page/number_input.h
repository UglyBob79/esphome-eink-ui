#pragma once
#include "bound_input.h"
#include "esphome/core/helpers.h"
#include "esphome/components/api/api_server.h"
#include "esphome/components/api/api_pb2.h"

namespace esphome {
namespace page {

class NumberInput : public BoundInput {
 public:
  NumberInput(lv_obj_t *box, lv_obj_t *label, int min_val, int max_val)
      : box_(box), label_(label), min_val_(min_val), max_val_(max_val) {}

  void set_entity_id(const char *entity_id) { this->entity_id_ = entity_id; }

  void subscribe_ha_state() override {
    if (!this->entity_id_ || !api::global_api_server)
      return;
    api::global_api_server->subscribe_home_assistant_state(
        this->entity_id_, nullptr, [this](StringRef state) {
          auto val = parse_number<float>(state.c_str());
          if (val.has_value()) {
            this->ha_value_ = (int) *val;
            if (this->page_active_ && *this->page_active_)
              this->sync_from_cache();
          }
        });
  }

  void sync_from_cache() override {
    this->value_ = this->ha_value_;
    this->update_label(this->value_);
  }

  int current_val() const { return this->value_; }
  bool is_editable() const override { return true; }

  void set_focused(bool f) override { this->apply_state(f ? 1 : 0); }
  void set_editing(bool e) override { this->apply_state(e ? 2 : 1); }

  void increment() override {
    int range = this->max_val_ - this->min_val_ + 1;
    this->value_ = this->min_val_ + (this->value_ - this->min_val_ + range - 1) % range;
    this->update_label(this->value_);
  }

  void decrement() override {
    int range = this->max_val_ - this->min_val_ + 1;
    this->value_ = this->min_val_ + (this->value_ - this->min_val_ + 1) % range;
    this->update_label(this->value_);
  }

  void save() override {
    if (!this->entity_id_ || !api::global_api_server)
      return;
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", this->value_);
    api::HomeassistantActionRequest req;
    req.service = StringRef("number.set_value");
    req.data.init(2);
    auto &kv0 = req.data.emplace_back();
    kv0.key = StringRef("entity_id");
    kv0.value = StringRef(this->entity_id_);
    auto &kv1 = req.data.emplace_back();
    kv1.key = StringRef("value");
    kv1.value = StringRef(buf);
    api::global_api_server->send_homeassistant_action(req);
  }

 protected:
  lv_obj_t *box_;
  lv_obj_t *label_;
  int min_val_;
  int max_val_;
  int value_{0};
  int ha_value_{0};
  const char *entity_id_{nullptr};

  void update_label(int v) {
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d", v);
    lv_label_set_text(this->label_, buf);
  }

  void apply_state(int state) {
    lv_color_t bg = (state == 2) ? lv_color_make(0, 0, 0) : lv_color_make(255, 255, 255);
    lv_color_t fg = (state == 2) ? lv_color_make(255, 255, 255) : lv_color_make(0, 0, 0);
    lv_obj_set_style_bg_color(this->box_, bg, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(this->label_, fg, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(this->box_, (state == 1) ? 4 : 2, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
  }
};

}  // namespace page
}  // namespace esphome
