/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/keymap.c
 *
 * PURPOSE:
 *   Implement deterministic keybinding lookup with user override precedence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/keymap.h"
#include "internal.h"


/*
 * Initialise workbench designer keymap from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_keymap_init(UmiWorkbenchDesignerKeymap *keymap)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (keymap == NULL) return;
    (void)memset(keymap, 0, sizeof(*keymap));
}

/*
 * Add workbench designer keymap only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_designer_keymap_add(
    UmiWorkbenchDesignerKeymap *keymap,
    const UmiWorkbenchDesignerKeybinding *binding)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (keymap == NULL || binding == NULL || binding->binding_id[0] == '\0' ||
        binding->command_id[0] == '\0' || binding->chord[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < keymap->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(keymap->bindings[index].binding_id, binding->binding_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (keymap->count >= UMI_WORKBENCH_DESIGNER_MAX_KEYBINDINGS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    keymap->bindings[keymap->count++] = *binding;
    keymap->revision += 1U;
    return UMI_STATUS_OK;
}

/* Add keymap only after its inputs and available capacity have been checked. */
static UmiStatus keymap_add(
    UmiWorkbenchDesignerKeymap *keymap,
    const char *id,
    const char *command,
    const char *chord,
    const char *context,
    uint32_t order)
{
    UmiWorkbenchDesignerKeybinding binding;
    (void)memset(&binding, 0, sizeof(binding));
    (void)umi_workbench_designer_copy_text(binding.binding_id, sizeof(binding.binding_id), id);
    (void)umi_workbench_designer_copy_text(binding.command_id, sizeof(binding.command_id), command);
    (void)umi_workbench_designer_copy_text(binding.chord, sizeof(binding.chord), chord);
    (void)umi_workbench_designer_copy_text(binding.context, sizeof(binding.context), context);
    binding.enabled = true;
    binding.order = order;
    return umi_workbench_designer_keymap_add(keymap, &binding);
}

/*
 * Provide the workbench designer keymap seed defaults operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_keymap_seed_defaults(
    UmiWorkbenchDesignerKeymap *keymap)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (keymap == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = keymap_add(keymap, "undo", "designer.undo", "Ctrl+Z", "designer", 10U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = keymap_add(keymap, "redo", "designer.redo", "Ctrl+Shift+Z", "designer", 20U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = keymap_add(keymap, "save", "designer.save", "Ctrl+S", "designer", 30U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = keymap_add(keymap, "copy", "designer.copy", "Ctrl+C", "designer", 40U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = keymap_add(keymap, "cut", "designer.cut", "Ctrl+X", "designer", 50U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = keymap_add(keymap, "paste", "designer.paste", "Ctrl+V", "designer", 60U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = keymap_add(keymap, "delete", "designer.delete", "Delete", "designer", 70U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = keymap_add(keymap, "palette", "designer.focus-palette", "Ctrl+Shift+P", "designer", 80U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = keymap_add(keymap, "browser", "designer.layout-browser", "Ctrl+Alt+L", "designer", 90U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = keymap_add(keymap, "preview", "designer.preview", "Ctrl+Shift+Enter", "designer", 100U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = keymap_add(keymap, "zoom-in", "designer.zoom-in", "Ctrl++", "canvas", 110U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = keymap_add(keymap, "zoom-out", "designer.zoom-out", "Ctrl+-", "canvas", 120U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = keymap_add(keymap, "zoom-reset", "designer.zoom-reset", "Ctrl+0", "canvas", 130U);
    return status;
}

/*
 * Provide the workbench designer keymap resolve operation used by this module and its
 * client applications.
 */
const UmiWorkbenchDesignerKeybinding *umi_workbench_designer_keymap_resolve(
    const UmiWorkbenchDesignerKeymap *keymap,
    const char *chord,
    const char *context)
{
    const UmiWorkbenchDesignerKeybinding *best = NULL;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (keymap == NULL || chord == NULL || context == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < keymap->count; ++index) {
        const UmiWorkbenchDesignerKeybinding *binding = &keymap->bindings[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!binding->enabled || strcmp(binding->chord, chord) != 0 ||
            (binding->context[0] != '\0' && strcmp(binding->context, context) != 0)) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (best == NULL || (binding->user_defined && !best->user_defined) ||
            (binding->user_defined == best->user_defined && binding->order > best->order)) {
            best = binding;
        }
    }
    return best;
}

/*
 * Provide the workbench designer keymap set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_keymap_set_enabled(
    UmiWorkbenchDesignerKeymap *keymap,
    const char *binding_id,
    bool enabled)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (keymap == NULL || binding_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < keymap->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(keymap->bindings[index].binding_id, binding_id) == 0) {
            keymap->bindings[index].enabled = enabled;
            keymap->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
