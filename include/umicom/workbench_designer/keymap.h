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


typedef struct UmiWorkbenchDesignerKeybinding {
    char binding_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char command_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char chord[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char context[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    bool enabled;
    bool user_defined;
    uint32_t order;
} UmiWorkbenchDesignerKeybinding;

typedef struct UmiWorkbenchDesignerKeymap {
    UmiWorkbenchDesignerKeybinding bindings[UMI_WORKBENCH_DESIGNER_MAX_KEYBINDINGS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerKeymap;

void umi_workbench_designer_keymap_init(UmiWorkbenchDesignerKeymap *keymap);
UmiStatus umi_workbench_designer_keymap_add(UmiWorkbenchDesignerKeymap *keymap, const UmiWorkbenchDesignerKeybinding *binding);
UmiStatus umi_workbench_designer_keymap_seed_defaults(UmiWorkbenchDesignerKeymap *keymap);
const UmiWorkbenchDesignerKeybinding *umi_workbench_designer_keymap_resolve(const UmiWorkbenchDesignerKeymap *keymap, const char *chord, const char *context);
UmiStatus umi_workbench_designer_keymap_set_enabled(UmiWorkbenchDesignerKeymap *keymap, const char *binding_id, bool enabled);

#ifdef __cplusplus
}
#endif

#endif
