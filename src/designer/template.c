/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/template.c
 *
 * PURPOSE:
 *   Implement owned visual templates and transaction-based instantiation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/template.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDesignerTemplateRegistry {
    UmiDesignerTemplate items[UMI_DESIGNER_TEMPLATE_CAPACITY];
    size_t count;
};

/*
 * Initialise designer template from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_designer_template_init(UmiDesignerTemplate *item,
                                         const char *template_id,
                                         const char *name,
                                         const char *category)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || template_id == NULL || name == NULL || category == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    status = umi_decl_copy_text(item->template_id, sizeof(item->template_id), template_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_copy_text(item->name, sizeof(item->name), name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_copy_text(item->category, sizeof(item->category), category);
    return status;
}

/*
 * Provide the designer template add node operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_template_add_node(UmiDesignerTemplate *item, const UmiDeclNode *node)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || node == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (item->node_count >= UMI_DESIGNER_TEMPLATE_NODE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    item->nodes[item->node_count++] = *node;
    return UMI_STATUS_OK;
}

/*
 * Initialise designer template registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_template_registry_create(UmiDesignerTemplateRegistry **out_registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = (UmiDesignerTemplateRegistry *)calloc(1U, sizeof(**out_registry));
    return *out_registry != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

/*
 * Release or reset state held by designer template registry so the same storage can be
 * reused safely.
 */
void umi_designer_template_registry_destroy(UmiDesignerTemplateRegistry *registry)
{
    free(registry);
}

/*
 * Add designer template registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_designer_template_registry_register(UmiDesignerTemplateRegistry *registry,
                                                      const UmiDesignerTemplate *item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || item == NULL || item->template_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].template_id, item->template_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_DESIGNER_TEMPLATE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *item;
    return UMI_STATUS_OK;
}

/*
 * Find designer template registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_designer_template_registry_find(const UmiDesignerTemplateRegistry *registry,
                                                  const char *template_id,
                                                  UmiDesignerTemplate *out_item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || template_id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].template_id, template_id) == 0) {
            *out_item = registry->items[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Return the number of records represented by designer template registry without changing
 * their state.
 */
size_t umi_designer_template_registry_count(const UmiDesignerTemplateRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/* Provide the prefixed operation used by this module and its client applications. */
static UmiStatus prefixed(char *out_text, size_t capacity, const char *prefix, const char *identifier)
{
    int count = snprintf(out_text, capacity, "%s_%s", prefix, identifier);
    return count >= 0 && (size_t)count < capacity ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Provide the designer template instantiate operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_template_instantiate(const UmiDesignerTemplate *item,
                                                UmiDesignerTransactionHistory *history,
                                                const char *target_parent_id,
                                                const char *id_prefix)
{
    UmiDesignerTransaction *transaction;
    UmiStatus status;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || history == NULL || target_parent_id == NULL || id_prefix == NULL || item->node_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    transaction = (UmiDesignerTransaction *)malloc(sizeof(*transaction));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_designer_transaction_init(transaction, "template-instantiate", item->name);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < item->node_count; ++index) {
        UmiDeclNode node = item->nodes[index];
        UmiDesignerOperation operation;
        size_t parent_index;
        char identifier[UMI_DECL_ID_CAPACITY];
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        status = prefixed(identifier, sizeof(identifier), id_prefix, node.node_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_decl_copy_text(node.node_id, sizeof(node.node_id), identifier);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        /* Visit each bounded item once so every record receives the same rule. */
        for (parent_index = 0U; parent_index < item->node_count; ++parent_index) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(node.parent_id, item->nodes[parent_index].node_id) == 0) {
                status = prefixed(identifier, sizeof(identifier), id_prefix, node.parent_id);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) status = umi_decl_copy_text(node.parent_id, sizeof(node.parent_id), identifier);
                break;
            }
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (parent_index == item->node_count) status = umi_decl_copy_text(node.parent_id, sizeof(node.parent_id), target_parent_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_designer_operation_add(&node, &operation);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_designer_transaction_add(transaction, &operation);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_designer_transaction_history_execute(history, transaction);
    }
    free(transaction);
    return status;
}
