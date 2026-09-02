/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/behavior_profiles/quality/quality_automated_tests.inc
 *
 * PURPOSE:
 *   Keep an acceptance run for the current session and require explicit user
 *   actions before a scenario can change the application under test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
SURFACE_BEHAVIOR("umicom.quality.automated-tests",
                 UMI_APPLICATION_PRESENTATION_STATE_EMPTY,
                 UMI_APPLICATION_PRESENTATION_REFRESH_MANUAL, 0U,
                 UMI_APPLICATION_PRESENTATION_SELECTION_SINGLE,
                 UMI_APPLICATION_PRESENTATION_PERSISTENCE_SESSION,
                 UMI_APPLICATION_PRESENTATION_COMMAND_TRANSACTIONAL,
                 "umicom.context.source", 0, 1, 1, 1),
