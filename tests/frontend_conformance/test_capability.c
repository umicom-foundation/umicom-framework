/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_capability.c
 *
 * PURPOSE:
 *   Focused regression coverage for individual semantic frontend capability descriptors and capability bit mapping.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/capability.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcCapability c;
    CHECK(umi_fc_capability_make(UMI_FC_CAP_DOCK, "dock", "Docking", true, &c) == UMI_STATUS_OK);
    CHECK(umi_fc_capability_bit(UMI_FC_CAP_DOCK) != 0U);
    CHECK(c.essential && c.kind == UMI_FC_CAP_DOCK);
    return 0;
}
