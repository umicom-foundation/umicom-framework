/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/secrets.c
 *
 * PURPOSE:
 *   Implement the provider-neutral secret helper and an environment-variable
 *   provider for development.  Secret values are copied into caller-owned
 *   buffers and are never retained by the provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/secrets.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/base/version.h"

static UmiStatus umi_environment_secret_get(void *instance,
                                             const char *secret_name,
                                             char *out_value,
                                             size_t capacity)
{
    const char *value;
    size_t length;
    (void)instance;

    if (secret_name == NULL || out_value == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    value = getenv(secret_name);
    if (value == NULL) {
        out_value[0] = '\0';
        return UMI_STATUS_NOT_FOUND;
    }

    length = strlen(value);
    if (length + 1U > capacity) {
        out_value[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_value, value, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_secret_provider_environment(UmiSecretProvider *out_provider)
{
    if (out_provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_provider, 0, sizeof(*out_provider));
    out_provider->structure_size = (uint32_t)sizeof(*out_provider);
    out_provider->abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
    out_provider->get = umi_environment_secret_get;
    return UMI_STATUS_OK;
}

UmiStatus umi_secret_get(const UmiSecretProvider *provider,
                         const char *secret_name,
                         char *out_value,
                         size_t capacity)
{
    if (provider == NULL ||
        provider->structure_size < sizeof(*provider) ||
        provider->abi_version != UMICOM_FRAMEWORK_ABI_VERSION ||
        provider->get == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return provider->get(provider->instance,
                         secret_name,
                         out_value,
                         capacity);
}

void umi_secret_provider_dispose(UmiSecretProvider *provider)
{
    if (provider == NULL) {
        return;
    }
    if (provider->destroy != NULL) {
        provider->destroy(provider->instance);
    }
    (void)memset(provider, 0, sizeof(*provider));
}

void umi_secret_redact(char *text)
{
    if (text == NULL) {
        return;
    }
    while (*text != '\0') {
        *text = '*';
        ++text;
    }
}
