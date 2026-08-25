/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_capability_set.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded capability-set operations for renderer and surface comparisons.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/capability_set.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcCapabilitySet a,b; umi_fc_capability_set_clear(&a); umi_fc_capability_set_clear(&b);
    CHECK(umi_fc_capability_set_add(&a, UMI_FC_CAP_DOCK) == UMI_STATUS_OK); CHECK(umi_fc_capability_set_add(&a, UMI_FC_CAP_TABS) == UMI_STATUS_OK); CHECK(umi_fc_capability_set_add(&b, UMI_FC_CAP_DOCK) == UMI_STATUS_OK); CHECK(umi_fc_capability_set_count(&a)==2U); CHECK(umi_fc_capability_set_missing(&a,&b)!=0U);
    return 0;
}
