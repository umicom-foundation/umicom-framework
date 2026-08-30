/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/workspace_policies/marketplace/learning.inc
 *
 * PURPOSE:
 *   Define portable startup, checkpoint, focus and background behavior for
 *   the org.umicom.workspace.marketplace.learning workspace recipe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
WORKSPACE_RUNTIME_POLICY("org.umicom.workspace.marketplace.learning",
                         UMI_APPLICATION_PRESENTATION_STARTUP_ALL_VISIBLE,
                         UMI_APPLICATION_PRESENTATION_CHECKPOINT_ON_EXIT, 0U,
                         UMI_APPLICATION_PRESENTATION_FOCUS_FIRST_VISIBLE,
                         UMI_APPLICATION_PRESENTATION_BACKGROUND_REDUCED,
                         1, 1, 0),
