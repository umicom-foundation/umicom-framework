/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/tab_model.h
 *
 * PURPOSE:
 *   Maintain terminal tabs, ordering, active selection, pinning and unread
 *   state independently of GTK, Qt, Wt or browser frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_TAB_MODEL_H
#define UMICOM_TERMINAL_TAB_MODEL_H

#include "umicom/terminal/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTerminalTabSnapshot {
    char session_id[UMI_TERMINAL_ID_CAPACITY];
    char title[UMI_TERMINAL_TITLE_CAPACITY];
    char profile_id[UMI_TERMINAL_ID_CAPACITY];
    size_t order;
    uint64_t unread_lines;
    int active;
    int pinned;
} UmiTerminalTabSnapshot;

typedef struct UmiTerminalTabModel UmiTerminalTabModel;

UmiStatus umi_terminal_tab_model_create(UmiTerminalTabModel **out_model);
void umi_terminal_tab_model_destroy(UmiTerminalTabModel *model);
UmiStatus umi_terminal_tab_model_add(UmiTerminalTabModel *model,
                                     const char *session_id,
                                     const char *title,
                                     const char *profile_id);
UmiStatus umi_terminal_tab_model_remove(UmiTerminalTabModel *model,
                                        const char *session_id);
UmiStatus umi_terminal_tab_model_activate(UmiTerminalTabModel *model,
                                          const char *session_id);
UmiStatus umi_terminal_tab_model_activate_relative(UmiTerminalTabModel *model,
                                                   int direction);
UmiStatus umi_terminal_tab_model_move(UmiTerminalTabModel *model,
                                      const char *session_id,
                                      size_t target_index);
UmiStatus umi_terminal_tab_model_set_pinned(UmiTerminalTabModel *model,
                                            const char *session_id,
                                            int pinned);
UmiStatus umi_terminal_tab_model_add_unread(UmiTerminalTabModel *model,
                                            const char *session_id,
                                            uint64_t lines);
UmiStatus umi_terminal_tab_model_at(const UmiTerminalTabModel *model,
                                    size_t index,
                                    UmiTerminalTabSnapshot *out_tab);
UmiStatus umi_terminal_tab_model_find(const UmiTerminalTabModel *model,
                                      const char *session_id,
                                      UmiTerminalTabSnapshot *out_tab);
size_t umi_terminal_tab_model_count(const UmiTerminalTabModel *model);
const char *umi_terminal_tab_model_active_id(const UmiTerminalTabModel *model);
uint64_t umi_terminal_tab_model_revision(const UmiTerminalTabModel *model);

#ifdef __cplusplus
}
#endif
#endif
