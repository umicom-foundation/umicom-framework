/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/signal.h
 *
 * PURPOSE:
 *   Define signal-to-command bindings for server-side and desktop frontend composition.
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
#ifndef UMICOM_FRONTEND_SIGNAL_H
#define UMICOM_FRONTEND_SIGNAL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FRONTEND_SIGNAL_CAPACITY 2048U

typedef struct UmiFrontendSignalSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char widget_id[128];
    char signal_name[128];
    char command_id[128];
    char argument[512];
    int enabled;
    int once;
    uint64_t revision;
} UmiFrontendSignalSnapshot;

typedef struct UmiFrontendSignalRegistry UmiFrontendSignalRegistry;

UmiStatus umi_frontend_signal_registry_create(UmiFrontendSignalRegistry **out_registry);
void umi_frontend_signal_registry_destroy(UmiFrontendSignalRegistry *registry);
UmiStatus umi_frontend_signal_registry_upsert(UmiFrontendSignalRegistry *registry, const UmiFrontendSignalSnapshot *item);
UmiStatus umi_frontend_signal_registry_remove(UmiFrontendSignalRegistry *registry, const char *id);
UmiStatus umi_frontend_signal_registry_find(const UmiFrontendSignalRegistry *registry, const char *id, UmiFrontendSignalSnapshot *out_item);
UmiStatus umi_frontend_signal_registry_at(const UmiFrontendSignalRegistry *registry, size_t index, UmiFrontendSignalSnapshot *out_item);
size_t umi_frontend_signal_registry_count(const UmiFrontendSignalRegistry *registry);
uint64_t umi_frontend_signal_registry_revision(const UmiFrontendSignalRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
