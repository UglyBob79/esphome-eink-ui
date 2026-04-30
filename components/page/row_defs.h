#pragma once
#include <stdint.h>

namespace esphome {
namespace page {

enum class RowType : uint8_t { TOGGLE, TIME_INPUT, SENSOR, LABEL, CHECKBOX };

struct ToggleRowDef {
  const char *label;
  const char *entity_id;
};

struct TimeInputRowDef {
  const char *label;
  const char *entity_hour_id;
  const char *entity_min_id;
};

struct SensorRowDef {
  const char *entity_id;
  const char *format;
};

struct LabelRowDef {
  const char *text;
};

struct CheckboxRowDef {
  const char *label;
  const char *entity_id;
};

struct RowDef {
  RowType type;
  ToggleRowDef toggle{};
  TimeInputRowDef time_input{};
  SensorRowDef sensor{};
  LabelRowDef label_row{};
  CheckboxRowDef checkbox{};
};

}  // namespace page
}  // namespace esphome
