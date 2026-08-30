/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/signature.h
 *
 * PURPOSE:
 *   Represent a detached artifact signature and signer identity without choosing one cryptographic implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Signatures prove publisher identity and artifact authenticity; the Framework record stays provider-neutral.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_SIGNATURE_H
#define INCLUDE_UMICOM_DELIVERY_SIGNATURE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiSignatureRecord {
    char artifact_id[UMI_DELIVERY_ID_CAPACITY];
    char signer_id[UMI_DELIVERY_ID_CAPACITY];
    char algorithm[UMI_DELIVERY_ID_CAPACITY];
    char signature[UMI_DELIVERY_TEXT_CAPACITY];
    int verified;
} UmiSignatureRecord;

UmiStatus umi_signature_record_init(UmiSignatureRecord *record,
                                    const char *artifact_id,
                                    const char *signer_id,
                                    const char *algorithm,
                                    const char *signature);

#ifdef __cplusplus
}
#endif

#endif
