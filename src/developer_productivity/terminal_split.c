/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/terminal_split.c
 *
 * PURPOSE:
 *   Implement portable terminal split-layout records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/terminal_split.h"

#include <string.h>

/*
 * Initialise developer terminal split model from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_terminal_split_model_init(
    UmiDeveloperTerminalSplitModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->revision = 1U;
}

/*
 * Add developer terminal split model only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_developer_terminal_split_model_add(
    UmiDeveloperTerminalSplitModel *model,
    const UmiDeveloperTerminalSplit *split)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || split == NULL ||
        split->split_id[0] == '\0' ||
        split->first_session_id[0] == '\0' ||
        split->second_session_id[0] == '\0' ||
        split->orientation < UMI_DEVELOPER_TERMINAL_SPLIT_HORIZONTAL ||
        split->orientation > UMI_DEVELOPER_TERMINAL_SPLIT_VERTICAL ||
        split->ratio <= 0.0 || split->ratio >= 1.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_DEVELOPER_TERMINAL_SPLIT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    model->splits[model->count++] = *split;
    model->revision += 1U;
    return UMI_STATUS_OK;
}
