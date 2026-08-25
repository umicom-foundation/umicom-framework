/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/element_tree.c
 *
 * PURPOSE:
 *   Own a bounded server-side browser element tree with parent validation and deterministic order.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/element_tree.h"

#include <string.h>

void umi_native_web_element_tree_init(UmiNativeWebElementTree *tree) { if (tree != NULL) (void)memset(tree, 0, sizeof(*tree)); }

const UmiNativeWebSemanticElement *umi_native_web_element_tree_find(const UmiNativeWebElementTree *tree, const char *element_id)
{
    size_t i; if (tree == NULL || element_id == NULL) return NULL;
    for (i=0U;i<tree->count;++i) if (strcmp(tree->elements[i].element_id, element_id)==0) return &tree->elements[i];
    return NULL;
}

UmiStatus umi_native_web_element_tree_upsert(UmiNativeWebElementTree *tree, const UmiNativeWebSemanticElement *element)
{
    size_t i;
    if (tree == NULL || umi_native_web_semantic_element_validate(element) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    if (element->parent_id[0] != '\0' && umi_native_web_element_tree_find(tree, element->parent_id) == NULL) return UMI_STATUS_NOT_FOUND;
    for (i=0U;i<tree->count;++i) if (strcmp(tree->elements[i].element_id, element->element_id)==0) { tree->elements[i]=*element; ++tree->revision; return UMI_STATUS_OK; }
    if (tree->count >= UMI_NATIVE_WEB_MAX_ELEMENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    tree->elements[tree->count++]=*element; ++tree->revision; return UMI_STATUS_OK;
}

UmiStatus umi_native_web_element_tree_remove(UmiNativeWebElementTree *tree, const char *element_id)
{
    size_t i, target=UMI_NATIVE_WEB_MAX_ELEMENTS;
    if (tree == NULL || element_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<tree->count;++i) {
        if (strcmp(tree->elements[i].parent_id, element_id)==0) return UMI_STATUS_BUSY;
        if (strcmp(tree->elements[i].element_id, element_id)==0) target=i;
    }
    if (target==UMI_NATIVE_WEB_MAX_ELEMENTS) return UMI_STATUS_NOT_FOUND;
    if (target+1U<tree->count) (void)memmove(&tree->elements[target], &tree->elements[target+1U], (tree->count-target-1U)*sizeof(tree->elements[0]));
    --tree->count; ++tree->revision; return UMI_STATUS_OK;
}

