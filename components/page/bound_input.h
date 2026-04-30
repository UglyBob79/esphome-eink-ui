#pragma once
#include <lvgl.h>

namespace esphome {
namespace page {

class BoundInput {
 public:
  virtual ~BoundInput() = default;
  virtual void subscribe_ha_state() {}
  virtual void sync_from_cache() = 0;
  virtual void set_focused(bool focused) = 0;
  virtual void set_editing(bool editing) = 0;
  virtual bool is_editable() const { return false; }
  virtual bool is_activatable() const { return false; }
  virtual void increment() {}
  virtual void decrement() {}
  virtual void activate() {}
  virtual void save() {}

  void set_page_active_ptr(const bool *ptr) { this->page_active_ = ptr; }

 protected:
  const bool *page_active_{nullptr};
};

}  // namespace page
}  // namespace esphome
