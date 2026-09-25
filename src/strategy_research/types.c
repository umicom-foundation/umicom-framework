/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/types.c
 *
 * PURPOSE:
 *   Initialise and validate deterministic strategy research evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/types.h"

#include <string.h>

void umi_strategy_research_input_init(UmiStrategyResearchInput *input)
{
    if (input == NULL) return;
    (void)memset(input, 0, sizeof(*input));
    input->trusted = 1;
    input->deterministic = 1;
}

void umi_strategy_research_snapshot_init(UmiStrategyResearchSnapshot *snapshot)
{
    if (snapshot == NULL) return;
    (void)memset(snapshot, 0, sizeof(*snapshot));
}

UmiStatus umi_strategy_research_snapshot_validate(
    const UmiStrategyResearchSnapshot *snapshot)
{
    if (snapshot == NULL || snapshot->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (snapshot->score < 0.0 || snapshot->score > 100.0) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (snapshot->ready && snapshot->blocked) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
