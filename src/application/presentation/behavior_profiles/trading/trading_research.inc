/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/behavior_profiles/trading/trading_research.inc
 *
 * PURPOSE:
 *   Refresh research when focused and follow the instrument selected in another
 *   linked trading panel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
SURFACE_BEHAVIOR("umicom.trading.research",
                 UMI_APPLICATION_PRESENTATION_STATE_READY,
                 UMI_APPLICATION_PRESENTATION_REFRESH_ON_FOCUS, 0U,
                 UMI_APPLICATION_PRESENTATION_SELECTION_SINGLE,
                 UMI_APPLICATION_PRESENTATION_PERSISTENCE_WORKSPACE,
                 UMI_APPLICATION_PRESENTATION_COMMAND_READ_ONLY,
                 "umicom.context.instrument", 0, 1, 1, 1,
                 UMI_APPLICATION_PRESENTATION_CONNECTIVITY_OPTIONAL,
                 UMI_APPLICATION_PRESENTATION_DATA_RESTRICTED),
