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

/* BEGINNER NOTE:
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

typedef struct UmiDesignerPropertySchemaRegistry UmiDesignerPropertySchemaRegistry;

UmiStatus umi_designer_property_schema_registry_create(UmiDesignerPropertySchemaRegistry **out_registry);
void umi_designer_property_schema_registry_destroy(UmiDesignerPropertySchemaRegistry *registry);
UmiStatus umi_designer_property_schema_registry_upsert(UmiDesignerPropertySchemaRegistry *registry, const UmiDesignerPropertySchemaSnapshot *item);
UmiStatus umi_designer_property_schema_registry_remove(UmiDesignerPropertySchemaRegistry *registry, const char *id);
UmiStatus umi_designer_property_schema_registry_find(const UmiDesignerPropertySchemaRegistry *registry, const char *id, UmiDesignerPropertySchemaSnapshot *out_item);
UmiStatus umi_designer_property_schema_registry_at(const UmiDesignerPropertySchemaRegistry *registry, size_t index, UmiDesignerPropertySchemaSnapshot *out_item);
size_t umi_designer_property_schema_registry_count(const UmiDesignerPropertySchemaRegistry *registry);
uint64_t umi_designer_property_schema_registry_revision(const UmiDesignerPropertySchemaRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
