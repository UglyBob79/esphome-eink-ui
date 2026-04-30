#pragma once
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "bound_input.h"
#include "number_input.h"
#include "toggle_input.h"
#include "sensor_label.h"
#include "checkbox_input.h"
#include "action_input.h"
#include "row_defs.h"
#include <vector>

static const char *const PAGE_TAG = "page";

namespace esphome {
namespace page {

class Page : public esphome::Component {
 public:
  static Page *active;

  void set_page_obj(lv_obj_t *obj) { this->page_obj_ = obj; }
  void set_title(const char *title) { this->title_ = title; }
  void set_font(const lv_font_t *font) { this->font_ = font; }
  void set_submit_label(const char *label) { this->submit_label_ = label; }

  void add_toggle_row(const char *label, const char *entity_id) {
    RowDef r;
    r.type = RowType::TOGGLE;
    r.toggle = {label, entity_id};
    this->rows_.push_back(r);
  }

  void add_time_input_row(const char *label, const char *entity_hour_id, const char *entity_min_id) {
    RowDef r;
    r.type = RowType::TIME_INPUT;
    r.time_input = {label, entity_hour_id, entity_min_id};
    this->rows_.push_back(r);
  }

  void add_sensor_row(const char *entity_id, const char *format) {
    RowDef r;
    r.type = RowType::SENSOR;
    r.sensor = {entity_id, format};
    this->rows_.push_back(r);
  }

  void add_label_row(const char *text) {
    RowDef r;
    r.type = RowType::LABEL;
    r.label_row = {text};
    this->rows_.push_back(r);
  }

  void add_checkbox_row(const char *label, const char *entity_id) {
    RowDef r;
    r.type = RowType::CHECKBOX;
    r.checkbox = {label, entity_id};
    this->rows_.push_back(r);
  }

  void setup() override {
    this->build_ui();
    for (auto *inp : this->inputs_)
      inp->subscribe_ha_state();
    // Initial LV_EVENT_SCREEN_LOADED fires before LATE setup, so catch it here.
    if (this->page_obj_ && lv_scr_act() == this->page_obj_)
      this->on_page_load();
  }
  void loop() override {}
  float get_setup_priority() const override { return esphome::setup_priority::LATE; }

  void on_page_load();
  void on_page_unload();
  void on_confirm();
  void on_up();
  void on_down();
  void on_submit();

 protected:
  lv_obj_t *page_obj_{nullptr};
  const char *title_{nullptr};
  const lv_font_t *font_{nullptr};
  const char *submit_label_{nullptr};
  bool page_active_{false};

  std::vector<RowDef> rows_;
  std::vector<BoundInput *> inputs_;
  size_t focused_idx_{0};
  bool editing_{false};

  void build_ui();
  void apply_page_active_state_();
  lv_obj_t *make_ticker_box(lv_obj_t *parent, lv_obj_t **label_out);

  static void on_load_event_(lv_event_t *e) {
    static_cast<Page *>(lv_event_get_user_data(e))->on_page_load();
  }
  static void on_unload_event_(lv_event_t *e) {
    static_cast<Page *>(lv_event_get_user_data(e))->on_page_unload();
  }

  void focus(size_t idx) {
    for (size_t i = 0; i < this->inputs_.size(); i++)
      this->inputs_[i]->set_focused(i == idx);
  }

  void navigate(int delta) {
    this->inputs_[this->focused_idx_]->set_focused(false);
    size_t n = this->inputs_.size();
    this->focused_idx_ = (this->focused_idx_ + n + delta) % n;
    this->inputs_[this->focused_idx_]->set_focused(true);
  }
};

}  // namespace page
}  // namespace esphome
