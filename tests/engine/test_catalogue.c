/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/engine/test_catalogue.c
 *
 * PURPOSE:
 *   Verify every reusable engine is discoverable and application requirements
 *   produce truthful readiness evidence.
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

#include "umicom/engine/engine.h"

int main(void)
{
    UmiEngineRequirement requirements[] = {
        {UMI_ENGINE_GAME, UMI_ENGINE_MATURITY_FOUNDATION,
         UMI_ENGINE_CAPABILITY_ENTITY_WORLD},
        {UMI_ENGINE_TRADING_MATCHING, UMI_ENGINE_MATURITY_OPERATIONAL,
         UMI_ENGINE_CAPABILITY_ORDER_MATCHING |
             UMI_ENGINE_CAPABILITY_RISK_CONTROLS}
    };
    UmiEngineRequirementReport report;
    const UmiEngineDescriptor *engine;

    assert(umi_engine_catalogue_count() == 8U);
    engine = umi_engine_catalogue_find_id("umicom.render3d");
    assert(engine != NULL);
    assert(strcmp(engine->display_name, "3D Rendering Engine") == 0);
    assert(umi_engine_descriptor_supports(
        engine, UMI_ENGINE_CAPABILITY_SCENE_GRAPH));
    assert(umi_engine_catalogue_validate(
        requirements, 2U, &report) == UMI_STATUS_OK);
    assert(report.ready);
    assert(report.satisfied_count == 2U);
    assert(report.missing_count == 0U);

    requirements[0].minimum_maturity = UMI_ENGINE_MATURITY_OPERATIONAL;
    assert(umi_engine_catalogue_validate(
        requirements, 1U, &report) == UMI_STATUS_OK);
    assert(!report.ready);
    assert(report.missing_count == 1U);
    return 0;
}
