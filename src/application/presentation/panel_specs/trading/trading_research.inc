/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/trading/trading_research.inc
 *
 * PURPOSE:
 *   Describe the reusable market research dashboard that combines news,
 *   economic context and analysis without coupling it to one broker adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.trading.research", "umicom.icon.trading.research",
           UMI_APPLICATION_PRESENTATION_SURFACE_DASHBOARD,
           UMI_APPLICATION_PRESENTATION_CHROME_DATA,
           UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 640U, 400U,
           UMI_APPLICATION_PRESENTATION_EMPTY_REFRESH,
           1, 1, 1, 1,
                 UMI_APPLICATION_PRESENTATION_COMPACT_KEEP),
