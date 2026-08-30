/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/workspace_policies/games/standard.inc
 *
 * PURPOSE:
 *   Define portable startup, checkpoint, focus and background behavior for
 *   the org.umicom.workspace.games.standard workspace recipe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
WORKSPACE_RUNTIME_POLICY("org.umicom.workspace.games.standard",
                         UMI_APPLICATION_PRESENTATION_STARTUP_RESTORE,
                         UMI_APPLICATION_PRESENTATION_CHECKPOINT_PERIODIC, 30U,
                         UMI_APPLICATION_PRESENTATION_FOCUS_LAST_ACTIVE,
                         UMI_APPLICATION_PRESENTATION_BACKGROUND_REDUCED,
                         1, 1, 1),
