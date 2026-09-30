/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/property_schema.h
 *
 * PURPOSE:
 *   Define component property schemas for inspection, validation and low-code authoring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_DESIGNER_PROPERTY_SCHEMA_H
#define UMICOM_DESIGNER_PROPERTY_SCHEMA_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/snapshot_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_PROPERTY_SCHEMA_CAPACITY 4096U

/**
 * Represent the designer property schema snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerPropertySchemaSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char component_type[128];
    char property_name[128];
    char value_type[64];
    char default_value[512];
    char category[128];
    int required;
    int bindable;
    int32_t order;
    uint64_t revision;
} UmiDesignerPropertySchemaSnapshot;

/**
 * Represent the designer property schema registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerPropertySchemaRegistry UmiDesignerPropertySchemaRegistry;

/**
 * Initialise designer property schema registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_designer_property_schema_registry_create(UmiDesignerPropertySchemaRegistry **out_registry);
/**
 * Release or reset state held by designer property schema registry so the same storage can
 * be reused safely.
 */
void umi_designer_property_schema_registry_destroy(UmiDesignerPropertySchemaRegistry *registry);
/**
 * Provide the designer property schema registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_designer_property_schema_registry_upsert(UmiDesignerPropertySchemaRegistry *registry, const UmiDesignerPropertySchemaSnapshot *item);
/**
 * Remove designer property schema registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_designer_property_schema_registry_remove(UmiDesignerPropertySchemaRegistry *registry, const char *id);
/**
 * Find designer property schema registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_property_schema_registry_find(const UmiDesignerPropertySchemaRegistry *registry, const char *id, UmiDesignerPropertySchemaSnapshot *out_item);
/**
 * Find designer property schema registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_property_schema_registry_at(const UmiDesignerPropertySchemaRegistry *registry, size_t index, UmiDesignerPropertySchemaSnapshot *out_item);
/**
 * Return the number of records represented by designer property schema registry without
 * changing their state.
 */
size_t umi_designer_property_schema_registry_count(const UmiDesignerPropertySchemaRegistry *registry);
/**
 * Provide the designer property schema registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_designer_property_schema_registry_revision(const UmiDesignerPropertySchemaRegistry *registry);


/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_designer_property_schema_snapshot_validate(const UmiDesignerPropertySchemaSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_DESIGNER_PROPERTY_SCHEMA_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_designer_property_schema_registry_upsert_many(UmiDesignerPropertySchemaRegistry *registry,
    const UmiDesignerPropertySchemaSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif

#endif
