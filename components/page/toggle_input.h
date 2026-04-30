#pragma once
#include "bound_input.h"
#include "esphome/components/api/api_server.h"
#include "esphome/components/api/api_pb2.h"

namespace esphome {
namespace page {

class ToggleInput : public BoundInput {
 public:
  explicit ToggleInput(lv_obj_t *sw) : sw_(sw) {
    lv_obj_set_style_bg_color(this->sw_, lv_color_make(0, 0, 0), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(this->sw_, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(this->sw_, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(this->sw_, lv_color_make(0, 0, 0), (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_CHECKED);
    lv_obj_set_style_border_width(this->sw_, 0, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(this->sw_, LV_OPA_TRANSP, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(this->sw_, LV_OPA_TRANSP, (uint32_t)LV_PART_INDICATOR | (uint32_t)LV_STATE_CHECKED);
    lv_obj_set_style_border_width(this->sw_, 0, LV_PART_INDICATOR);
    lv_obj_set_style_border_width(this->sw_, 0, (uint32_t)LV_PART_INDICATOR | (uint32_t)LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(this->sw_, lv_color_make(255, 255, 255), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(this->sw_, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_border_width(this->sw_, 0, LV_PART_KNOB);
    lv_obj_set_style_bg_color(this->sw_, lv_color_make(255, 255, 255), (uint32_t)LV_PART_KNOB | (uint32_t)LV_STATE_CHECKED);
    lv_obj_set_style_border_width(this->sw_, 0, (uint32_t)LV_PART_KNOB | (uint32_t)LV_STATE_CHECKED);
  }

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
    this->apply_checked(this->value_);
  }

  bool current_val() const { return this->value_; }
  bool is_activatable() const override { return true; }

  void set_focused(bool f) override {
    lv_obj_set_style_outline_width(this->sw_, f ? 3 : 0, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(this->sw_, lv_color_make(0, 0, 0), (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(this->sw_, 2, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
  }
  void set_editing(bool) override {}

  void activate() override {
    this->value_ = !this->value_;
    this->apply_checked(this->value_);
  }

  void save() override {
    if (!this->entity_id_ || !api::global_api_server)
      return;
    api::HomeassistantActionRequest req;
    req.service = StringRef(this->value_ ? "switch.turn_on" : "switch.turn_off");
    req.data.init(1);
    auto &kv = req.data.emplace_back();
    kv.key = StringRef("entity_id");
    kv.value = StringRef(this->entity_id_);
    api::global_api_server->send_homeassistant_action(req);
  }

 protected:
  lv_obj_t *sw_;
  bool value_{false};
  bool ha_value_{false};
  const char *entity_id_{nullptr};

  void apply_checked(bool v) {
    if (v)
      lv_obj_add_state(this->sw_, LV_STATE_CHECKED);
    else
      lv_obj_clear_state(this->sw_, LV_STATE_CHECKED);
  }
};

}  // namespace page
}  // namespace esphome
