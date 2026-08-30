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
typedef struct UmiSecretProviderRegistry UmiSecretProviderRegistry;
UmiStatus umi_secret_provider_registry_create(UmiSecretProviderRegistry **out_registry);
void umi_secret_provider_registry_destroy(UmiSecretProviderRegistry *registry);
UmiStatus umi_secret_provider_registry_add(UmiSecretProviderRegistry *registry,
                                           const char *provider_id,
                                           UmiSecretProvider *provider);
UmiStatus umi_secret_provider_registry_resolve(const UmiSecretProviderRegistry *registry,
                                               const char *reference,
                                               char *out_value,
                                               size_t capacity);
size_t umi_secret_provider_registry_count(const UmiSecretProviderRegistry *registry);
#ifdef __cplusplus
}
#endif

#endif
