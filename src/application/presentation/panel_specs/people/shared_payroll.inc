/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/people/shared_payroll.inc
 *
 * PURPOSE:
 *   Define the portable panel presentation contract for umicom.shared.payroll.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.shared.payroll", "umicom.icon.shared.payroll",
           UMI_APPLICATION_PRESENTATION_SURFACE_DASHBOARD,
           UMI_APPLICATION_PRESENTATION_CHROME_DATA,
           UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 560U, 320U,
           UMI_APPLICATION_PRESENTATION_EMPTY_REFRESH,
           1, 1, 1, 1,
                 UMI_APPLICATION_PRESENTATION_COMPACT_KEEP),
