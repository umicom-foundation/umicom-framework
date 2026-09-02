/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/widget_tree.h
 *
 * PURPOSE:
 *   Define a frontend-neutral widget tree reusable by web, GTK and headless renderers.
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
#ifndef UMICOM_FRONTEND_WIDGET_TREE_H
#define UMICOM_FRONTEND_WIDGET_TREE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FRONTEND_WIDGET_TREE_CAPACITY 4096U

/**
 * Represent the frontend widget snapshot data shared with callers of this public contract.
 */
typedef struct UmiFrontendWidgetSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char parent_id[128];
    char type[128];
    char text[512];
    char style_class[256];
    int visible;
    int enabled;
    int32_t order;
    uint64_t revision;
} UmiFrontendWidgetSnapshot;

/**
 * Represent the frontend widget registry data shared with callers of this public contract.
 */
typedef struct UmiFrontendWidgetRegistry UmiFrontendWidgetRegistry;

/**
 * Initialise frontend widget tree registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_widget_tree_registry_create(UmiFrontendWidgetRegistry **out_registry);
/**
 * Release or reset state held by frontend widget tree registry so the same storage can be
 * reused safely.
 */
void umi_frontend_widget_tree_registry_destroy(UmiFrontendWidgetRegistry *registry);
/**
 * Provide the frontend widget tree registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_widget_tree_registry_upsert(UmiFrontendWidgetRegistry *registry, const UmiFrontendWidgetSnapshot *item);
/**
 * Remove frontend widget tree registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_frontend_widget_tree_registry_remove(UmiFrontendWidgetRegistry *registry, const char *id);
/**
 * Find frontend widget tree registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_widget_tree_registry_find(const UmiFrontendWidgetRegistry *registry, const char *id, UmiFrontendWidgetSnapshot *out_item);
/**
 * Find frontend widget tree registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_widget_tree_registry_at(const UmiFrontendWidgetRegistry *registry, size_t index, UmiFrontendWidgetSnapshot *out_item);
/**
 * Return the number of records represented by frontend widget tree registry without
 * changing their state.
 */
size_t umi_frontend_widget_tree_registry_count(const UmiFrontendWidgetRegistry *registry);
/**
 * Provide the frontend widget tree registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_frontend_widget_tree_registry_revision(const UmiFrontendWidgetRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
