/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/behavior_profiles/development/development_extensions.inc
 *
 * PURPOSE:
 *   Refresh the extension inventory on demand and retain its workspace view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
SURFACE_BEHAVIOR("umicom.development.extensions",
                 UMI_APPLICATION_PRESENTATION_STATE_EMPTY,
                 UMI_APPLICATION_PRESENTATION_REFRESH_MANUAL, 0U,
                 UMI_APPLICATION_PRESENTATION_SELECTION_SINGLE,
                 UMI_APPLICATION_PRESENTATION_PERSISTENCE_WORKSPACE,
                 UMI_APPLICATION_PRESENTATION_COMMAND_TRANSACTIONAL,
                 "umicom.context.source", 0, 1, 1, 1,
                 UMI_APPLICATION_PRESENTATION_CONNECTIVITY_OFFLINE,
                 UMI_APPLICATION_PRESENTATION_DATA_INTERNAL),
