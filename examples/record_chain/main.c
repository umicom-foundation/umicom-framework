/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/record_chain/main.c
 * PURPOSE: Demonstrate receipt interchange and verification against a retained checkpoint.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/record_chain/archive.h"
#include <stdio.h>

int main(void)
{
    /* Synthetic identity and bytes are demonstration fixtures. Real hosts own
     * source identities, canonical schemas and independently retained anchors. */
    const unsigned char chain_id[32] = {42U};
    const char payload[] = "synthetic transaction";
    unsigned char envelope[UMI_RECORD_CHAIN_RECORD_BYTES];
    UmiRecordChainCheckpoint origin, tip, verifier;
    UmiRecordChainRecord record, decoded;
    UmiRecordChainInput input;
    char hex[65];
    if (umi_record_chain_start(chain_id, &origin) != UMI_STATUS_OK) return 1;
    tip = verifier = origin;
    if (umi_record_chain_append(&tip, 1U, payload, sizeof payload - 1U, &record) != UMI_STATUS_OK) return 2;
    if (umi_record_chain_record_encode(&record, envelope, sizeof envelope) != UMI_STATUS_OK) return 3;
    if (umi_record_chain_record_decode(envelope, sizeof envelope, &decoded) != UMI_STATUS_OK) return 4;
    input = (UmiRecordChainInput){&decoded, payload, sizeof payload - 1U};
    if (umi_record_chain_verify(&verifier, &input, 1U, &tip) != UMI_STATUS_OK) return 5;
    if (!umi_record_chain_checkpoint_equal(&verifier, &tip)) return 6;
    UmiSha256Hex(tip.digest, hex);
    printf("Verified one synthetic record against its retained checkpoint: %s\n", hex);
    return 0;
}
