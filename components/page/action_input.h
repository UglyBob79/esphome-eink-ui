#pragma once
#include "bound_input.h"

namespace esphome {
namespace page {

class Page;  // forward declaration

class ActionInput : public BoundInput {
 public:
  explicit ActionInput(lv_obj_t *btn, Page *page) : btn_(btn), page_(page) {}

  void sync_from_cache() override {}
  bool is_activatable() const override { return true; }

  void set_focused(bool f) override {
    lv_obj_set_style_bg_color(this->btn_, f ? lv_color_make(0, 0, 0) : lv_color_make(255, 255, 255),
                               (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_t *lbl = lv_obj_get_child(this->btn_, 0);
    if (lbl)
      lv_obj_set_style_text_color(lbl, f ? lv_color_make(255, 255, 255) : lv_color_make(0, 0, 0),
                                   (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
  }
  void set_editing(bool) override {}
  void activate() override;

 protected:
  lv_obj_t *btn_;
  Page *page_;
};

}  // namespace page
}  // namespace esphome
