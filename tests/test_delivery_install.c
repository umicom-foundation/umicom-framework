/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_install.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/install_plan.h"
#include "umicom/delivery/install_state.h"
int main(void) {
    UmiInstallPlan plan;
    UmiInstallState state;
    umi_install_plan_init(&plan);
    assert(umi_install_plan_add(&plan, UMI_INSTALL_COPY_FILE, "a", "b") == UMI_STATUS_OK);
    assert(plan.count == 1U);
    assert(umi_install_state_init(&state, "studio", "0.14.0", 4U, "/opt/studio") == UMI_STATUS_OK);
    assert(state.generation == 4U);
    return 0;
}
