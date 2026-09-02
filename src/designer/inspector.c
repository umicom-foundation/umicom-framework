/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/inspector.c
 *
 * PURPOSE:
 *   Expose selected component values and reusable typed property metadata to
 *   any frontend without giving the view direct access to document internals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Property types and validation come from the declarative component registry.
 * Applications should not maintain a second hard-coded property catalogue.
 */

#include "umicom/designer/inspector.h"

#include <string.h>

/* Provide the designer inspect operation used by this module and its client applications. */
UmiStatus umi_designer_inspect(
    const UmiDesignerDocument *document,
    const char *id,
    UmiDesignerInspectorSnapshot *out_snapshot)
{
    UmiDeclNode node;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || id == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_decl_document_find_node(
        umi_designer_document_declarative((UmiDesignerDocument *)document),
        id,
        &node
    );
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)umi_decl_copy_text(out_snapshot->node_id,
                             sizeof(out_snapshot->node_id),
                             node.node_id);
    (void)umi_decl_copy_text(out_snapshot->component_type,
                             sizeof(out_snapshot->component_type),
                             node.component_type);
    out_snapshot->attribute_count = node.attribute_count;

    /* Apply this branch only when its contract condition is satisfied. */
    if (node.attribute_count > 0U) {
        (void)memcpy(out_snapshot->attributes,
                     node.attributes,
                     node.attribute_count * sizeof(node.attributes[0]));
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the designer inspector schema operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_inspector_schema(
    const UmiDeclComponentRegistry *registry,
    const char *component_type,
    UmiDesignerInspectorSchema *out_schema)
{
    UmiDeclComponentDescriptor descriptor;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || component_type == NULL || out_schema == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_decl_component_registry_find(registry,
                                              component_type,
                                              &descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)memset(out_schema, 0, sizeof(*out_schema));
    (void)umi_decl_copy_text(out_schema->component_type,
                             sizeof(out_schema->component_type),
                             descriptor.component_type);
    out_schema->property_count = descriptor.property_count;

    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor.property_count > 0U) {
        (void)memcpy(out_schema->properties,
                     descriptor.properties,
                     descriptor.property_count *
                         sizeof(descriptor.properties[0]));
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the designer inspector property operation used by this module and its client
 * applications.
 */
const UmiDeclPropertyDescriptor *umi_designer_inspector_property(
    const UmiDesignerInspectorSchema *schema,
    const char *property_name)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (schema == NULL || property_name == NULL) {
        return NULL;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < schema->property_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(schema->properties[index].name, property_name) == 0) {
            return &schema->properties[index];
        }
    }

    return NULL;
}

/*
 * Provide the designer inspector validate property operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_inspector_validate_property(
    const UmiDeclComponentRegistry *registry,
    const char *component_type,
    const char *property_name,
    const char *value_text)
{
    UmiDesignerInspectorSchema schema;
    const UmiDeclPropertyDescriptor *property;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || component_type == NULL ||
        property_name == NULL || value_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_designer_inspector_schema(registry,
                                           component_type,
                                           &schema);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    property = umi_designer_inspector_property(&schema, property_name);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    return umi_decl_property_validate_text(property, value_text);
}
