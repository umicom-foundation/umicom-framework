/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/schema_catalogue.c
 *
 * PURPOSE:
 *   Implement the durable collection catalogue and deterministic schema identity used by migrations, health checks and backups.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/schema_catalogue.h"
#include "umicom/workbench_layout_data/key_codec.h"
#include <string.h>



void umi_workbench_layout_data_schema_catalogue_init(
    UmiWorkbenchLayoutDataSchemaCatalogue *catalogue)
{
    if (catalogue == NULL) return;
    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->structure_size = sizeof(*catalogue);
    catalogue->current_version =
        UMI_WORKBENCH_LAYOUT_DATA_SCHEMA_VERSION;
}

UmiStatus umi_workbench_layout_data_schema_catalogue_add(
    UmiWorkbenchLayoutDataSchemaCatalogue *catalogue,
    const UmiWorkbenchLayoutDataCollectionDescriptor *descriptor)
{
    size_t index;
    if (catalogue == NULL || descriptor == NULL ||
        catalogue->structure_size < sizeof(*catalogue) ||
        descriptor->structure_size < sizeof(*descriptor) ||
        descriptor->name[0] == '\0' ||
        descriptor->key_prefix[0] == '\0' ||
        strncmp(descriptor->key_prefix, "workbench-layout/",
                strlen("workbench-layout/")) != 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (catalogue->count >=
        UMI_WORKBENCH_LAYOUT_DATA_MAX_COLLECTIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        if (catalogue->collections[index].kind == descriptor->kind ||
            strcmp(catalogue->collections[index].name,
                   descriptor->name) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    catalogue->collections[catalogue->count++] = *descriptor;
    catalogue->catalogue_hash =
        umi_workbench_layout_data_schema_catalogue_hash(catalogue);
    return UMI_STATUS_OK;
}

static UmiStatus add_collection(
    UmiWorkbenchLayoutDataSchemaCatalogue *catalogue,
    UmiWorkbenchLayoutDataRecordKind kind,
    const char *name,
    bool authoritative,
    bool durable,
    bool replicated,
    bool retention_managed)
{
    UmiWorkbenchLayoutDataCollectionDescriptor descriptor;
    UmiStatus status;
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = sizeof(descriptor);
    descriptor.kind = kind;
    descriptor.schema_version =
        UMI_WORKBENCH_LAYOUT_DATA_SCHEMA_VERSION;
    descriptor.authoritative = authoritative;
    descriptor.durable = durable;
    descriptor.replicated = replicated;
    descriptor.retention_managed = retention_managed;
    status = umi_workbench_layout_data_copy_text(
        descriptor.name, sizeof(descriptor.name), name, false);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_key_prefix(
            kind, NULL, descriptor.key_prefix,
            sizeof(descriptor.key_prefix));
    }
    descriptor.schema_hash =
        umi_workbench_layout_data_hash_text(name) ^
        ((uint64_t)kind << 32U) ^
        (uint64_t)descriptor.schema_version;
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_layout_data_schema_catalogue_add(
        catalogue, &descriptor);
}

UmiStatus umi_workbench_layout_data_schema_catalogue_seed(
    UmiWorkbenchLayoutDataSchemaCatalogue *catalogue)
{
    UmiStatus status;
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_layout_data_schema_catalogue_init(catalogue);
#define ADD(kind, name, authority, durable, replicated, retention) \
    do { \
        status = add_collection(catalogue, kind, name, authority, \
                                durable, replicated, retention); \
        if (status != UMI_STATUS_OK) return status; \
    } while (0)
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_MANIFEST,
        "layout-manifests", true, true, true, false);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_LAYOUT_CHUNK,
        "layout-chunks", true, true, true, false);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST,
        "session-manifests", true, true, false, true);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK,
        "session-chunks", true, true, false, true);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_REVISION,
        "layout-revisions", true, true, true, true);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_GRANT,
        "layout-grants", true, true, true, true);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_LEASE,
        "layout-leases", true, true, false, true);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_CHANGE,
        "layout-change-feed", true, true, true, true);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_OUTBOX,
        "layout-outbox", true, true, false, true);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_CONFLICT,
        "layout-conflicts", true, true, true, true);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_SYNC_CURSOR,
        "layout-sync-cursors", true, true, false, false);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_OFFLINE_OPERATION,
        "layout-offline-operations", true, true, false, true);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_PRESENCE,
        "layout-presence", false, false, false, true);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_MIGRATION,
        "layout-migrations", true, true, false, false);
    ADD(UMI_WORKBENCH_LAYOUT_DATA_RECORD_METADATA,
        "layout-metadata", true, true, true, false);
#undef ADD
    return umi_workbench_layout_data_schema_catalogue_validate(catalogue);
}

const UmiWorkbenchLayoutDataCollectionDescriptor *
umi_workbench_layout_data_schema_catalogue_find(
    const UmiWorkbenchLayoutDataSchemaCatalogue *catalogue,
    UmiWorkbenchLayoutDataRecordKind kind)
{
    size_t index;
    if (catalogue == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index) {
        if (catalogue->collections[index].kind == kind) {
            return &catalogue->collections[index];
        }
    }
    return NULL;
}

UmiStatus umi_workbench_layout_data_schema_catalogue_validate(
    const UmiWorkbenchLayoutDataSchemaCatalogue *catalogue)
{
    size_t index;
    size_t nested;
    if (catalogue == NULL ||
        catalogue->structure_size < sizeof(*catalogue) ||
        catalogue->current_version == 0U ||
        catalogue->count == 0U ||
        catalogue->count > UMI_WORKBENCH_LAYOUT_DATA_MAX_COLLECTIONS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiWorkbenchLayoutDataCollectionDescriptor *entry =
            &catalogue->collections[index];
        if (entry->structure_size < sizeof(*entry) ||
            entry->name[0] == '\0' ||
            entry->key_prefix[0] == '\0' ||
            strncmp(entry->key_prefix, "workbench-layout/",
                    strlen("workbench-layout/")) != 0 ||
            entry->schema_version == 0U ||
            entry->schema_hash == 0U) {
            return UMI_STATUS_INVALID_STATE;
        }
        for (nested = index + 1U;
             nested < catalogue->count;
             ++nested) {
            if (entry->kind == catalogue->collections[nested].kind ||
                strcmp(entry->name,
                       catalogue->collections[nested].name) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    return UMI_STATUS_OK;
}

uint64_t umi_workbench_layout_data_schema_catalogue_hash(
    const UmiWorkbenchLayoutDataSchemaCatalogue *catalogue)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    size_t index;
    if (catalogue == NULL) return 0U;
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiWorkbenchLayoutDataCollectionDescriptor *entry =
            &catalogue->collections[index];
        hash ^= umi_workbench_layout_data_hash_text(entry->name);
        hash *= UINT64_C(1099511628211);
        hash ^= entry->schema_hash;
        hash *= UINT64_C(1099511628211);
    }
    hash ^= (uint64_t)catalogue->current_version;
    return hash;
}
