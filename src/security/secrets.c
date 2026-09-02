/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/secrets.c
 *
 * PURPOSE:
 *   Implement the provider-neutral secret helper and an environment-variable
 *   provider for development.  Secret values are copied into caller-owned
 *   buffers and are never retained by the provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/secrets.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/base/version.h"

/*
 * Provide the environment secret get operation used by this module and its client
 * applications.
 */
static UmiStatus umi_environment_secret_get(void *instance,
                                             const char *secret_name,
                                             char *out_value,
                                             size_t capacity)
{
    const char *value;
    size_t length;
    (void)instance;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (secret_name == NULL || out_value == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    value = getenv(secret_name);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) {
        out_value[0] = '\0';
        return UMI_STATUS_NOT_FOUND;
    }

    length = strlen(value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) {
        out_value[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_value, value, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the secret provider environment operation used by this module and its client
 * applications.
 */
UmiStatus umi_secret_provider_environment(UmiSecretProvider *out_provider)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_provider, 0, sizeof(*out_provider));
    out_provider->structure_size = (uint32_t)sizeof(*out_provider);
    out_provider->abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
    out_provider->get = umi_environment_secret_get;
    return UMI_STATUS_OK;
}

/* Provide the secret get operation used by this module and its client applications. */
UmiStatus umi_secret_get(const UmiSecretProvider *provider,
                         const char *secret_name,
                         char *out_value,
                         size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL ||
        provider->structure_size <
            offsetof(UmiSecretProvider, destroy) + sizeof(provider->destroy) ||
        provider->abi_version != UMICOM_FRAMEWORK_ABI_VERSION ||
        provider->get == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return provider->get(provider->instance,
                         secret_name,
                         out_value,
                         capacity);
}

/* Copy secret into module-owned storage so callers keep ownership of their input values. */
UmiStatus umi_secret_set(const UmiSecretProvider *provider,
                         const char *secret_name,
                         const char *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL || secret_name == NULL || secret_name[0] == '\0' ||
        value == NULL ||
        provider->abi_version != UMICOM_FRAMEWORK_ABI_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (provider->structure_size <
        offsetof(UmiSecretProvider, set) + sizeof(provider->set)) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider->set == NULL) return UMI_STATUS_NOT_IMPLEMENTED;
    return provider->set(provider->instance, secret_name, value);
}

/* Remove secret while keeping the remaining records in a valid and discoverable state. */
UmiStatus umi_secret_remove(const UmiSecretProvider *provider,
                            const char *secret_name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL || secret_name == NULL || secret_name[0] == '\0' ||
        provider->abi_version != UMICOM_FRAMEWORK_ABI_VERSION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (provider->structure_size <
        offsetof(UmiSecretProvider, remove) + sizeof(provider->remove)) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider->remove == NULL) return UMI_STATUS_NOT_IMPLEMENTED;
    return provider->remove(provider->instance, secret_name);
}

/* Release or reset state held by secret provider so the same storage can be reused safely. */
void umi_secret_provider_dispose(UmiSecretProvider *provider)
{
    size_t provider_size;
    const size_t original_provider_size =
        offsetof(UmiSecretProvider, destroy) + sizeof(provider->destroy);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL) {
        return;
    }
    provider_size = provider->structure_size < sizeof(*provider)
        ? (size_t)provider->structure_size
        : sizeof(*provider);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (provider->structure_size >= original_provider_size &&
        provider->destroy != NULL) {
        provider->destroy(provider->instance);
    }
    (void)memset(provider, 0, provider_size);
}

/* Provide the secret redact operation used by this module and its client applications. */
void umi_secret_redact(char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) {
        return;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*text != '\0') {
        *text = '*';
        ++text;
    }
}

/* Release or reset state held by secret so the same storage can be reused safely. */
void umi_secret_clear(void *memory, size_t length)
{
    volatile unsigned char *cursor = (volatile unsigned char *)memory;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (memory == NULL) return;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (length > 0U) {
        *cursor = 0U;
        ++cursor;
        --length;
    }
}
