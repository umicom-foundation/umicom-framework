/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/tree_model.h
 *
 * PURPOSE:
 *   Define a toolkit-neutral hierarchical tree model for Explorer-style views.
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
#ifndef UMICOM_UI_TREE_MODEL_H
#define UMICOM_UI_TREE_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_TREE_MODEL_CAPACITY 2048U

typedef struct UmiUiTreeModelSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char parent_id[128];
    char label[256];
    char description[512];
    char icon_name[128];
    int expanded;
    int selectable;
    int32_t order;
    uint64_t revision;
} UmiUiTreeModelSnapshot;

typedef struct UmiUiTreeModelRegistry UmiUiTreeModelRegistry;

UmiStatus umi_ui_tree_model_registry_create(UmiUiTreeModelRegistry **out_registry);
void umi_ui_tree_model_registry_destroy(UmiUiTreeModelRegistry *registry);
UmiStatus umi_ui_tree_model_registry_upsert(UmiUiTreeModelRegistry *registry, const UmiUiTreeModelSnapshot *item);
UmiStatus umi_ui_tree_model_registry_remove(UmiUiTreeModelRegistry *registry, const char *id);
UmiStatus umi_ui_tree_model_registry_find(const UmiUiTreeModelRegistry *registry, const char *id, UmiUiTreeModelSnapshot *out_item);
UmiStatus umi_ui_tree_model_registry_at(const UmiUiTreeModelRegistry *registry, size_t index, UmiUiTreeModelSnapshot *out_item);
UmiStatus umi_ui_tree_model_registry_set_expanded(UmiUiTreeModelRegistry *registry,
                                                    const char *id,
                                                    int expanded);
size_t umi_ui_tree_model_registry_child_count(const UmiUiTreeModelRegistry *registry,
                                              const char *parent_id);
UmiStatus umi_ui_tree_model_registry_child_at(const UmiUiTreeModelRegistry *registry,
                                              const char *parent_id,
                                              size_t child_index,
                                              UmiUiTreeModelSnapshot *out_item);
size_t umi_ui_tree_model_registry_count(const UmiUiTreeModelRegistry *registry);
uint64_t umi_ui_tree_model_registry_revision(const UmiUiTreeModelRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
