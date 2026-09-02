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

#ifdef __cplusplus
}
#endif

#endif
