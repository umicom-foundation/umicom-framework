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

typedef struct UmiFrontendWidgetRegistry UmiFrontendWidgetRegistry;

UmiStatus umi_frontend_widget_tree_registry_create(UmiFrontendWidgetRegistry **out_registry);
void umi_frontend_widget_tree_registry_destroy(UmiFrontendWidgetRegistry *registry);
UmiStatus umi_frontend_widget_tree_registry_upsert(UmiFrontendWidgetRegistry *registry, const UmiFrontendWidgetSnapshot *item);
UmiStatus umi_frontend_widget_tree_registry_remove(UmiFrontendWidgetRegistry *registry, const char *id);
UmiStatus umi_frontend_widget_tree_registry_find(const UmiFrontendWidgetRegistry *registry, const char *id, UmiFrontendWidgetSnapshot *out_item);
UmiStatus umi_frontend_widget_tree_registry_at(const UmiFrontendWidgetRegistry *registry, size_t index, UmiFrontendWidgetSnapshot *out_item);
size_t umi_frontend_widget_tree_registry_count(const UmiFrontendWidgetRegistry *registry);
uint64_t umi_frontend_widget_tree_registry_revision(const UmiFrontendWidgetRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
