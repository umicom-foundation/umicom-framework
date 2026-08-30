/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/web_session.h
 *
 * PURPOSE:
 *   Define isolated frontend sessions for server-driven interactive applications.
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
#ifndef UMICOM_FRONTEND_WEB_SESSION_H
#define UMICOM_FRONTEND_WEB_SESSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FRONTEND_WEB_SESSION_CAPACITY 1024U

typedef struct UmiFrontendSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char user_id[128];
    char route[512];
    char transport[64];
    uint64_t created_at;
    uint64_t last_activity;
    int authenticated;
    int connected;
    int suspended;
    uint64_t revision;
} UmiFrontendSessionSnapshot;

typedef struct UmiFrontendSessionRegistry UmiFrontendSessionRegistry;

UmiStatus umi_frontend_web_session_registry_create(UmiFrontendSessionRegistry **out_registry);
void umi_frontend_web_session_registry_destroy(UmiFrontendSessionRegistry *registry);
UmiStatus umi_frontend_web_session_registry_upsert(UmiFrontendSessionRegistry *registry, const UmiFrontendSessionSnapshot *item);
UmiStatus umi_frontend_web_session_registry_remove(UmiFrontendSessionRegistry *registry, const char *id);
UmiStatus umi_frontend_web_session_registry_find(const UmiFrontendSessionRegistry *registry, const char *id, UmiFrontendSessionSnapshot *out_item);
UmiStatus umi_frontend_web_session_registry_at(const UmiFrontendSessionRegistry *registry, size_t index, UmiFrontendSessionSnapshot *out_item);
UmiStatus umi_frontend_web_session_registry_touch(UmiFrontendSessionRegistry *registry,
                                                     const char *id,
                                                     uint64_t last_activity,
                                                     int connected,
                                                     int suspended);
size_t umi_frontend_web_session_registry_count(const UmiFrontendSessionRegistry *registry);
uint64_t umi_frontend_web_session_registry_revision(const UmiFrontendSessionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
