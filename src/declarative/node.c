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

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/node.h"

#include <string.h>

/* ENGINEERING NOTE: The previous node operations could read past a damaged
 * attribute count, and constructing an invalid replacement in the live slot
 * changed a property even when the edit failed. They are retained below for
 * review. The active operations validate bounded records and prepare edits in
 * separate storage before committing them. Public names and layouts are kept. */
#if 0
/*
 * Initialise decl node from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_decl_node_init(UmiDeclNode *node, const char *node_id, const char *component_type, const char *parent_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL || !umi_decl_id_is_valid(node_id) || !umi_decl_id_is_valid(component_type)) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(node, 0, sizeof(*node));
    node->kind = UMI_DECL_NODE_COMPONENT;
    status = umi_decl_copy_text(node->node_id, sizeof(node->node_id), node_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_copy_text(node->component_type, sizeof(node->component_type), component_type);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && parent_id != NULL && parent_id[0] != '\0' && strcmp(parent_id, "-") != 0) {
        status = umi_decl_copy_text(node->parent_id, sizeof(node->parent_id), parent_id);
    }
    return status;
}

/*
 * Provide the decl node set attribute operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_node_set_attribute(UmiDeclNode *node, const char *name, UmiDeclValueKind kind, const char *value_text)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL || name == NULL || value_text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < node->attribute_count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_decl_attribute_name_equal(&node->attributes[i], name)) {
            return umi_decl_attribute_init(&node->attributes[i], name, kind, value_text);
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (node->attribute_count >= UMI_DECL_MAX_ATTRIBUTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_decl_attribute_init(&node->attributes[node->attribute_count], name, kind, value_text) != UMI_STATUS_OK) {
        return UMI_STATUS_PARSE_ERROR;
    }
    node->attribute_count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the decl node get attribute operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_node_get_attribute(const UmiDeclNode *node, const char *name, UmiDeclAttribute *out_attribute)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL || name == NULL || out_attribute == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < node->attribute_count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_decl_attribute_name_equal(&node->attributes[i], name)) {
            *out_attribute = node->attributes[i];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the decl node remove attribute operation used by this module and its client
 * applications.
 */
UmiStatus umi_decl_node_remove_attribute(UmiDeclNode *node, const char *name)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < node->attribute_count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_decl_attribute_name_equal(&node->attributes[i], name)) {
            size_t remaining = node->attribute_count - i - 1U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (remaining > 0U) (void)memmove(&node->attributes[i], &node->attributes[i + 1U], remaining * sizeof(node->attributes[0]));
            node->attribute_count -= 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
#endif

/* Framework owns these checks so the designer, parser and native exporter do
 * not disagree about a safe public node. Parent existence and cycles belong
 * to a complete-document check, not incremental node construction. */
UmiStatus UmiDeclNodeValidate(const UmiDeclNode *node)
{
    if (node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (node->attribute_count > UMI_DECL_MAX_ATTRIBUTES ||
        memchr(node->node_id, '\0', sizeof node->node_id) == NULL ||
        memchr(node->component_type, '\0', sizeof node->component_type) == NULL ||
        memchr(node->parent_id, '\0', sizeof node->parent_id) == NULL)
        return UMI_STATUS_INVALID_STATE;
    if (!umi_decl_id_is_valid(node->node_id) ||
        !umi_decl_id_is_valid(node->component_type) ||
        (node->parent_id[0] != '\0' && !umi_decl_id_is_valid(node->parent_id)) ||
        (node->kind != UMI_DECL_NODE_COMPONENT && node->kind != UMI_DECL_NODE_RESOURCE))
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < node->attribute_count; ++i) {
        const UmiDeclAttribute *a = &node->attributes[i];
        if (memchr(a->name, '\0', sizeof a->name) == NULL || a->name[0] == '\0' ||
            memchr(a->value.text, '\0', sizeof a->value.text) == NULL ||
            a->value.kind < UMI_DECL_VALUE_STRING || a->value.kind > UMI_DECL_VALUE_REAL ||
            (a->value.kind == UMI_DECL_VALUE_BOOLEAN &&
             a->value.boolean_value != 0 && a->value.boolean_value != 1))
            return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(a->name, node->attributes[j].name) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_decl_node_init(UmiDeclNode *node, const char *node_id,
    const char *component_type, const char *parent_id)
{
    UmiDeclNode candidate = {0};
    if (node == NULL || !umi_decl_id_is_valid(node_id) ||
        !umi_decl_id_is_valid(component_type)) return UMI_STATUS_INVALID_ARGUMENT;
    if (parent_id != NULL && parent_id[0] != '\0' && strcmp(parent_id, "-") != 0 &&
        !umi_decl_id_is_valid(parent_id)) return UMI_STATUS_INVALID_ARGUMENT;
    candidate.kind = UMI_DECL_NODE_COMPONENT;
    (void)umi_decl_copy_text(candidate.node_id, sizeof candidate.node_id, node_id);
    (void)umi_decl_copy_text(candidate.component_type, sizeof candidate.component_type, component_type);
    if (parent_id != NULL && strcmp(parent_id, "-") != 0)
        (void)umi_decl_copy_text(candidate.parent_id, sizeof candidate.parent_id, parent_id);
    *node = candidate;
    return UMI_STATUS_OK;
}

UmiStatus umi_decl_node_set_attribute(UmiDeclNode *node, const char *name,
    UmiDeclValueKind kind, const char *value_text)
{
    UmiDeclAttribute candidate;
    size_t index;
    UmiStatus status;
    if (node == NULL || name == NULL || name[0] == '\0' || value_text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiDeclNodeValidate(node);
    if (status != UMI_STATUS_OK) return status;
    index = node->attribute_count;
    for (size_t i = 0U; i < node->attribute_count; ++i)
        if (strcmp(node->attributes[i].name, name) == 0) { index = i; break; }
    if (index == UMI_DECL_MAX_ATTRIBUTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Inputs may point into the old property; construct before changing it. */
    status = umi_decl_attribute_init(&candidate, name, kind, value_text);
    if (status != UMI_STATUS_OK) return status;
    node->attributes[index] = candidate;
    if (index == node->attribute_count) ++node->attribute_count;
    return UMI_STATUS_OK;
}

UmiStatus umi_decl_node_get_attribute(const UmiDeclNode *node, const char *name,
    UmiDeclAttribute *out_attribute)
{
    UmiStatus status;
    if (node == NULL || name == NULL || out_attribute == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiDeclNodeValidate(node);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < node->attribute_count; ++i) {
        if (strcmp(node->attributes[i].name, name) == 0) {
            *out_attribute = node->attributes[i];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_decl_node_remove_attribute(UmiDeclNode *node, const char *name)
{
    UmiStatus status;
    if (node == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiDeclNodeValidate(node);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < node->attribute_count; ++i) {
        if (strcmp(node->attributes[i].name, name) == 0) {
            size_t remaining = node->attribute_count - i - 1U;
            if (remaining != 0U)
                memmove(&node->attributes[i], &node->attributes[i + 1U],
                    remaining * sizeof node->attributes[0]);
            --node->attribute_count;
            memset(&node->attributes[node->attribute_count], 0, sizeof node->attributes[0]);
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
