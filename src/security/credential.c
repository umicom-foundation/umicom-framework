/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/credential.c
 *
 * PURPOSE:
 *   Implement a bounded registry of secret references, ownership and expiry
 *   metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/credential.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
struct UmiCredentialRegistry { UmiCredentialRecord items[UMI_CREDENTIAL_REGISTRY_MAX]; size_t count; UmiMutex *mutex; };
/* Find credential while leaving the underlying catalogue or model owned by this module. */
static size_t credential_find(const UmiCredentialRegistry *registry, const char *id)
{ size_t i; /* Visit each bounded item once so every record receives the same rule. */ for (i = 0U; i < registry->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[i].credential_id, id) == 0) return i; return SIZE_MAX; }
/*
 * Initialise credential registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_credential_registry_create(UmiCredentialRegistry **out_registry)
{
    UmiCredentialRegistry *registry; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL; registry = (UmiCredentialRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&registry->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { free(registry); return status; }
    *out_registry = registry; return UMI_STATUS_OK;
}
/*
 * Release or reset state held by credential registry so the same storage can be reused
 * safely.
 */
void umi_credential_registry_destroy(UmiCredentialRegistry *registry)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL) { umi_mutex_destroy(registry->mutex); free(registry); } }
/* Add credential registry only after its inputs and available capacity have been checked. */
UmiStatus umi_credential_registry_register(UmiCredentialRegistry *registry, const UmiCredentialRecord *record)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || record == NULL || record->credential_id[0] == '\0' || record->principal_id[0] == '\0' || record->provider_id[0] == '\0' || record->secret_reference[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (credential_find(registry, record->credential_id) != SIZE_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_ALREADY_EXISTS; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_CREDENTIAL_REGISTRY_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    registry->items[registry->count] = *record; registry->items[registry->count].active = record->active != 0; ++registry->count;
    (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
/*
 * Provide the credential registry get operation used by this module and its client
 * applications.
 */
UmiStatus umi_credential_registry_get(const UmiCredentialRegistry *registry, const char *credential_id, UmiCredentialRecord *out_record)
{
    size_t index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || credential_id == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    index = credential_find(registry, credential_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (index == SIZE_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND; }
    *out_record = registry->items[index]; (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
/*
 * Provide the credential registry revoke operation used by this module and its client
 * applications.
 */
UmiStatus umi_credential_registry_revoke(UmiCredentialRegistry *registry, const char *credential_id)
{
    size_t index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || credential_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    index = credential_find(registry, credential_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (index == SIZE_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND; }
    registry->items[index].active = 0; (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by credential registry without changing their
 * state.
 */
size_t umi_credential_registry_count(const UmiCredentialRegistry *registry)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL && umi_mutex_lock(registry->mutex) == UMI_STATUS_OK) { n = registry->count; (void)umi_mutex_unlock(registry->mutex); } return n; }
