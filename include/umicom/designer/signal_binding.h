/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/signal_binding.h
 *
 * PURPOSE:
 *   Define visual-designer signal bindings without embedding toolkit callbacks in project files.
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
#ifndef UMICOM_DESIGNER_SIGNAL_BINDING_H
#define UMICOM_DESIGNER_SIGNAL_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_SIGNAL_BINDING_CAPACITY 2048U

typedef struct UmiDesignerSignalBindingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char node_id[128];
    char signal_name[128];
    char command_id[128];
    char argument[512];
    int enabled;
    uint64_t revision;
} UmiDesignerSignalBindingSnapshot;

typedef struct UmiDesignerSignalBindingRegistry UmiDesignerSignalBindingRegistry;

UmiStatus umi_designer_signal_binding_registry_create(UmiDesignerSignalBindingRegistry **out_registry);
void umi_designer_signal_binding_registry_destroy(UmiDesignerSignalBindingRegistry *registry);
UmiStatus umi_designer_signal_binding_registry_upsert(UmiDesignerSignalBindingRegistry *registry, const UmiDesignerSignalBindingSnapshot *item);
UmiStatus umi_designer_signal_binding_registry_remove(UmiDesignerSignalBindingRegistry *registry, const char *id);
UmiStatus umi_designer_signal_binding_registry_find(const UmiDesignerSignalBindingRegistry *registry, const char *id, UmiDesignerSignalBindingSnapshot *out_item);
UmiStatus umi_designer_signal_binding_registry_at(const UmiDesignerSignalBindingRegistry *registry, size_t index, UmiDesignerSignalBindingSnapshot *out_item);
size_t umi_designer_signal_binding_registry_count(const UmiDesignerSignalBindingRegistry *registry);
uint64_t umi_designer_signal_binding_registry_revision(const UmiDesignerSignalBindingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
