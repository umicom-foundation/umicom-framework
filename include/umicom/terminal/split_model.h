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

typedef struct UmiTerminalSplitModel UmiTerminalSplitModel;

UmiStatus umi_terminal_split_model_create(const char *initial_session_id,
                                           UmiTerminalSplitModel **out_model);
void umi_terminal_split_model_destroy(UmiTerminalSplitModel *model);
UmiStatus umi_terminal_split_model_split(UmiTerminalSplitModel *model,
                                         uint64_t leaf_id,
                                         const char *new_session_id,
                                         UmiTerminalOrientation orientation,
                                         double ratio,
                                         uint64_t *out_new_leaf_id);
UmiStatus umi_terminal_split_model_close(UmiTerminalSplitModel *model,
                                         uint64_t leaf_id);
UmiStatus umi_terminal_split_model_set_ratio(UmiTerminalSplitModel *model,
                                             uint64_t node_id,
                                             double ratio);
UmiStatus umi_terminal_split_model_at(const UmiTerminalSplitModel *model,
                                      size_t index,
                                      UmiTerminalSplitSnapshot *out_node);
UmiStatus umi_terminal_split_model_find(const UmiTerminalSplitModel *model,
                                        uint64_t node_id,
                                        UmiTerminalSplitSnapshot *out_node);
size_t umi_terminal_split_model_count(const UmiTerminalSplitModel *model);
uint64_t umi_terminal_split_model_root_id(const UmiTerminalSplitModel *model);
uint64_t umi_terminal_split_model_revision(const UmiTerminalSplitModel *model);

#ifdef __cplusplus
}
#endif
#endif
