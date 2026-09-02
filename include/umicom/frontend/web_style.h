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

/*
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

/**
 * Represent the frontend style snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the frontend style registry data shared with callers of this public contract.
 */
typedef struct UmiFrontendStyleRegistry UmiFrontendStyleRegistry;

/**
 * Initialise frontend web style registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_web_style_registry_create(UmiFrontendStyleRegistry **out_registry);
/**
 * Release or reset state held by frontend web style registry so the same storage can be
 * reused safely.
 */
void umi_frontend_web_style_registry_destroy(UmiFrontendStyleRegistry *registry);
/**
 * Provide the frontend web style registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_web_style_registry_upsert(UmiFrontendStyleRegistry *registry, const UmiFrontendStyleSnapshot *item);
/**
 * Remove frontend web style registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_frontend_web_style_registry_remove(UmiFrontendStyleRegistry *registry, const char *id);
/**
 * Find frontend web style registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_web_style_registry_find(const UmiFrontendStyleRegistry *registry, const char *id, UmiFrontendStyleSnapshot *out_item);
/**
 * Find frontend web style registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_web_style_registry_at(const UmiFrontendStyleRegistry *registry, size_t index, UmiFrontendStyleSnapshot *out_item);
/**
 * Return the number of records represented by frontend web style registry without changing
 * their state.
 */
size_t umi_frontend_web_style_registry_count(const UmiFrontendStyleRegistry *registry);
/**
 * Provide the frontend web style registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_frontend_web_style_registry_revision(const UmiFrontendStyleRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
