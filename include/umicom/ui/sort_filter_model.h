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

/**
 * Represent the ui sort filter snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the ui sort filter registry data shared with callers of this public contract.
 */
typedef struct UmiUiSortFilterRegistry UmiUiSortFilterRegistry;

/**
 * Initialise ui sort filter model registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_sort_filter_model_registry_create(UmiUiSortFilterRegistry **out_registry);
/**
 * Release or reset state held by ui sort filter model registry so the same storage can be
 * reused safely.
 */
void umi_ui_sort_filter_model_registry_destroy(UmiUiSortFilterRegistry *registry);
/**
 * Provide the ui sort filter model registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_sort_filter_model_registry_upsert(UmiUiSortFilterRegistry *registry, const UmiUiSortFilterSnapshot *item);
/**
 * Remove ui sort filter model registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_sort_filter_model_registry_remove(UmiUiSortFilterRegistry *registry, const char *id);
/**
 * Find ui sort filter model registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_sort_filter_model_registry_find(const UmiUiSortFilterRegistry *registry, const char *id, UmiUiSortFilterSnapshot *out_item);
/**
 * Find ui sort filter model registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_sort_filter_model_registry_at(const UmiUiSortFilterRegistry *registry, size_t index, UmiUiSortFilterSnapshot *out_item);
/**
 * Provide the ui sort filter model matches operation used by this module and its client
 * applications.
 */
int umi_ui_sort_filter_model_matches(const UmiUiSortFilterSnapshot *filter,
                                     const char *text);
/**
 * Provide the ui sort filter model compare text operation used by this module and its
 * client applications.
 */
int umi_ui_sort_filter_model_compare_text(const UmiUiSortFilterSnapshot *filter,
                                          const char *left,
                                          const char *right);
/**
 * Return the number of records represented by ui sort filter model registry without
 * changing their state.
 */
size_t umi_ui_sort_filter_model_registry_count(const UmiUiSortFilterRegistry *registry);
/**
 * Provide the ui sort filter model registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_sort_filter_model_registry_revision(const UmiUiSortFilterRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
