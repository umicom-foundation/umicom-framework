/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/sort_filter_model.h
 *
 * PURPOSE:
 *   Define reusable filter and sort descriptors shared by list, tree and table views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_UI_SORT_FILTER_MODEL_H
#define UMICOM_UI_SORT_FILTER_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_SORT_FILTER_MODEL_CAPACITY 128U

typedef struct UmiUiSortFilterSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char query[256];
    char sort_key[128];
    int ascending;
    int case_sensitive;
    int enabled;
    int32_t priority;
    uint64_t revision;
} UmiUiSortFilterSnapshot;

typedef struct UmiUiSortFilterRegistry UmiUiSortFilterRegistry;

UmiStatus umi_ui_sort_filter_model_registry_create(UmiUiSortFilterRegistry **out_registry);
void umi_ui_sort_filter_model_registry_destroy(UmiUiSortFilterRegistry *registry);
UmiStatus umi_ui_sort_filter_model_registry_upsert(UmiUiSortFilterRegistry *registry, const UmiUiSortFilterSnapshot *item);
UmiStatus umi_ui_sort_filter_model_registry_remove(UmiUiSortFilterRegistry *registry, const char *id);
UmiStatus umi_ui_sort_filter_model_registry_find(const UmiUiSortFilterRegistry *registry, const char *id, UmiUiSortFilterSnapshot *out_item);
UmiStatus umi_ui_sort_filter_model_registry_at(const UmiUiSortFilterRegistry *registry, size_t index, UmiUiSortFilterSnapshot *out_item);
int umi_ui_sort_filter_model_matches(const UmiUiSortFilterSnapshot *filter,
                                     const char *text);
int umi_ui_sort_filter_model_compare_text(const UmiUiSortFilterSnapshot *filter,
                                          const char *left,
                                          const char *right);
size_t umi_ui_sort_filter_model_registry_count(const UmiUiSortFilterRegistry *registry);
uint64_t umi_ui_sort_filter_model_registry_revision(const UmiUiSortFilterRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
