/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_command_surface.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
/*
 * Exercise test command surface and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_command_surface(void){
    UmiApplicationCommandSurface s;
    UmiApplicationCommandQuery query = {0};
    UmiApplicationCommandQueryResult result;
    UmiApplicationExperienceDefinition malformed = *test_trader_experience();
    assert(umi_application_command_surface_build(test_trader_experience(),&s)==UMI_STATUS_OK);
    assert(s.command_count>0U);
    /* A hostile or stale catalogue count must fail before the builder reads past its array. */
    malformed.panel_count = UMI_APPLICATION_EXPERIENCE_MAX_PANELS + 1U;
    assert(umi_application_command_surface_build(&malformed,&s)==UMI_STATUS_CAPACITY_EXCEEDED);
    /* A corrupted command surface is treated as unavailable rather than searched unsafely. */
    s.command_count = UMI_APPLICATION_RUNTIME_MAX_COMMANDS + 1U;
    assert(umi_application_command_surface_find(&s,
        UMI_APPLICATION_COMMAND_ACTIVATE_PANEL,"anything") == NULL);
    /* A capability probe can hide unavailable commands from the normal palette. */
    assert(umi_application_command_surface_build(test_trader_experience(), &s) ==
           UMI_STATUS_OK);
    query.structure_size = sizeof(query);
    query.text = "chart";
    query.include_unavailable = false;
    query.capability_probe = test_capability_probe;
    query.capability_probe_data = (void *)"umicom.chart";
    assert(umi_application_command_surface_query(&s, &query, &result) ==
           UMI_STATUS_OK);
    assert(result.match_count == 0U);
    assert(result.unavailable_count > 0U);
    /* A palette may include the same result as disabled so the user sees why it is blocked. */
    query.include_unavailable = true;
    assert(umi_application_command_surface_query(&s, &query, &result) ==
           UMI_STATUS_OK);
    assert(result.match_count > 0U);
    assert(result.available_count == 0U);
    assert(!result.available[0]);
    /* Search is case-insensitive and works without a provider probe for static commands. */
    query.text = "TRADING";
    query.include_unavailable = false;
    query.capability_probe = NULL;
    assert(umi_application_command_surface_query(&s, &query, &result) ==
           UMI_STATUS_OK);
    assert(result.match_count > 0U);
    assert(result.available_count == result.match_count);
    query.structure_size = 0U;
    assert(umi_application_command_surface_query(&s, &query, &result) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
