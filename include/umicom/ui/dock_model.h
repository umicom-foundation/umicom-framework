/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/dock_model.h
 *
 * PURPOSE:
 *   Define persistent dock areas and dock groups inspired by mature multi-pane workbenches.
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
#ifndef UMICOM_UI_DOCK_MODEL_H
#define UMICOM_UI_DOCK_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_DOCK_MODEL_CAPACITY 256U

/**
 * Represent the ui dock snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiDockSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char title[256];
    char area[64];
    char group_id[128];
    char active_item_id[128];
    int visible;
    int locked;
    int floating;
    int32_t order;
    int32_t size;
    uint64_t revision;
} UmiUiDockSnapshot;

/**
 * Represent the ui dock registry data shared with callers of this public contract.
 */
typedef struct UmiUiDockRegistry UmiUiDockRegistry;

/**
 * Initialise ui dock model registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_dock_model_registry_create(UmiUiDockRegistry **out_registry);
/**
 * Release or reset state held by ui dock model registry so the same storage can be reused
 * safely.
 */
void umi_ui_dock_model_registry_destroy(UmiUiDockRegistry *registry);
/**
 * Provide the ui dock model registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_dock_model_registry_upsert(UmiUiDockRegistry *registry, const UmiUiDockSnapshot *item);
/**
 * Remove ui dock model registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_dock_model_registry_remove(UmiUiDockRegistry *registry, const char *id);
/**
 * Find ui dock model registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_dock_model_registry_find(const UmiUiDockRegistry *registry, const char *id, UmiUiDockSnapshot *out_item);
/**
 * Find ui dock model registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_dock_model_registry_at(const UmiUiDockRegistry *registry, size_t index, UmiUiDockSnapshot *out_item);
/**
 * Return the number of records represented by ui dock model registry without changing
 * their state.
 */
size_t umi_ui_dock_model_registry_count(const UmiUiDockRegistry *registry);
/**
 * Provide the ui dock model registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_dock_model_registry_revision(const UmiUiDockRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
