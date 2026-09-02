/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/repository.c
 *
 * PURPOSE:
 *   Implement stable namespaced record keys and repository save, load and remove operations over any UmiStore provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/repository.h"

#include <stdio.h>
#include <string.h>

#include "include/data_internal.h"

/* Provide the record key operation used by this module and its client applications. */
static UmiStatus record_key(const UmiRepository *repository,
                            const char *record_id,
                            char *out_key,
                            size_t capacity)
{
    return umi_data_key(out_key, capacity, repository->prefix, record_id);
}

/*
 * Initialise repository from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_repository_init(UmiRepository *repository,
                              const UmiStore *store,
                              const char *namespace_name)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Write repository in its stable representation and report capacity or input failures to
 * the caller.
 */
UmiStatus umi_repository_save(UmiRepository *repository,
                              const char *record_id,
                              const char *serialised_value)
{
    char key[320];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || serialised_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = record_key(repository, record_id, key, sizeof(key));
    return status == UMI_STATUS_OK
        ? umi_store_set(&repository->store, key, serialised_value)
        : status;
}

/*
 * Read repository into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_repository_load(const UmiRepository *repository,
                              const char *record_id,
                              char *out_value,
                              size_t value_capacity)
{
    char key[320];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = record_key(repository, record_id, key, sizeof(key));
    return status == UMI_STATUS_OK
        ? umi_store_get(&repository->store, key, out_value, value_capacity)
        : status;
}

/* Remove repository while keeping the remaining records in a valid and discoverable state. */
UmiStatus umi_repository_remove(UmiRepository *repository,
                                const char *record_id)
{
    char key[320];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = record_key(repository, record_id, key, sizeof(key));
    return status == UMI_STATUS_OK
        ? umi_store_remove(&repository->store, key)
        : status;
}
