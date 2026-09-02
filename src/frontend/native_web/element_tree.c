/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/element_tree.c
 *
 * PURPOSE:
 *   Own a bounded server-side browser element tree with parent validation and deterministic order.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/element_tree.h"

#include <string.h>

/*
 * Initialise native web element tree from caller-provided values so later operations
 * receive a known state.
 */
void umi_native_web_element_tree_init(UmiNativeWebElementTree *tree) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (tree != NULL) (void)memset(tree, 0, sizeof(*tree)); }

/*
 * Find native web element tree while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiNativeWebSemanticElement *umi_native_web_element_tree_find(const UmiNativeWebElementTree *tree, const char *element_id)
{
    size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (tree == NULL || element_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<tree->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(tree->elements[i].element_id, element_id)==0) return &tree->elements[i];
    return NULL;
}

/*
 * Provide the native web element tree upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_element_tree_upsert(UmiNativeWebElementTree *tree, const UmiNativeWebSemanticElement *element)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tree == NULL || umi_native_web_semantic_element_validate(element) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (element->parent_id[0] != '\0' && umi_native_web_element_tree_find(tree, element->parent_id) == NULL) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<tree->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(tree->elements[i].element_id, element->element_id)==0) { tree->elements[i]=*element; ++tree->revision; return UMI_STATUS_OK; }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (tree->count >= UMI_NATIVE_WEB_MAX_ELEMENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    tree->elements[tree->count++]=*element; ++tree->revision; return UMI_STATUS_OK;
}

/*
 * Remove native web element tree while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_native_web_element_tree_remove(UmiNativeWebElementTree *tree, const char *element_id)
{
    size_t i, target=UMI_NATIVE_WEB_MAX_ELEMENTS;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tree == NULL || element_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<tree->count;++i) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (strcmp(tree->elements[i].parent_id, element_id)==0) return UMI_STATUS_BUSY;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(tree->elements[i].element_id, element_id)==0) target=i;
    }
    /* Configure the optional target only when its feature has created it. */
    if (target==UMI_NATIVE_WEB_MAX_ELEMENTS) return UMI_STATUS_NOT_FOUND;
    /* Configure the optional target only when its feature has created it. */
    if (target+1U<tree->count) (void)memmove(&tree->elements[target], &tree->elements[target+1U], (tree->count-target-1U)*sizeof(tree->elements[0]));
    --tree->count; ++tree->revision; return UMI_STATUS_OK;
}

