/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/behavior_profiles/trading/trading_options.inc
 *
 * PURPOSE:
 *   Stream option analytics for the shared instrument while keeping trade
 *   changes behind guarded commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
SURFACE_BEHAVIOR("umicom.trading.options",
                 UMI_APPLICATION_PRESENTATION_STATE_READY,
                 UMI_APPLICATION_PRESENTATION_REFRESH_STREAMING, 1U,
                 UMI_APPLICATION_PRESENTATION_SELECTION_SINGLE,
                 UMI_APPLICATION_PRESENTATION_PERSISTENCE_WORKSPACE,
                 UMI_APPLICATION_PRESENTATION_COMMAND_GUARDED,
                 "umicom.context.instrument", 0, 1, 1, 1,
                 UMI_APPLICATION_PRESENTATION_CONNECTIVITY_STREAMING,
                 UMI_APPLICATION_PRESENTATION_DATA_RESTRICTED),
