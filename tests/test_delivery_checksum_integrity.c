/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_checksum_integrity.c
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
#include "umicom/delivery/checksum.h"
#include "umicom/delivery/integrity.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    char a[UMI_DELIVERY_DIGEST_CAPACITY];
    char b[UMI_DELIVERY_DIGEST_CAPACITY];
    UmiIntegrityResult result;
    assert(umi_checksum_text("umicom", a, sizeof(a)) == UMI_STATUS_OK);
    assert(umi_checksum_text("umicom", b, sizeof(b)) == UMI_STATUS_OK);
    assert(strcmp(a,b) == 0);
    assert(umi_integrity_verify(&result, "artifact", a, b) == UMI_STATUS_OK);
    assert(result.status == UMI_EVIDENCE_PASS);
    return 0;
}
