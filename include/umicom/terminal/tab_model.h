/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/tab_model.h
 *
 * PURPOSE:
 *   Maintain terminal tabs, ordering, active selection, pinning and unread
 *   state independently of GTK, Qt, Wt or browser frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_TAB_MODEL_H
#define UMICOM_TERMINAL_TAB_MODEL_H

#include "umicom/terminal/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the terminal tab snapshot data shared with callers of this public contract.
 */
typedef struct UmiTerminalTabSnapshot {
    char session_id[UMI_TERMINAL_ID_CAPACITY];
    char title[UMI_TERMINAL_TITLE_CAPACITY];
    char profile_id[UMI_TERMINAL_ID_CAPACITY];
    size_t order;
    uint64_t unread_lines;
    int active;
    int pinned;
} UmiTerminalTabSnapshot;

/**
 * Represent the terminal tab model data shared with callers of this public contract.
 */
typedef struct UmiTerminalTabModel UmiTerminalTabModel;

/**
 * Initialise terminal tab model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_terminal_tab_model_create(UmiTerminalTabModel **out_model);
/**
 * Release or reset state held by terminal tab model so the same storage can be reused
 * safely.
 */
void umi_terminal_tab_model_destroy(UmiTerminalTabModel *model);
/**
 * Add terminal tab model only after its inputs and available capacity have been checked.
 */
UmiStatus umi_terminal_tab_model_add(UmiTerminalTabModel *model,
                                     const char *session_id,
                                     const char *title,
                                     const char *profile_id);
/**
 * Remove terminal tab model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_terminal_tab_model_remove(UmiTerminalTabModel *model,
                                        const char *session_id);
/**
 * Provide the terminal tab model activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_tab_model_activate(UmiTerminalTabModel *model,
                                          const char *session_id);
/**
 * Provide the terminal tab model activate relative operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_tab_model_activate_relative(UmiTerminalTabModel *model,
                                                   int direction);
/**
 * Provide the terminal tab model move operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_tab_model_move(UmiTerminalTabModel *model,
                                      const char *session_id,
                                      size_t target_index);
/**
 * Provide the terminal tab model set pinned operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_tab_model_set_pinned(UmiTerminalTabModel *model,
                                            const char *session_id,
                                            int pinned);
/**
 * Provide the terminal tab model add unread operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_tab_model_add_unread(UmiTerminalTabModel *model,
                                            const char *session_id,
                                            uint64_t lines);
/**
 * Find terminal tab model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_terminal_tab_model_at(const UmiTerminalTabModel *model,
                                    size_t index,
                                    UmiTerminalTabSnapshot *out_tab);
/**
 * Find terminal tab model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_terminal_tab_model_find(const UmiTerminalTabModel *model,
                                      const char *session_id,
                                      UmiTerminalTabSnapshot *out_tab);
/**
 * Return the number of records represented by terminal tab model without changing their
 * state.
 */
size_t umi_terminal_tab_model_count(const UmiTerminalTabModel *model);
/**
 * Provide the terminal tab model active id operation used by this module and its client
 * applications.
 */
const char *umi_terminal_tab_model_active_id(const UmiTerminalTabModel *model);
/**
 * Provide the terminal tab model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_terminal_tab_model_revision(const UmiTerminalTabModel *model);

#ifdef __cplusplus
}
#endif
#endif
