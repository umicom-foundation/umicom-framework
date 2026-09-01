/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_signature_signing.c
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
#include "umicom/delivery/signing.h"
static UmiStatus fake_sign(void *instance, const char *artifact_id, const char *digest, UmiSignatureRecord *record) {
    (void)instance; (void)digest;
    return umi_signature_record_init(record, artifact_id, "tester", "test", "signed");
}
int main(void) {
    UmiSigningProvider provider;
    UmiSignatureRecord record;
    assert(umi_signing_provider_init(&provider, "test.signer", 0, fake_sign) == UMI_STATUS_OK);
    assert(umi_signing_provider_sign(&provider, "a", "b", &record) == UMI_STATUS_OK);
    return 0;
}
