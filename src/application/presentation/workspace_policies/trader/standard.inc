/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/workspace_policies/trader/standard.inc
 *
 * PURPOSE:
 *   Define portable startup, checkpoint, focus and background behavior for
 *   the org.umicom.workspace.trader.standard workspace recipe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
WORKSPACE_RUNTIME_POLICY("org.umicom.workspace.trader.standard",
                         UMI_APPLICATION_PRESENTATION_STARTUP_RESTORE,
                         UMI_APPLICATION_PRESENTATION_CHECKPOINT_PERIODIC, 15U,
                         UMI_APPLICATION_PRESENTATION_FOCUS_LAST_ACTIVE,
                         UMI_APPLICATION_PRESENTATION_BACKGROUND_FULL,
                         1, 1, 1),
