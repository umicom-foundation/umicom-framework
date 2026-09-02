/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/split_model.h
 *
 * PURPOSE:
 *   Represent terminal pane splits as an explicit bounded tree suitable for
 *   persistence and rendering by multiple frontend toolkits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_SPLIT_MODEL_H
#define UMICOM_TERMINAL_SPLIT_MODEL_H

#include "umicom/base/status.h"
#include "umicom/terminal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the terminal split snapshot data shared with callers of this public contract.
 */
typedef struct UmiTerminalSplitSnapshot {
    uint64_t node_id;
    uint64_t parent_id;
    uint64_t first_child_id;
    uint64_t second_child_id;
    char session_id[UMI_TERMINAL_ID_CAPACITY];
    UmiTerminalOrientation orientation;
    double ratio;
    int leaf;
} UmiTerminalSplitSnapshot;

/**
 * Represent the terminal split model data shared with callers of this public contract.
 */
typedef struct UmiTerminalSplitModel UmiTerminalSplitModel;

/**
 * Initialise terminal split model from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_terminal_split_model_create(const char *initial_session_id,
                                           UmiTerminalSplitModel **out_model);
/**
 * Release or reset state held by terminal split model so the same storage can be reused
 * safely.
 */
void umi_terminal_split_model_destroy(UmiTerminalSplitModel *model);
/**
 * Provide the terminal split model split operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_split_model_split(UmiTerminalSplitModel *model,
                                         uint64_t leaf_id,
                                         const char *new_session_id,
                                         UmiTerminalOrientation orientation,
                                         double ratio,
                                         uint64_t *out_new_leaf_id);
/**
 * Provide the terminal split model close operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_split_model_close(UmiTerminalSplitModel *model,
                                         uint64_t leaf_id);
/**
 * Provide the terminal split model set ratio operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_split_model_set_ratio(UmiTerminalSplitModel *model,
                                             uint64_t node_id,
                                             double ratio);
/**
 * Find terminal split model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_terminal_split_model_at(const UmiTerminalSplitModel *model,
                                      size_t index,
                                      UmiTerminalSplitSnapshot *out_node);
/**
 * Find terminal split model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_terminal_split_model_find(const UmiTerminalSplitModel *model,
                                        uint64_t node_id,
                                        UmiTerminalSplitSnapshot *out_node);
/**
 * Return the number of records represented by terminal split model without changing their
 * state.
 */
size_t umi_terminal_split_model_count(const UmiTerminalSplitModel *model);
/**
 * Provide the terminal split model root id operation used by this module and its client
 * applications.
 */
uint64_t umi_terminal_split_model_root_id(const UmiTerminalSplitModel *model);
/**
 * Provide the terminal split model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_terminal_split_model_revision(const UmiTerminalSplitModel *model);

#ifdef __cplusplus
}
#endif
#endif
