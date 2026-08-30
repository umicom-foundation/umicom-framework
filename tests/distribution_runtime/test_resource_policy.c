/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_resource_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for resource selection rules for locale, scale and optional fallbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/resource_policy.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrResourcePolicy p; umi_dr_resource_policy_init(&p); CHECK(umi_dr_copy_text(p.preferred_locale,sizeof(p.preferred_locale),"en-GB")==UMI_STATUS_OK); p.allow_locale_fallback=false; CHECK(umi_dr_resource_policy_accepts(&p,"en-GB",100U)); CHECK(!umi_dr_resource_policy_accepts(&p,"fr-FR",100U));
    return 0;
}
