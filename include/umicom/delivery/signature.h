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

/*
 * Signatures prove publisher identity and artifact authenticity; the Framework record stays provider-neutral.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_SIGNATURE_H
#define INCLUDE_UMICOM_DELIVERY_SIGNATURE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the signature record data shared with callers of this public contract.
 */
typedef struct UmiSignatureRecord {
    char artifact_id[UMI_DELIVERY_ID_CAPACITY];
    char signer_id[UMI_DELIVERY_ID_CAPACITY];
    char algorithm[UMI_DELIVERY_ID_CAPACITY];
    char signature[UMI_DELIVERY_TEXT_CAPACITY];
    int verified;
} UmiSignatureRecord;

/**
 * Initialise signature record from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_signature_record_init(UmiSignatureRecord *record,
                                    const char *artifact_id,
                                    const char *signer_id,
                                    const char *algorithm,
                                    const char *signature);

#ifdef __cplusplus
}
#endif

#endif
