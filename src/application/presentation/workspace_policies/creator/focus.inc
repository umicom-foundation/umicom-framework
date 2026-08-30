/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/workspace_policies/creator/focus.inc
 *
 * PURPOSE:
 *   Define portable startup, checkpoint, focus and background behavior for
 *   the org.umicom.workspace.creator.focus workspace recipe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
WORKSPACE_RUNTIME_POLICY("org.umicom.workspace.creator.focus",
                         UMI_APPLICATION_PRESENTATION_STARTUP_PRIMARY_FIRST,
                         UMI_APPLICATION_PRESENTATION_CHECKPOINT_ON_CHANGE, 0U,
                         UMI_APPLICATION_PRESENTATION_FOCUS_PRIMARY,
                         UMI_APPLICATION_PRESENTATION_BACKGROUND_PAUSED,
                         0, 1, 0),
