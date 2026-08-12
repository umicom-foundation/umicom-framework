/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/render_tree.h
 *
 * PURPOSE:
 *   Define incremental render-tree snapshots for efficient multi-frontend updates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_FRONTEND_RENDER_TREE_H
#define UMICOM_FRONTEND_RENDER_TREE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FRONTEND_RENDER_TREE_CAPACITY 4096U

typedef struct UmiFrontendRenderNodeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char widget_id[128];
    char parent_id[128];
    char markup[1024];
    uint64_t checksum;
    int dirty;
    int32_t order;
    uint64_t revision;
} UmiFrontendRenderNodeSnapshot;

typedef struct UmiFrontendRenderNodeRegistry UmiFrontendRenderNodeRegistry;

UmiStatus umi_frontend_render_tree_registry_create(UmiFrontendRenderNodeRegistry **out_registry);
void umi_frontend_render_tree_registry_destroy(UmiFrontendRenderNodeRegistry *registry);
UmiStatus umi_frontend_render_tree_registry_upsert(UmiFrontendRenderNodeRegistry *registry, const UmiFrontendRenderNodeSnapshot *item);
UmiStatus umi_frontend_render_tree_registry_remove(UmiFrontendRenderNodeRegistry *registry, const char *id);
UmiStatus umi_frontend_render_tree_registry_find(const UmiFrontendRenderNodeRegistry *registry, const char *id, UmiFrontendRenderNodeSnapshot *out_item);
UmiStatus umi_frontend_render_tree_registry_at(const UmiFrontendRenderNodeRegistry *registry, size_t index, UmiFrontendRenderNodeSnapshot *out_item);
size_t umi_frontend_render_tree_registry_count(const UmiFrontendRenderNodeRegistry *registry);
uint64_t umi_frontend_render_tree_registry_revision(const UmiFrontendRenderNodeRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
