/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_ui_component_registry.c
 *
 * PURPOSE:
 *   Test one reusable toolkit-neutral UI component behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/registry.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiComponentRegistry registry;
    UmiUiComponentSpec spec =
        umi_ui_component_spec_default(UMI_UI_COMPONENT_LABEL);
    UmiUiComponentSpec found;

    umi_ui_component_registry_init(&registry);
    (void)umi_ui_component_spec_set_id(&spec, "title");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ui_component_registry_add(&registry, &spec) != UMI_STATUS_OK) {
        return 1;
    }
    return umi_ui_component_registry_find(&registry, "title", &found) ==
                   UMI_STATUS_OK
               ? 0
               : 2;
}
