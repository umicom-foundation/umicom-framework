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

void umi_integration_suite_init(
    UmiIntegrationSuiteDefinition *suite,
    const char *id,
    const char *name)
{
    if (suite == NULL) {
        return;
    }
    (void)memset(suite, 0, sizeof(*suite));
    if (id != NULL) {
        (void)snprintf(suite->id, sizeof(suite->id), "%s", id);
    }
    if (name != NULL) {
        (void)snprintf(suite->name, sizeof(suite->name), "%s", name);
    }
}

UmiStatus umi_integration_suite_add_member(
    UmiIntegrationSuiteDefinition *suite,
    const char *application_id,
    UmiIntegrationDependencyKind kind,
    unsigned preferred_frontend)
{
    size_t index;
    UmiIntegrationSuiteMember *member;
    int written;

    if (suite == NULL || application_id == NULL ||
        application_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < suite->member_count; ++index) {
        if (strcmp(suite->members[index].application_id,
                   application_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (suite->member_count >= UMI_INTEGRATION_MAX_MEMBERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    member = &suite->members[suite->member_count];
    written = snprintf(member->application_id,
                       sizeof(member->application_id),
                       "%s",
                       application_id);
    if (written < 0 ||
        (size_t)written >= sizeof(member->application_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    member->kind = kind;
    member->preferred_frontend = preferred_frontend;
    ++suite->member_count;
    return UMI_STATUS_OK;
}
