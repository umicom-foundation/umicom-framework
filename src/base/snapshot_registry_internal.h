/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/snapshot_registry_internal.h
 * PURPOSE: Share atomic batch publication for value-only snapshot registries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SNAPSHOT_REGISTRY_INTERNAL_H
#define UMICOM_SNAPSHOT_REGISTRY_INTERNAL_H
#include "umicom/base/snapshot_validation.h"
#include <stdlib.h>
#include <string.h>

/* Only use this descriptor for a char array, not a pointer. Its complete
 * capacity is checked before the validator searches for a terminator. */
#define UMI_SNAPSHOT_TEXT_FIELD(Type, Member, Required) \
    {#Member, offsetof(Type, Member), sizeof(((Type *)0)->Member), Required}

/* The registry must own only its value array, count and revision; records must
 * contain no owned pointers, locks, callbacks or external handles. Upsert must
 * modify that registry alone. Each use is reviewed against those constraints.
 * A temporary full registry preserves exact existing upsert/normalisation
 * behaviour while making publication all-or-nothing. No callback observes it.
 * This is owner-thread atomicity, not cross-thread or filesystem atomicity. */
#define UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(Function, Registry, Snapshot, Validate, Upsert, Capacity) \
UmiStatus Function(Registry *registry, const Snapshot *items, size_t count, \
    UmiSnapshotBatchResult *outResult) \
{ \
    UmiSnapshotBatchResult result = {0U, SIZE_MAX, {UMI_SNAPSHOT_VALID, NULL, SIZE_MAX, 0U}}; \
    if (outResult != NULL) *outResult = result; \
    if (registry == NULL || (items == NULL && count != 0U)) return UMI_STATUS_INVALID_ARGUMENT; \
    if (count > (size_t)(Capacity)) return UMI_STATUS_CAPACITY_EXCEEDED; \
    if (count == 0U) return UMI_STATUS_OK; \
    for (size_t index = 0U; index < count; ++index) { \
        UmiStatus status = Validate(&items[index], &result.validation); \
        if (status != UMI_STATUS_OK) { \
            result.rejected_index = index; \
            if (outResult != NULL) *outResult = result; \
            return status; \
        } \
        for (size_t previous = 0U; previous < index; ++previous) { \
            if (strcmp(items[previous].id, items[index].id) == 0) { \
                result.rejected_index = index; \
                result.validation = (UmiSnapshotValidation){UMI_SNAPSHOT_DUPLICATE_ID, "id", 0U, sizeof(items[index].id)}; \
                if (outResult != NULL) *outResult = result; \
                return UMI_STATUS_ALREADY_EXISTS; \
            } \
        } \
    } \
    if ((uint64_t)count > UINT64_MAX - registry->revision) return UMI_STATUS_CAPACITY_EXCEEDED; \
    Registry *staged = malloc(sizeof(*staged)); \
    if (staged == NULL) return UMI_STATUS_OUT_OF_MEMORY; \
    *staged = *registry; \
    for (size_t index = 0U; index < count; ++index) { \
        UmiStatus status = Upsert(staged, &items[index]); \
        if (status != UMI_STATUS_OK) { \
            result.rejected_index = index; \
            free(staged); \
            if (outResult != NULL) *outResult = result; \
            return status; \
        } \
    } \
    *registry = *staged; \
    free(staged); \
    result.applied = count; \
    if (outResult != NULL) *outResult = result; \
    return UMI_STATUS_OK; \
}

/* Replace one document's records on a private copy, leaving other documents
 * intact. This reuses the canonical upsert and its value-only ownership rules.
 * Scope membership and collisions are checked before any registry is changed.
 * Inputs/outResult must not alias each other; callers serialize owner access. */
#define UMI_DEFINE_SNAPSHOT_DOCUMENT_REPLACE(Function, Registry, Snapshot, Validate, Upsert, Capacity) \
UmiStatus Function(Registry *registry, const char *document_id, uint64_t expected_revision, \
    const Snapshot *items, size_t count, UmiSnapshotBatchResult *outResult) \
{ \
    UmiSnapshotBatchResult result = {0U, SIZE_MAX, {UMI_SNAPSHOT_VALID, NULL, SIZE_MAX, 0U}}; \
    char scope[sizeof(((Snapshot *)0)->document_id)]; \
    size_t length = 0U, removed = 0U; \
    if (outResult != NULL) *outResult = result; \
    if (registry == NULL || document_id == NULL || (items == NULL && count != 0U)) \
        return UMI_STATUS_INVALID_ARGUMENT; \
    while (length < sizeof(scope) && document_id[length] != '\0') ++length; \
    if (length == sizeof(scope)) return UMI_STATUS_CAPACITY_EXCEEDED; \
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT; \
    memcpy(scope, document_id, length + 1U); \
    if (registry->count > (size_t)(Capacity)) return UMI_STATUS_INVALID_STATE; \
    if (registry->revision != expected_revision) return UMI_STATUS_INVALID_STATE; \
    if (count > (size_t)(Capacity)) return UMI_STATUS_CAPACITY_EXCEEDED; \
    for (size_t index = 0U; index < count; ++index) { \
        UmiStatus status = Validate(&items[index], &result.validation); \
        if (status == UMI_STATUS_OK && strcmp(items[index].document_id, scope) != 0) { \
            status = UMI_STATUS_PERMISSION_DENIED; \
            result.validation = (UmiSnapshotValidation){UMI_SNAPSHOT_SCOPE_MISMATCH, "document_id", SIZE_MAX, sizeof(scope)}; \
        } \
        if (status != UMI_STATUS_OK) { \
            result.rejected_index = index; \
            if (outResult != NULL) *outResult = result; \
            return status; \
        } \
        for (size_t previous = 0U; previous < index; ++previous) { \
            if (strcmp(items[index].id, items[previous].id) == 0) { \
                result.rejected_index = index; \
                result.validation = (UmiSnapshotValidation){UMI_SNAPSHOT_DUPLICATE_ID, "id", 0U, sizeof(items[index].id)}; \
                if (outResult != NULL) *outResult = result; \
                return UMI_STATUS_ALREADY_EXISTS; \
            } \
        } \
        for (size_t stored = 0U; stored < registry->count; ++stored) { \
            if (strcmp(items[index].id, registry->items[stored].id) == 0 && \
                strcmp(registry->items[stored].document_id, scope) != 0) { \
                result.rejected_index = index; \
                result.validation = (UmiSnapshotValidation){UMI_SNAPSHOT_SCOPE_MISMATCH, "id", 0U, sizeof(items[index].id)}; \
                if (outResult != NULL) *outResult = result; \
                return UMI_STATUS_PERMISSION_DENIED; \
            } \
        } \
    } \
    for (size_t index = 0U; index < registry->count; ++index) \
        if (strcmp(registry->items[index].document_id, scope) == 0) ++removed; \
    if (count > (size_t)(Capacity) - (registry->count - removed)) return UMI_STATUS_CAPACITY_EXCEEDED; \
    if (removed == 0U && count == 0U) return UMI_STATUS_OK; \
    if ((uint64_t)removed > UINT64_MAX - registry->revision || \
        (uint64_t)count > UINT64_MAX - registry->revision - (uint64_t)removed) \
        return UMI_STATUS_CAPACITY_EXCEEDED; \
    Registry *staged = malloc(sizeof(*staged)); \
    if (staged == NULL) return UMI_STATUS_OUT_OF_MEMORY; \
    *staged = *registry; \
    size_t retained = 0U; \
    for (size_t index = 0U; index < registry->count; ++index) { \
        if (strcmp(registry->items[index].document_id, scope) != 0) { \
            memmove(&staged->items[retained], &registry->items[index], sizeof(staged->items[0])); \
            ++retained; \
        } \
    } \
    memset(&staged->items[retained], 0, (registry->count - retained) * sizeof(staged->items[0])); \
    staged->count = retained; \
    staged->revision += (uint64_t)removed; \
    for (size_t index = 0U; index < count; ++index) { \
        UmiStatus status = Upsert(staged, &items[index]); \
        if (status != UMI_STATUS_OK) { \
            result.rejected_index = index; \
            free(staged); \
            if (outResult != NULL) *outResult = result; \
            return status; \
        } \
    } \
    *registry = *staged; \
    free(staged); \
    result.applied = count; \
    if (outResult != NULL) *outResult = result; \
    return UMI_STATUS_OK; \
}
#endif
