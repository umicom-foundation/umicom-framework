/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_types.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include <string.h>
#include "umicom/delivery/types.h"
int main(void) {
    assert(strcmp(umi_delivery_stage_text(UMI_DELIVERY_STAGE_PACKAGE), "package") == 0);
    assert(strcmp(umi_package_format_text(UMI_PACKAGE_ZIP), "zip") == 0);
    assert(strcmp(umi_release_channel_text(UMI_RELEASE_STABLE), "stable") == 0);
    return 0;
}
