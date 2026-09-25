/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/strategy_research/test_types.c
 *
 * PURPOSE:
 *   Verify strategy research input defaults and snapshot validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/strategy_research/types.h"

int main(void)
{
    UmiStrategyResearchInput input;
    UmiStrategyResearchSnapshot snapshot;

    umi_strategy_research_input_init(&input);
    assert(input.trusted == 1);
    assert(input.deterministic == 1);

    umi_strategy_research_snapshot_init(&snapshot);
    (void)strcpy(snapshot.id, "strategy.signal-score");
    snapshot.score = 75.0;
    snapshot.ready = 1;
    assert(umi_strategy_research_snapshot_validate(&snapshot) == UMI_STATUS_OK);

    snapshot.score = 101.0;
    assert(umi_strategy_research_snapshot_validate(&snapshot) ==
           UMI_STATUS_INVALID_STATE);
    return 0;
}
