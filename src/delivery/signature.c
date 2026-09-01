/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/signature.c
 *
 * PURPOSE:
 *   Implement a detached artifact signature and signer identity without choosing one cryptographic implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Signatures prove publisher identity and artifact authenticity; the Framework record stays provider-neutral.
 */

#include "umicom/delivery/signature.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_signature_record_init(UmiSignatureRecord *record,
                                    const char *artifact_id,
                                    const char *signer_id,
                                    const char *algorithm,
                                    const char *signature)
{
    UmiStatus status;
    if (record == NULL || artifact_id == NULL || signer_id == NULL ||
        algorithm == NULL || signature == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(record, 0, sizeof(*record));
    status = umi_delivery_copy_text(record->artifact_id, sizeof(record->artifact_id), artifact_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(record->signer_id, sizeof(record->signer_id), signer_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_delivery_copy_text(record->algorithm, sizeof(record->algorithm), algorithm);
    if (status != UMI_STATUS_OK) return status;
    return umi_delivery_copy_text(record->signature, sizeof(record->signature), signature);
}
