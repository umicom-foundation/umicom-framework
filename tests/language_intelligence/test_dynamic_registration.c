/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_dynamic_registration.c
 * PURPOSE: Focused regression test for dynamic registration.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/dynamic_registration.h"
#include <string.h>
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceDynamicRegistration catalogue;
    UmiLanguageIntelligenceDynamicRegistrationItem first = {0};
    UmiLanguageIntelligenceDynamicRegistrationItem second = {0};
    const UmiLanguageIntelligenceDynamicRegistrationItem *best;
    umi_language_intelligence_dynamic_registration_init(&catalogue);
    (void)umi_language_intelligence_copy_text(first.id, sizeof(first.id), "one");
    (void)umi_language_intelligence_copy_text(first.label, sizeof(first.label), "One");
    first.enabled = 1; first.priority = 10U; first.fingerprint = 1U;
    (void)umi_language_intelligence_copy_text(second.id, sizeof(second.id), "two");
    second.enabled = 1; second.priority = 20U; second.fingerprint = 2U;
    CHECK(umi_language_intelligence_dynamic_registration_upsert(&catalogue, &first) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_dynamic_registration_upsert(&catalogue, &second) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_dynamic_registration_find(&catalogue, "one") != NULL);
    best = umi_language_intelligence_dynamic_registration_best(&catalogue);
    CHECK(best != NULL && strcmp(best->id, "two") == 0);
    return 0;
}
