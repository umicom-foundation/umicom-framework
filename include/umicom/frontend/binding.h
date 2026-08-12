/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/binding.h
 *
 * PURPOSE:
 *   Define model-to-view bindings that can be shared by multiple frontend adapters.
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
#ifndef UMICOM_FRONTEND_BINDING_H
#define UMICOM_FRONTEND_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FRONTEND_BINDING_CAPACITY 2048U

typedef struct UmiFrontendBindingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char source_path[256];
    char target_widget_id[128];
    char target_property[128];
    char converter[128];
    int two_way;
    int enabled;
    uint64_t revision;
} UmiFrontendBindingSnapshot;

typedef struct UmiFrontendBindingRegistry UmiFrontendBindingRegistry;

UmiStatus umi_frontend_binding_registry_create(UmiFrontendBindingRegistry **out_registry);
void umi_frontend_binding_registry_destroy(UmiFrontendBindingRegistry *registry);
UmiStatus umi_frontend_binding_registry_upsert(UmiFrontendBindingRegistry *registry, const UmiFrontendBindingSnapshot *item);
UmiStatus umi_frontend_binding_registry_remove(UmiFrontendBindingRegistry *registry, const char *id);
UmiStatus umi_frontend_binding_registry_find(const UmiFrontendBindingRegistry *registry, const char *id, UmiFrontendBindingSnapshot *out_item);
UmiStatus umi_frontend_binding_registry_at(const UmiFrontendBindingRegistry *registry, size_t index, UmiFrontendBindingSnapshot *out_item);
size_t umi_frontend_binding_registry_count(const UmiFrontendBindingRegistry *registry);
uint64_t umi_frontend_binding_registry_revision(const UmiFrontendBindingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
