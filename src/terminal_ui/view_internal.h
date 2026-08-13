/* Private helpers for terminal/process/task view-model projections. */
#ifndef UMICOM_TERMINAL_UI_VIEW_INTERNAL_H
#define UMICOM_TERMINAL_UI_VIEW_INTERNAL_H

#include "umicom/ui/command_view.h"

UmiStatus umi_terminal_ui_base_view(const char *view_id,
                                    const char *kind,
                                    const char *title,
                                    const char *summary,
                                    UmiUiViewModel **out_view);
UmiStatus umi_terminal_ui_set_string(UmiUiViewModel *view,
                                     const char *key,
                                     const char *value);
UmiStatus umi_terminal_ui_set_integer(UmiUiViewModel *view,
                                      const char *key,
                                      int64_t value);
UmiStatus umi_terminal_ui_set_boolean(UmiUiViewModel *view,
                                      const char *key,
                                      int value);
UmiStatus umi_terminal_ui_set_action(UmiUiViewModel *view,
                                     size_t index,
                                     const char *action_id,
                                     const char *label,
                                     const char *tooltip);
#endif
