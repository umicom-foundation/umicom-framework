/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/behavior_profiles/shell/shell_multi_monitor.inc
 *
 * PURPOSE:
 *   Define live refresh, selection, persistence, command and context behavior
 *   for the reusable umicom.shell.multi-monitor application component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
SURFACE_BEHAVIOR("umicom.shell.multi-monitor",
                 UMI_APPLICATION_PRESENTATION_STATE_READY,
                 UMI_APPLICATION_PRESENTATION_REFRESH_STREAMING, 5U,
                 UMI_APPLICATION_PRESENTATION_SELECTION_SINGLE,
                 UMI_APPLICATION_PRESENTATION_PERSISTENCE_TRANSIENT,
                 UMI_APPLICATION_PRESENTATION_COMMAND_READ_ONLY,
                 "umicom.context.selection", 0, 1, 1, 0,
                 UMI_APPLICATION_PRESENTATION_CONNECTIVITY_OFFLINE,
                 UMI_APPLICATION_PRESENTATION_DATA_INTERNAL),
