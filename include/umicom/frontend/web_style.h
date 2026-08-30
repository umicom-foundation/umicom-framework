/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/web_style.h
 *
 * PURPOSE:
 *   Define toolkit-neutral style rules for generated web and embedded-browser frontends.
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
#ifndef UMICOM_FRONTEND_WEB_STYLE_H
#define UMICOM_FRONTEND_WEB_STYLE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FRONTEND_WEB_STYLE_CAPACITY 2048U

typedef struct UmiFrontendStyleSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char selector[256];
    char property[128];
    char value[512];
    char media_query[256];
    int32_t order;
    uint64_t revision;
} UmiFrontendStyleSnapshot;

typedef struct UmiFrontendStyleRegistry UmiFrontendStyleRegistry;

UmiStatus umi_frontend_web_style_registry_create(UmiFrontendStyleRegistry **out_registry);
void umi_frontend_web_style_registry_destroy(UmiFrontendStyleRegistry *registry);
UmiStatus umi_frontend_web_style_registry_upsert(UmiFrontendStyleRegistry *registry, const UmiFrontendStyleSnapshot *item);
UmiStatus umi_frontend_web_style_registry_remove(UmiFrontendStyleRegistry *registry, const char *id);
UmiStatus umi_frontend_web_style_registry_find(const UmiFrontendStyleRegistry *registry, const char *id, UmiFrontendStyleSnapshot *out_item);
UmiStatus umi_frontend_web_style_registry_at(const UmiFrontendStyleRegistry *registry, size_t index, UmiFrontendStyleSnapshot *out_item);
size_t umi_frontend_web_style_registry_count(const UmiFrontendStyleRegistry *registry);
uint64_t umi_frontend_web_style_registry_revision(const UmiFrontendStyleRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
