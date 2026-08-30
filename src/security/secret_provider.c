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

static UmiStatus umi_secret_reference_split(
    const char *reference,
    char *provider_id,
    size_t provider_capacity,
    const char **out_secret_name)
{
    const char *separator;
    const char *secret_name;
    size_t provider_length;
    if (reference == NULL || provider_id == NULL || provider_capacity == 0U ||
        out_secret_name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    separator = strchr(reference, ':');
    if (separator == NULL || separator == reference) return UMI_STATUS_PARSE_ERROR;
    secret_name = separator + 1;
    if (secret_name[0] == '/' && secret_name[1] == '/') secret_name += 2;
    if (secret_name[0] == '\0') return UMI_STATUS_PARSE_ERROR;
    provider_length = (size_t)(separator - reference);
    if (provider_length >= provider_capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(provider_id, reference, provider_length);
    provider_id[provider_length] = '\0';
    *out_secret_name = secret_name;
    return UMI_STATUS_OK;
}
UmiStatus umi_secret_provider_registry_create(UmiSecretProviderRegistry **out_registry)
{
    UmiSecretProviderRegistry *registry; UmiStatus status;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL; registry = (UmiSecretProviderRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&registry->mutex); if (status != UMI_STATUS_OK) { free(registry); return status; }
    *out_registry = registry; return UMI_STATUS_OK;
}
void umi_secret_provider_registry_destroy(UmiSecretProviderRegistry *registry)
{
    size_t i; if (registry == NULL) return;
    for (i = 0U; i < registry->count; ++i) umi_secret_provider_dispose(&registry->items[i].provider);
    umi_mutex_destroy(registry->mutex); free(registry);
}
UmiStatus umi_secret_provider_registry_add(UmiSecretProviderRegistry *registry, const char *provider_id, UmiSecretProvider *provider)
{
    size_t i;
    size_t provider_size;
    const size_t original_provider_size =
        offsetof(UmiSecretProvider, destroy) + sizeof(provider->destroy);
    UmiStatus status;
    if (registry == NULL || provider_id == NULL || provider_id[0] == '\0' ||
        provider == NULL || provider->structure_size < original_provider_size ||
        provider->get == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(provider_id) >= UMI_SECRET_PROVIDER_ID_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_mutex_lock(registry->mutex); if (status != UMI_STATUS_OK) return status;
    for (i = 0U; i < registry->count; ++i) if (strcmp(registry->items[i].provider_id, provider_id) == 0) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_ALREADY_EXISTS; }
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
UmiStatus umi_secret_provider_registry_resolve(const UmiSecretProviderRegistry *registry, const char *reference, char *out_value, size_t capacity)
{
    const char *secret_name; char provider_id[UMI_SECRET_PROVIDER_ID_CAPACITY]; size_t i; UmiStatus status;
    if (registry == NULL || reference == NULL || out_value == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_secret_reference_split(reference, provider_id, sizeof(provider_id), &secret_name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_mutex_lock(registry->mutex); if (status != UMI_STATUS_OK) return status;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].provider_id, provider_id) == 0) {
            status = umi_secret_get(&registry->items[i].provider, secret_name, out_value, capacity);
            (void)umi_mutex_unlock(registry->mutex); return status;
        }
    }
    (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_secret_provider_registry_store(
    UmiSecretProviderRegistry *registry,
    const char *reference,
    const char *value)
{
    const char *secret_name;
    char provider_id[UMI_SECRET_PROVIDER_ID_CAPACITY];
    size_t index;
    UmiStatus status;
    if (registry == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_secret_reference_split(
        reference, provider_id, sizeof(provider_id), &secret_name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < registry->count; ++index) {
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

UmiStatus umi_secret_provider_registry_remove(
    UmiSecretProviderRegistry *registry,
    const char *reference)
{
    const char *secret_name;
    char provider_id[UMI_SECRET_PROVIDER_ID_CAPACITY];
    size_t index;
    UmiStatus status;
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_secret_reference_split(
        reference, provider_id, sizeof(provider_id), &secret_name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < registry->count; ++index) {
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
size_t umi_secret_provider_registry_count(const UmiSecretProviderRegistry *registry)
{ size_t n = 0U; if (registry != NULL && umi_mutex_lock(registry->mutex) == UMI_STATUS_OK) { n = registry->count; (void)umi_mutex_unlock(registry->mutex); } return n; }
