/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/secrets.h
 *
 * PURPOSE:
 *   Define a provider-neutral secret lookup contract.  The foundation release
 *   includes an environment-variable provider and leaves platform credential
 *   managers behind the same stable C ABI for later adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_SECRETS_H
#define UMICOM_SECURITY_SECRETS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SECRET_VALUE_CAPACITY 4096U

typedef struct UmiSecretProviderV1 {
    uint32_t structure_size;
    uint32_t abi_version;
    void *instance;
    UmiStatus (*get)(void *instance,
                     const char *secret_name,
                     char *out_value,
                     size_t capacity);
    void (*destroy)(void *instance);
} UmiSecretProviderV1;

UmiStatus umi_secret_provider_environment(UmiSecretProviderV1 *out_provider);
UmiStatus umi_secret_get(const UmiSecretProviderV1 *provider,
                         const char *secret_name,
                         char *out_value,
                         size_t capacity);
void umi_secret_provider_dispose(UmiSecretProviderV1 *provider);
void umi_secret_redact(char *text);

#ifdef __cplusplus
}
#endif

#endif
