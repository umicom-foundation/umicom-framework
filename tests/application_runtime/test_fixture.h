/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_fixture.h
 *
 * PURPOSE:
 *   Provide shared canonical experience lookups and a deterministic capability probe for runtime tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_TEST_FIXTURE_H
#define UMICOM_APPLICATION_RUNTIME_TEST_FIXTURE_H

#include <assert.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/runtime/runtime.h"
#include "umicom/application_ui/application_ui.h"

static inline const UmiApplicationExperienceDefinition *test_trader_experience(void)
{
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find("org.umicom.trader");
    assert(experience != NULL);
    return experience;
}

static inline int test_capability_probe(const char *capability_id, void *user_data)
{
    const char *blocked = (const char *)user_data;
    return blocked == NULL || capability_id == NULL || strcmp(blocked, capability_id) != 0;
}

#endif
