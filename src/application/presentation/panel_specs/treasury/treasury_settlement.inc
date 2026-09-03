/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/treasury/treasury_settlement.inc
 *
 * PURPOSE:
 *   Define the portable panel presentation contract for umicom.treasury.settlement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.treasury.settlement", "umicom.icon.treasury.settlement",
           UMI_APPLICATION_PRESENTATION_SURFACE_VIEW,
           UMI_APPLICATION_PRESENTATION_CHROME_STANDARD,
           UMI_APPLICATION_COMPONENT_REGION_LEFT, 360U, 280U,
           UMI_APPLICATION_PRESENTATION_EMPTY_CREATE,
           1, 1, 1, 1,
                 UMI_APPLICATION_PRESENTATION_COMPACT_AUTO_HIDE),
