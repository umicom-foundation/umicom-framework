/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/behavior_profiles/healthcare/synthetic_access.inc
 *
 * PURPOSE:
 *   Describe the existing fictional access-request workspace as offline,
 *   transient and isolated from cross-application selection contexts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* This metadata describes local synthetic editing only. It does not enable
 * clinical actions, provider connections or persistence of patient records. */
SURFACE_BEHAVIOR("umicom.healthcare.synthetic-access",
                 UMI_APPLICATION_PRESENTATION_STATE_READY,
                 UMI_APPLICATION_PRESENTATION_REFRESH_MANUAL, 0U,
                 UMI_APPLICATION_PRESENTATION_SELECTION_SINGLE,
                 UMI_APPLICATION_PRESENTATION_PERSISTENCE_TRANSIENT,
                 UMI_APPLICATION_PRESENTATION_COMMAND_EDITABLE,
                 "umicom.context.healthcare.synthetic", 0, 0, 0, 0,
                 UMI_APPLICATION_PRESENTATION_CONNECTIVITY_OFFLINE,
                 UMI_APPLICATION_PRESENTATION_DATA_INTERNAL),
