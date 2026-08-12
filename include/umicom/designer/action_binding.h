/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/action_binding.h
 *
 * PURPOSE:
 *   Define visual-designer action bindings to canonical Framework commands.
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
#ifndef UMICOM_DESIGNER_ACTION_BINDING_H
#define UMICOM_DESIGNER_ACTION_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_ACTION_BINDING_CAPACITY 2048U

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

typedef struct UmiDesignerActionBindingRegistry UmiDesignerActionBindingRegistry;

UmiStatus umi_designer_action_binding_registry_create(UmiDesignerActionBindingRegistry **out_registry);
void umi_designer_action_binding_registry_destroy(UmiDesignerActionBindingRegistry *registry);
UmiStatus umi_designer_action_binding_registry_upsert(UmiDesignerActionBindingRegistry *registry, const UmiDesignerActionBindingSnapshot *item);
UmiStatus umi_designer_action_binding_registry_remove(UmiDesignerActionBindingRegistry *registry, const char *id);
UmiStatus umi_designer_action_binding_registry_find(const UmiDesignerActionBindingRegistry *registry, const char *id, UmiDesignerActionBindingSnapshot *out_item);
UmiStatus umi_designer_action_binding_registry_at(const UmiDesignerActionBindingRegistry *registry, size_t index, UmiDesignerActionBindingSnapshot *out_item);
size_t umi_designer_action_binding_registry_count(const UmiDesignerActionBindingRegistry *registry);
uint64_t umi_designer_action_binding_registry_revision(const UmiDesignerActionBindingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
