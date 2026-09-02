/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/signing.h
 *
 * PURPOSE:
 *   Define a stable C signing-provider interface for platform or external signing systems.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The Framework controls when signing is required while a provider owns the actual certificate, key or external signing process.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_SIGNING_H
#define INCLUDE_UMICOM_DELIVERY_SIGNING_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/signature.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiSignFn)(void *instance,
                               const char *artifact_id,
                               const char *digest,
                               UmiSignatureRecord *signature);

/**
 * Represent the signing provider data shared with callers of this public contract.
 */
typedef struct UmiSigningProvider {
    char provider_id[UMI_DELIVERY_ID_CAPACITY];
    void *instance;
    UmiSignFn sign;
} UmiSigningProvider;

/**
 * Initialise signing provider from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_signing_provider_init(UmiSigningProvider *provider,
                                    const char *provider_id,
                                    void *instance,
                                    UmiSignFn sign);
/**
 * Provide the signing provider sign operation used by this module and its client
 * applications.
 */
UmiStatus umi_signing_provider_sign(const UmiSigningProvider *provider,
                                    const char *artifact_id,
                                    const char *digest,
                                    UmiSignatureRecord *signature);

#ifdef __cplusplus
}
#endif

#endif
