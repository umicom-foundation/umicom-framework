/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/inspector.h
 *
 * PURPOSE:
 *   Build property-inspector snapshots and typed property schemas for selected
 *   semantic components without exposing document internals to product views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A future Bank, Trader, Media or Studio designer can use the same property
 * schema and validation API. Only the visual presentation belongs to the app.
 */

#ifndef UMICOM_DESIGNER_INSPECTOR_H
#define UMICOM_DESIGNER_INSPECTOR_H
#include "umicom/designer/document.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the designer inspector snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerInspectorSnapshot {
    char node_id[UMI_DECL_ID_CAPACITY];
    char component_type[UMI_DECL_ID_CAPACITY];
    UmiDeclAttribute attributes[UMI_DECL_MAX_ATTRIBUTES];
    size_t attribute_count;
} UmiDesignerInspectorSnapshot;

/**
 * Represent the designer inspector schema data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerInspectorSchema {
    char component_type[UMI_DECL_ID_CAPACITY];
    UmiDeclPropertyDescriptor properties[UMI_DECL_MAX_PROPERTIES];
    size_t property_count;
} UmiDesignerInspectorSchema;

/**
 * Provide the designer inspect operation used by this module and its client applications.
 */
UmiStatus umi_designer_inspect(
    const UmiDesignerDocument *document,
    const char *node_id,
    UmiDesignerInspectorSnapshot *out_snapshot
);

/**
 * Provide the designer inspector schema operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_inspector_schema(
    const UmiDeclComponentRegistry *registry,
    const char *component_type,
    UmiDesignerInspectorSchema *out_schema
);

/**
 * Provide the designer inspector property operation used by this module and its client
 * applications.
 */
const UmiDeclPropertyDescriptor *umi_designer_inspector_property(
    const UmiDesignerInspectorSchema *schema,
    const char *property_name
);

/**
 * Provide the designer inspector validate property operation used by this module and its
 * client applications.
 */
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
