/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/repository.c
 *
 * PURPOSE:
 *   Implement stable namespaced record keys and repository save, load and remove operations over any UmiStore provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/repository.h"

#include <stdio.h>
#include <string.h>

#include "include/data_internal.h"

static UmiStatus record_key(const UmiRepository *repository,
                            const char *record_id,
                            char *out_key,
                            size_t capacity)
{
    return umi_data_key(out_key, capacity, repository->prefix, record_id);
}

UmiStatus umi_repository_init(UmiRepository *repository,
                              const UmiStore *store,
                              const char *namespace_name)
{
    int written;
    if (repository == NULL || store == NULL || namespace_name == NULL ||
        namespace_name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    repository->store = *store;
    written = snprintf(repository->prefix,
                       sizeof(repository->prefix),
                       "repo.%s.",
                       namespace_name);
    return written < 0 || (size_t)written >= sizeof(repository->prefix)
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

UmiStatus umi_repository_save(UmiRepository *repository,
                              const char *record_id,
                              const char *serialised_value)
{
    char key[320];
    UmiStatus status;
    if (repository == NULL || serialised_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = record_key(repository, record_id, key, sizeof(key));
    return status == UMI_STATUS_OK
        ? umi_store_set(&repository->store, key, serialised_value)
        : status;
}

UmiStatus umi_repository_load(const UmiRepository *repository,
                              const char *record_id,
                              char *out_value,
                              size_t value_capacity)
{
    char key[320];
    UmiStatus status;
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = record_key(repository, record_id, key, sizeof(key));
    return status == UMI_STATUS_OK
        ? umi_store_get(&repository->store, key, out_value, value_capacity)
        : status;
}

UmiStatus umi_repository_remove(UmiRepository *repository,
                                const char *record_id)
{
    char key[320];
    UmiStatus status;
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = record_key(repository, record_id, key, sizeof(key));
    return status == UMI_STATUS_OK
        ? umi_store_remove(&repository->store, key)
        : status;
}
