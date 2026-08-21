/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/schema_catalogue.h
 *
 * PURPOSE:
 *   Describe the logical Data Server collections, durability, replication and retention policy used by layout persistence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_SCHEMA_CATALOGUE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_SCHEMA_CATALOGUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/workbench_layout_data/types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_LAYOUT_DATA_MAX_COLLECTIONS 32U

typedef struct UmiWorkbenchLayoutDataCollectionDescriptor {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordKind kind;
    char name[UMI_WORKBENCH_LAYOUT_DATA_TEXT_CAPACITY];
    char key_prefix[UMI_WORKBENCH_LAYOUT_DATA_KEY_CAPACITY];
    uint32_t schema_version;
    uint64_t schema_hash;
    bool authoritative;
    bool durable;
    bool replicated;
    bool retention_managed;
} UmiWorkbenchLayoutDataCollectionDescriptor;

typedef struct UmiWorkbenchLayoutDataSchemaCatalogue {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataCollectionDescriptor
        collections[UMI_WORKBENCH_LAYOUT_DATA_MAX_COLLECTIONS];
    size_t count;
    uint32_t current_version;
    uint64_t catalogue_hash;
} UmiWorkbenchLayoutDataSchemaCatalogue;

void umi_workbench_layout_data_schema_catalogue_init(
    UmiWorkbenchLayoutDataSchemaCatalogue *catalogue);

UmiStatus umi_workbench_layout_data_schema_catalogue_seed(
    UmiWorkbenchLayoutDataSchemaCatalogue *catalogue);

UmiStatus umi_workbench_layout_data_schema_catalogue_add(
    UmiWorkbenchLayoutDataSchemaCatalogue *catalogue,
    const UmiWorkbenchLayoutDataCollectionDescriptor *descriptor);

const UmiWorkbenchLayoutDataCollectionDescriptor *
umi_workbench_layout_data_schema_catalogue_find(
    const UmiWorkbenchLayoutDataSchemaCatalogue *catalogue,
    UmiWorkbenchLayoutDataRecordKind kind);

UmiStatus umi_workbench_layout_data_schema_catalogue_validate(
    const UmiWorkbenchLayoutDataSchemaCatalogue *catalogue);

uint64_t umi_workbench_layout_data_schema_catalogue_hash(
    const UmiWorkbenchLayoutDataSchemaCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
