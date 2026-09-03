/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/trading/trading_options.inc
 *
 * PURPOSE:
 *   Describe the reusable options analysis surface so trading applications
 *   can share contracts for chains, strategies and scenario views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.trading.options", "umicom.icon.trading.options",
           UMI_APPLICATION_PRESENTATION_SURFACE_TOOL,
           UMI_APPLICATION_PRESENTATION_CHROME_DATA,
           UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 640U, 400U,
           UMI_APPLICATION_PRESENTATION_EMPTY_SELECT,
           1, 1, 1, 1,
                 UMI_APPLICATION_PRESENTATION_COMPACT_KEEP),
