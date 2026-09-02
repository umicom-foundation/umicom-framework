/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/schema_catalogue.h
 *
 * PURPOSE:
 *   Describe the logical Data Server collections, durability, replication and retention policy used by layout persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the workbench layout data collection descriptor data shared with callers of
 * this public contract.
 */
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

/**
 * Represent the workbench layout data schema catalogue data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutDataSchemaCatalogue {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataCollectionDescriptor
        collections[UMI_WORKBENCH_LAYOUT_DATA_MAX_COLLECTIONS];
    size_t count;
    uint32_t current_version;
    uint64_t catalogue_hash;
} UmiWorkbenchLayoutDataSchemaCatalogue;

/**
 * Initialise workbench layout data schema catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_layout_data_schema_catalogue_init(
    UmiWorkbenchLayoutDataSchemaCatalogue *catalogue);

/**
 * Provide the workbench layout data schema catalogue seed operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_data_schema_catalogue_seed(
    UmiWorkbenchLayoutDataSchemaCatalogue *catalogue);

/**
 * Add workbench layout data schema catalogue only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_workbench_layout_data_schema_catalogue_add(
    UmiWorkbenchLayoutDataSchemaCatalogue *catalogue,
    const UmiWorkbenchLayoutDataCollectionDescriptor *descriptor);

/**
 * Find workbench layout data schema catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiWorkbenchLayoutDataCollectionDescriptor *
umi_workbench_layout_data_schema_catalogue_find(
    const UmiWorkbenchLayoutDataSchemaCatalogue *catalogue,
    UmiWorkbenchLayoutDataRecordKind kind);

/**
 * Check that workbench layout data schema catalogue satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_layout_data_schema_catalogue_validate(
    const UmiWorkbenchLayoutDataSchemaCatalogue *catalogue);

/**
 * Provide the workbench layout data schema catalogue hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_layout_data_schema_catalogue_hash(
    const UmiWorkbenchLayoutDataSchemaCatalogue *catalogue);

#ifdef __cplusplus
}
#endif

#endif
