/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/test_scenario_matrix_02.c
 *
 * PURPOSE:
 *   Validate stable cross-application scenario metadata and bounded identifiers.
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
#include "context_channel_scenarios.h"
int main(void)
{
    size_t index; size_t matches=0U;
    for(index=1U; index<umi_context_scenario_fixture_count; index+=10U){
        const UmiContextScenarioFixture *fixture=&umi_context_scenario_fixtures[index];
        assert(fixture->scenario_id!=NULL);
        assert(fixture->source_application!=NULL);
        assert(fixture->target_application!=NULL);
        assert(fixture->schema_id!=NULL);
        assert(strlen(fixture->channel_id)>0U);
        assert(fixture->expected_delivery==1);
        matches+=1U;
    }
    assert(matches>0U);
    return 0;
}
