/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/secret_provider.h
 *
 * PURPOSE:
 *   Register named secret providers and resolve provider-qualified references
 *   without exposing provider implementation types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_SECRET_PROVIDER_H
#define UMICOM_SECURITY_SECRET_PROVIDER_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/security/secrets.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SECRET_PROVIDER_ID_CAPACITY 96U
#define UMI_SECRET_PROVIDER_REGISTRY_MAX 32U
/**
 * Represent the secret provider registry data shared with callers of this public contract.
 */
typedef struct UmiSecretProviderRegistry UmiSecretProviderRegistry;
/**
 * Initialise secret provider registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_secret_provider_registry_create(UmiSecretProviderRegistry **out_registry);
/**
 * Release or reset state held by secret provider registry so the same storage can be
 * reused safely.
 */
void umi_secret_provider_registry_destroy(UmiSecretProviderRegistry *registry);
/**
 * Add secret provider registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_secret_provider_registry_add(UmiSecretProviderRegistry *registry,
                                           const char *provider_id,
                                           UmiSecretProvider *provider);
/**
 * Provide the secret provider registry resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_secret_provider_registry_resolve(const UmiSecretProviderRegistry *registry,
                                               const char *reference,
                                               char *out_value,
                                               size_t capacity);
/**
 * Provide the secret provider registry store operation used by this module and its client
 * applications.
 */
UmiStatus umi_secret_provider_registry_store(
    UmiSecretProviderRegistry *registry,
    const char *reference,
    const char *value);
/**
 * Remove secret provider registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_secret_provider_registry_remove(
    UmiSecretProviderRegistry *registry,
    const char *reference);
/**
 * Return the number of records represented by secret provider registry without changing
 * their state.
 */
size_t umi_secret_provider_registry_count(const UmiSecretProviderRegistry *registry);
#ifdef __cplusplus
}
#endif

#endif
