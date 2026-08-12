/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/inspector.h
 *
 * PURPOSE:
 *   Build property-inspector snapshots and typed property schemas for selected
 *   semantic components without exposing document internals to product views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A future Bank, Trader, Media or Studio designer can use the same property
 * schema and validation API. Only the visual presentation belongs to the app.
 */

#ifndef UMICOM_DESIGNER_INSPECTOR_H
#define UMICOM_DESIGNER_INSPECTOR_H
#include "umicom/designer/document.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerInspectorSnapshot {
    char node_id[UMI_DECL_ID_CAPACITY];
    char component_type[UMI_DECL_ID_CAPACITY];
    UmiDeclAttribute attributes[UMI_DECL_MAX_ATTRIBUTES];
    size_t attribute_count;
} UmiDesignerInspectorSnapshot;

typedef struct UmiDesignerInspectorSchema {
    char component_type[UMI_DECL_ID_CAPACITY];
    UmiDeclPropertyDescriptor properties[UMI_DECL_MAX_PROPERTIES];
    size_t property_count;
} UmiDesignerInspectorSchema;

UmiStatus umi_designer_inspect(
    const UmiDesignerDocument *document,
    const char *node_id,
    UmiDesignerInspectorSnapshot *out_snapshot
);

UmiStatus umi_designer_inspector_schema(
    const UmiDeclComponentRegistry *registry,
    const char *component_type,
    UmiDesignerInspectorSchema *out_schema
);

const UmiDeclPropertyDescriptor *umi_designer_inspector_property(
    const UmiDesignerInspectorSchema *schema,
    const char *property_name
);

UmiStatus umi_designer_inspector_validate_property(
    const UmiDeclComponentRegistry *registry,
    const char *component_type,
    const char *property_name,
    const char *value_text
);

#ifdef __cplusplus
}
#endif
#endif
