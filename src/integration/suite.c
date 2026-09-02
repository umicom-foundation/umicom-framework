/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/suite.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/suite.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise integration suite from caller-provided values so later operations receive a
 * known state.
 */
void umi_integration_suite_init(
    UmiIntegrationSuiteDefinition *suite,
    const char *id,
    const char *name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (suite == NULL) {
        return;
    }
    (void)memset(suite, 0, sizeof(*suite));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) {
        (void)snprintf(suite->id, sizeof(suite->id), "%s", id);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (name != NULL) {
        (void)snprintf(suite->name, sizeof(suite->name), "%s", name);
    }
}

/*
 * Provide the integration suite add member operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_suite_add_member(
    UmiIntegrationSuiteDefinition *suite,
    const char *application_id,
    UmiIntegrationDependencyKind kind,
    unsigned preferred_frontend)
{
    size_t index;
    UmiIntegrationSuiteMember *member;
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (suite == NULL || application_id == NULL ||
        application_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < suite->member_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(suite->members[index].application_id,
                   application_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (suite->member_count >= UMI_INTEGRATION_MAX_MEMBERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    member = &suite->members[suite->member_count];
    written = snprintf(member->application_id,
                       sizeof(member->application_id),
                       "%s",
                       application_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (written < 0 ||
        (size_t)written >= sizeof(member->application_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    member->kind = kind;
    member->preferred_frontend = preferred_frontend;
    ++suite->member_count;
    return UMI_STATUS_OK;
}
