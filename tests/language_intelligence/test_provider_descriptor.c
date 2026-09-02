/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_provider_descriptor.c
 * PURPOSE: Focused regression test for provider descriptor.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_descriptor.h"


#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceProviderDescriptor value;
    UmiLanguageIntelligenceProviderDescriptor other;
    umi_language_intelligence_provider_descriptor_init(&value, "provider_descriptor.one");
    umi_language_intelligence_types_init_range(&value.range, 1U, 0U, 1U, 5U);
    CHECK(umi_language_intelligence_provider_descriptor_set_subject(&value, "subject") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_provider_descriptor_set_detail(&value, "detail") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_provider_descriptor_validate(&value) == UMI_STATUS_OK);
    umi_language_intelligence_provider_descriptor_init(&other, "provider_descriptor.one");
    umi_language_intelligence_types_init_range(&other.range, 0U, 0U, 0U, 0U);
    CHECK(umi_language_intelligence_provider_descriptor_same_identity(&value, &other) != 0);
    CHECK(value.revision >= 3U);
    return 0;
}
