/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/node.c
 *
 * PURPOSE:
 *   Implement semantic component nodes and bounded property storage for deterministic templates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/node.h"

#include <string.h>

UmiStatus umi_decl_node_init(UmiDeclNode *node, const char *node_id, const char *component_type, const char *parent_id)
{
    UmiStatus status;
    if (node == NULL || !umi_decl_id_is_valid(node_id) || !umi_decl_id_is_valid(component_type)) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(node, 0, sizeof(*node));
    node->kind = UMI_DECL_NODE_COMPONENT;
    status = umi_decl_copy_text(node->node_id, sizeof(node->node_id), node_id);
    if (status == UMI_STATUS_OK) status = umi_decl_copy_text(node->component_type, sizeof(node->component_type), component_type);
    if (status == UMI_STATUS_OK && parent_id != NULL && parent_id[0] != '\0' && strcmp(parent_id, "-") != 0) {
        status = umi_decl_copy_text(node->parent_id, sizeof(node->parent_id), parent_id);
    }
    return status;
}

UmiStatus umi_decl_node_set_attribute(UmiDeclNode *node, const char *name, UmiDeclValueKind kind, const char *value_text)
{
    size_t i;
    if (node == NULL || name == NULL || value_text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < node->attribute_count; ++i) {
        if (umi_decl_attribute_name_equal(&node->attributes[i], name)) {
            return umi_decl_attribute_init(&node->attributes[i], name, kind, value_text);
        }
    }
    if (node->attribute_count >= UMI_DECL_MAX_ATTRIBUTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_decl_attribute_init(&node->attributes[node->attribute_count], name, kind, value_text) != UMI_STATUS_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }
    node->attribute_count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_decl_node_get_attribute(const UmiDeclNode *node, const char *name, UmiDeclAttribute *out_attribute)
{
    size_t i;
    if (node == NULL || name == NULL || out_attribute == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < node->attribute_count; ++i) {
        if (umi_decl_attribute_name_equal(&node->attributes[i], name)) {
            *out_attribute = node->attributes[i];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_decl_node_remove_attribute(UmiDeclNode *node, const char *name)
{
    size_t i;
    if (node == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < node->attribute_count; ++i) {
        if (umi_decl_attribute_name_equal(&node->attributes[i], name)) {
            size_t remaining = node->attribute_count - i - 1U;
            if (remaining > 0U) (void)memmove(&node->attributes[i], &node->attributes[i + 1U], remaining * sizeof(node->attributes[0]));
            node->attribute_count -= 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
