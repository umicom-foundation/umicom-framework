/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/behavior_profiles/treasury/treasury_trade_blotter.inc
 *
 * PURPOSE:
 *   Define live refresh, selection, persistence, command and context behavior
 *   for the reusable umicom.treasury.trade-blotter application component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
SURFACE_BEHAVIOR("umicom.treasury.trade-blotter",
                 UMI_APPLICATION_PRESENTATION_STATE_READY,
                 UMI_APPLICATION_PRESENTATION_REFRESH_MANUAL, 0U,
                 UMI_APPLICATION_PRESENTATION_SELECTION_SINGLE,
                 UMI_APPLICATION_PRESENTATION_PERSISTENCE_TRANSIENT,
                 UMI_APPLICATION_PRESENTATION_COMMAND_READ_ONLY,
                 "umicom.context.instrument", 0, 0, 1, 1,
                 UMI_APPLICATION_PRESENTATION_CONNECTIVITY_OPTIONAL,
                 UMI_APPLICATION_PRESENTATION_DATA_RESTRICTED),
