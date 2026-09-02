/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/action_binding.h
 *
 * PURPOSE:
 *   Define visual-designer action bindings to canonical Framework commands.
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
#ifndef UMICOM_DESIGNER_ACTION_BINDING_H
#define UMICOM_DESIGNER_ACTION_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_ACTION_BINDING_CAPACITY 2048U

/**
 * Represent the designer action binding snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerActionBindingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char node_id[128];
    char action_name[128];
    char command_id[128];
    char state_path[256];
    int enabled;
    uint64_t revision;
} UmiDesignerActionBindingSnapshot;

/**
 * Represent the designer action binding registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerActionBindingRegistry UmiDesignerActionBindingRegistry;

/**
 * Initialise designer action binding registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_designer_action_binding_registry_create(UmiDesignerActionBindingRegistry **out_registry);
/**
 * Release or reset state held by designer action binding registry so the same storage can
 * be reused safely.
 */
void umi_designer_action_binding_registry_destroy(UmiDesignerActionBindingRegistry *registry);
/**
 * Provide the designer action binding registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_designer_action_binding_registry_upsert(UmiDesignerActionBindingRegistry *registry, const UmiDesignerActionBindingSnapshot *item);
/**
 * Remove designer action binding registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_designer_action_binding_registry_remove(UmiDesignerActionBindingRegistry *registry, const char *id);
/**
 * Find designer action binding registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_action_binding_registry_find(const UmiDesignerActionBindingRegistry *registry, const char *id, UmiDesignerActionBindingSnapshot *out_item);
/**
 * Find designer action binding registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_action_binding_registry_at(const UmiDesignerActionBindingRegistry *registry, size_t index, UmiDesignerActionBindingSnapshot *out_item);
/**
 * Return the number of records represented by designer action binding registry without
 * changing their state.
 */
size_t umi_designer_action_binding_registry_count(const UmiDesignerActionBindingRegistry *registry);
/**
 * Provide the designer action binding registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_designer_action_binding_registry_revision(const UmiDesignerActionBindingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
