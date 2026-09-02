/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/keymap.h
 *
 * PURPOSE:
 *   Resolve platform-independent key chords into designer command identifiers
 *   and contexts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_KEYMAP_H
#define UMICOM_WORKBENCH_DESIGNER_KEYMAP_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer keybinding data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerKeybinding {
    char binding_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char command_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char chord[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char context[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    bool enabled;
    bool user_defined;
    uint32_t order;
} UmiWorkbenchDesignerKeybinding;

/**
 * Represent the workbench designer keymap data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerKeymap {
    UmiWorkbenchDesignerKeybinding bindings[UMI_WORKBENCH_DESIGNER_MAX_KEYBINDINGS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerKeymap;

/**
 * Initialise workbench designer keymap from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_keymap_init(UmiWorkbenchDesignerKeymap *keymap);
/**
 * Add workbench designer keymap only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_designer_keymap_add(UmiWorkbenchDesignerKeymap *keymap, const UmiWorkbenchDesignerKeybinding *binding);
/**
 * Provide the workbench designer keymap seed defaults operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_keymap_seed_defaults(UmiWorkbenchDesignerKeymap *keymap);
/**
 * Provide the workbench designer keymap resolve operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerKeybinding *umi_workbench_designer_keymap_resolve(const UmiWorkbenchDesignerKeymap *keymap, const char *chord, const char *context);
/**
 * Provide the workbench designer keymap set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_keymap_set_enabled(UmiWorkbenchDesignerKeymap *keymap, const char *binding_id, bool enabled);

#ifdef __cplusplus
}
#endif

#endif
