/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/keybinding.h
 *
 * PURPOSE:
 *   Register keyboard chords and resolve them to canonical command identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Keybindings refer to Framework command IDs. They never contain function pointers or GTK callbacks, so the same map works in different frontends.
 */

#ifndef UMICOM_UI_KEYBINDING_H
#define UMICOM_UI_KEYBINDING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/context.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_KEYBINDING_MAX 512U
#define UMI_UI_CHORD_CAPACITY 96U
#define UMI_UI_WHEN_CAPACITY 256U

/**
 * Represent the ui keybinding snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiKeybindingSnapshot {
    char binding_id[UMI_UI_ID_CAPACITY];
    char command_id[UMI_UI_ID_CAPACITY];
    char chord[UMI_UI_CHORD_CAPACITY];
    char argument[UMI_UI_TEXT_CAPACITY];
    char when_expression[UMI_UI_WHEN_CAPACITY];
    int32_t order;
    int enabled;
} UmiUiKeybindingSnapshot;

/**
 * Represent the ui keybinding resolution data shared with callers of this public contract.
 */
typedef struct UmiUiKeybindingResolution {
    char binding_id[UMI_UI_ID_CAPACITY];
    char command_id[UMI_UI_ID_CAPACITY];
    char argument[UMI_UI_TEXT_CAPACITY];
} UmiUiKeybindingResolution;

/**
 * Represent the ui keybinding registry data shared with callers of this public contract.
 */
typedef struct UmiUiKeybindingRegistry UmiUiKeybindingRegistry;

/**
 * Initialise ui keybinding registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_keybinding_registry_create(UmiUiKeybindingRegistry **out_registry);
/**
 * Release or reset state held by ui keybinding registry so the same storage can be reused
 * safely.
 */
void umi_ui_keybinding_registry_destroy(UmiUiKeybindingRegistry *registry);
/**
 * Provide the ui keybinding registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_keybinding_registry_upsert(UmiUiKeybindingRegistry *registry,
                                            const UmiUiKeybindingSnapshot *item);
/**
 * Remove ui keybinding registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_keybinding_registry_remove(UmiUiKeybindingRegistry *registry,
                                            const char *binding_id);
/**
 * Find ui keybinding registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_keybinding_registry_find(const UmiUiKeybindingRegistry *registry,
                                          const char *binding_id,
                                          UmiUiKeybindingSnapshot *out_item);
/**
 * Find ui keybinding registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_keybinding_registry_at(const UmiUiKeybindingRegistry *registry,
                                        size_t index,
                                        UmiUiKeybindingSnapshot *out_item);
/**
 * Provide the ui keybinding registry resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_keybinding_registry_resolve(
    const UmiUiKeybindingRegistry *registry,
    const UmiUiContextStore *context,
    const char *chord,
    UmiUiKeybindingResolution *out_resolution);
/**
 * Return the number of records represented by ui keybinding registry without changing
 * their state.
 */
size_t umi_ui_keybinding_registry_count(const UmiUiKeybindingRegistry *registry);
/**
 * Provide the ui keybinding registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_keybinding_registry_revision(const UmiUiKeybindingRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
