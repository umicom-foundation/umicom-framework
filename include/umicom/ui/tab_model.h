/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/tab_model.h
 *
 * PURPOSE:
 *   Define editor and tool tabs with preview, pinning, dirty-state and grouping semantics.
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
#ifndef UMICOM_UI_TAB_MODEL_H
#define UMICOM_UI_TAB_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_TAB_MODEL_CAPACITY 1024U

/**
 * Represent the ui tab snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiTabSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char group_id[128];
    char title[256];
    char resource[512];
    char icon_name[128];
    int active;
    int pinned;
    int preview;
    int dirty;
    int closable;
    int32_t order;
    uint64_t revision;
} UmiUiTabSnapshot;

/**
 * Represent the ui tab registry data shared with callers of this public contract.
 */
typedef struct UmiUiTabRegistry UmiUiTabRegistry;

/**
 * Initialise ui tab model registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_tab_model_registry_create(UmiUiTabRegistry **out_registry);
/**
 * Release or reset state held by ui tab model registry so the same storage can be reused
 * safely.
 */
void umi_ui_tab_model_registry_destroy(UmiUiTabRegistry *registry);
/**
 * Provide the ui tab model registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_tab_model_registry_upsert(UmiUiTabRegistry *registry, const UmiUiTabSnapshot *item);
/**
 * Remove ui tab model registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_tab_model_registry_remove(UmiUiTabRegistry *registry, const char *id);
/**
 * Find ui tab model registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_tab_model_registry_find(const UmiUiTabRegistry *registry, const char *id, UmiUiTabSnapshot *out_item);
/**
 * Find ui tab model registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_tab_model_registry_at(const UmiUiTabRegistry *registry, size_t index, UmiUiTabSnapshot *out_item);
/**
 * Provide the ui tab model registry activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_tab_model_registry_activate(UmiUiTabRegistry *registry,
                                               const char *id);
/**
 * Provide the ui tab model registry set dirty operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_tab_model_registry_set_dirty(UmiUiTabRegistry *registry,
                                              const char *id,
                                              int dirty);
/**
 * Return the number of records represented by ui tab model registry without changing their
 * state.
 */
size_t umi_ui_tab_model_registry_count(const UmiUiTabRegistry *registry);
/**
 * Provide the ui tab model registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_tab_model_registry_revision(const UmiUiTabRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
