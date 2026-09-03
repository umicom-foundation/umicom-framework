/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/behavior_profiles/development/development_live_preview.inc
 *
 * PURPOSE:
 *   Define refresh, persistence and context behaviour for the active-document
 *   preview shared by developer applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
SURFACE_BEHAVIOR("umicom.development.live-preview",
                 UMI_APPLICATION_PRESENTATION_STATE_EMPTY,
                 UMI_APPLICATION_PRESENTATION_REFRESH_ON_FOCUS, 0U,
                 UMI_APPLICATION_PRESENTATION_SELECTION_SINGLE,
                 UMI_APPLICATION_PRESENTATION_PERSISTENCE_WORKSPACE,
                 UMI_APPLICATION_PRESENTATION_COMMAND_READ_ONLY,
                 "umicom.context.source", 0, 1, 0, 1,
                 UMI_APPLICATION_PRESENTATION_CONNECTIVITY_OPTIONAL,
                 UMI_APPLICATION_PRESENTATION_DATA_INTERNAL),
