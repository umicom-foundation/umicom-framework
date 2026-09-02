/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/secret_provider.c
 *
 * PURPOSE:
 *   Implement ownership-safe registration and provider-qualified secret
 *   resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/secret_provider.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "umicom/platform/threading.h"
typedef struct UmiStoredSecretProvider { char provider_id[UMI_SECRET_PROVIDER_ID_CAPACITY]; UmiSecretProvider provider; } UmiStoredSecretProvider;
struct UmiSecretProviderRegistry { UmiStoredSecretProvider items[UMI_SECRET_PROVIDER_REGISTRY_MAX]; size_t count; UmiMutex *mutex; };

/*
 * Provide the secret reference split operation used by this module and its client
 * applications.
 */
static UmiStatus umi_secret_reference_split(
    const char *reference,
    char *provider_id,
    size_t provider_capacity,
    const char **out_secret_name)
{
    const char *separator;
    const char *secret_name;
    size_t provider_length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (reference == NULL || provider_id == NULL || provider_capacity == 0U ||
        out_secret_name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    separator = strchr(reference, ':');
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (separator == NULL || separator == reference) return UMI_STATUS_PARSE_ERROR;
    secret_name = separator + 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (secret_name[0] == '/' && secret_name[1] == '/') secret_name += 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (secret_name[0] == '\0') return UMI_STATUS_PARSE_ERROR;
    provider_length = (size_t)(separator - reference);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (provider_length >= provider_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(provider_id, reference, provider_length);
    provider_id[provider_length] = '\0';
    *out_secret_name = secret_name;
    return UMI_STATUS_OK;
}
/*
 * Initialise secret provider registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_secret_provider_registry_create(UmiSecretProviderRegistry **out_registry)
{
    UmiSecretProviderRegistry *registry; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL; registry = (UmiSecretProviderRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&registry->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { free(registry); return status; }
    *out_registry = registry; return UMI_STATUS_OK;
}
/*
 * Release or reset state held by secret provider registry so the same storage can be
 * reused safely.
 */
void umi_secret_provider_registry_destroy(UmiSecretProviderRegistry *registry)
{
    size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) umi_secret_provider_dispose(&registry->items[i].provider);
    umi_mutex_destroy(registry->mutex); free(registry);
}
/*
 * Add secret provider registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_secret_provider_registry_add(UmiSecretProviderRegistry *registry, const char *provider_id, UmiSecretProvider *provider)
{
    size_t i;
    size_t provider_size;
    const size_t original_provider_size =
        offsetof(UmiSecretProvider, destroy) + sizeof(provider->destroy);
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0' ||
        provider == NULL || provider->structure_size < original_provider_size ||
        provider->get == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(provider_id) >= UMI_SECRET_PROVIDER_ID_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_mutex_lock(registry->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[i].provider_id, provider_id) == 0) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_ALREADY_EXISTS; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_SECRET_PROVIDER_REGISTRY_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    (void)snprintf(registry->items[registry->count].provider_id, UMI_SECRET_PROVIDER_ID_CAPACITY, "%s", provider_id);
    provider_size = provider->structure_size < sizeof(*provider)
        ? (size_t)provider->structure_size
        : sizeof(*provider);
    (void)memset(&registry->items[registry->count].provider,
                 0,
                 sizeof(registry->items[registry->count].provider));
    (void)memcpy(&registry->items[registry->count].provider,
                 provider,
                 provider_size);
    (void)memset(provider, 0, provider_size);
    ++registry->count;
    (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
/*
 * Provide the secret provider registry resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_secret_provider_registry_resolve(const UmiSecretProviderRegistry *registry, const char *reference, char *out_value, size_t capacity)
{
    const char *secret_name; char provider_id[UMI_SECRET_PROVIDER_ID_CAPACITY]; size_t i; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || reference == NULL || out_value == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_secret_reference_split(reference, provider_id, sizeof(provider_id), &secret_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_mutex_lock(registry->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[i].provider_id, provider_id) == 0) {
            status = umi_secret_get(&registry->items[i].provider, secret_name, out_value, capacity);
            (void)umi_mutex_unlock(registry->mutex); return status;
        }
    }
    (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the secret provider registry store operation used by this module and its client
 * applications.
 */
UmiStatus umi_secret_provider_registry_store(
    UmiSecretProviderRegistry *registry,
    const char *reference,
    const char *value)
{
    const char *secret_name;
    char provider_id[UMI_SECRET_PROVIDER_ID_CAPACITY];
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_secret_reference_split(
        reference, provider_id, sizeof(provider_id), &secret_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_mutex_lock(registry->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].provider_id, provider_id) == 0) {
            status = umi_secret_set(
                &registry->items[index].provider, secret_name, value);
            (void)umi_mutex_unlock(registry->mutex);
            return status;
        }
    }
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Remove secret provider registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_secret_provider_registry_remove(
    UmiSecretProviderRegistry *registry,
    const char *reference)
{
    const char *secret_name;
    char provider_id[UMI_SECRET_PROVIDER_ID_CAPACITY];
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_secret_reference_split(
        reference, provider_id, sizeof(provider_id), &secret_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_mutex_lock(registry->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].provider_id, provider_id) == 0) {
            status = umi_secret_remove(
                &registry->items[index].provider, secret_name);
            (void)umi_mutex_unlock(registry->mutex);
            return status;
        }
    }
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_NOT_FOUND;
}
/*
 * Return the number of records represented by secret provider registry without changing
 * their state.
 */
size_t umi_secret_provider_registry_count(const UmiSecretProviderRegistry *registry)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL && umi_mutex_lock(registry->mutex) == UMI_STATUS_OK) { n = registry->count; (void)umi_mutex_unlock(registry->mutex); } return n; }
