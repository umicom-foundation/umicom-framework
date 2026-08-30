/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/secrets.h
 *
 * PURPOSE:
 *   Define a provider-neutral secret lookup contract.  The foundation release
 *   includes an environment-variable provider and leaves platform credential
 *   managers behind the same stable C ABI for later adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

typedef struct UmiSecretProvider {
    uint32_t structure_size;
    uint32_t abi_version;
    void *instance;
    UmiStatus (*get)(void *instance,
                     const char *secret_name,
                     char *out_value,
                     size_t capacity);
    void (*destroy)(void *instance);
    /* Optional mutation functions are appended to preserve the original ABI
     * prefix. Read-only providers, such as environment variables, leave them
     * NULL and return UMI_STATUS_NOT_IMPLEMENTED through the helpers below. */
    UmiStatus (*set)(void *instance,
                     const char *secret_name,
                     const char *value);
    UmiStatus (*remove)(void *instance,
                        const char *secret_name);
} UmiSecretProvider;

UmiStatus umi_secret_provider_environment(UmiSecretProvider *out_provider);
UmiStatus umi_secret_get(const UmiSecretProvider *provider,
                         const char *secret_name,
                         char *out_value,
                         size_t capacity);
UmiStatus umi_secret_set(const UmiSecretProvider *provider,
                         const char *secret_name,
                         const char *value);
UmiStatus umi_secret_remove(const UmiSecretProvider *provider,
                            const char *secret_name);
void umi_secret_provider_dispose(UmiSecretProvider *provider);
void umi_secret_redact(char *text);
void umi_secret_clear(void *memory, size_t length);

#ifdef __cplusplus
}
#endif

#endif
