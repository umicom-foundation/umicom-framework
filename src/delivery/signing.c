/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/signing.c
 *
 * PURPOSE:
 *   Implement a stable C signing-provider interface for platform or external signing systems.
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

#include "umicom/delivery/signing.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise signing provider from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_signing_provider_init(UmiSigningProvider *provider,
                                    const char *provider_id,
                                    void *instance,
                                    UmiSignFn sign)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL || provider_id == NULL || sign == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(provider, 0, sizeof(*provider));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_delivery_copy_text(provider->provider_id,
                               sizeof(provider->provider_id),
                               provider_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    provider->instance = instance;
    provider->sign = sign;
    return UMI_STATUS_OK;
}

/*
 * Provide the signing provider sign operation used by this module and its client
 * applications.
 */
UmiStatus umi_signing_provider_sign(const UmiSigningProvider *provider,
                                    const char *artifact_id,
                                    const char *digest,
                                    UmiSignatureRecord *signature)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL || provider->sign == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return provider->sign(provider->instance, artifact_id, digest, signature);
}
