/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/behavior_profiles/operations/operations_logs.inc
 *
 * PURPOSE:
 *   Define live refresh, selection, persistence, command and context behavior
 *   for the reusable umicom.operations.logs application component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
SURFACE_BEHAVIOR("umicom.operations.logs",
                 UMI_APPLICATION_PRESENTATION_STATE_READY,
                 UMI_APPLICATION_PRESENTATION_REFRESH_STREAMING, 5U,
                 UMI_APPLICATION_PRESENTATION_SELECTION_NONE,
                 UMI_APPLICATION_PRESENTATION_PERSISTENCE_TRANSIENT,
                 UMI_APPLICATION_PRESENTATION_COMMAND_READ_ONLY,
                 "umicom.context.selection", 0, 1, 0, 1,
                 UMI_APPLICATION_PRESENTATION_CONNECTIVITY_OFFLINE,
                 UMI_APPLICATION_PRESENTATION_DATA_INTERNAL),
