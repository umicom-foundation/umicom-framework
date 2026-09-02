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

/**
 * Represent the designer signal binding snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the designer signal binding registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerSignalBindingRegistry UmiDesignerSignalBindingRegistry;

/**
 * Initialise designer signal binding registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_designer_signal_binding_registry_create(UmiDesignerSignalBindingRegistry **out_registry);
/**
 * Release or reset state held by designer signal binding registry so the same storage can
 * be reused safely.
 */
void umi_designer_signal_binding_registry_destroy(UmiDesignerSignalBindingRegistry *registry);
/**
 * Provide the designer signal binding registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_designer_signal_binding_registry_upsert(UmiDesignerSignalBindingRegistry *registry, const UmiDesignerSignalBindingSnapshot *item);
/**
 * Remove designer signal binding registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_designer_signal_binding_registry_remove(UmiDesignerSignalBindingRegistry *registry, const char *id);
/**
 * Find designer signal binding registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_signal_binding_registry_find(const UmiDesignerSignalBindingRegistry *registry, const char *id, UmiDesignerSignalBindingSnapshot *out_item);
/**
 * Find designer signal binding registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_signal_binding_registry_at(const UmiDesignerSignalBindingRegistry *registry, size_t index, UmiDesignerSignalBindingSnapshot *out_item);
/**
 * Return the number of records represented by designer signal binding registry without
 * changing their state.
 */
size_t umi_designer_signal_binding_registry_count(const UmiDesignerSignalBindingRegistry *registry);
/**
 * Provide the designer signal binding registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_designer_signal_binding_registry_revision(const UmiDesignerSignalBindingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
