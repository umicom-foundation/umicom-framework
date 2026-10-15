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
#include <stdint.h>
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
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
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
#endif

UmiStatus umi_developer_terminal_split_model_add(
    UmiDeveloperTerminalSplitModel *model,
    const UmiDeveloperTerminalSplit *split)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (model != NULL && model->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

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
