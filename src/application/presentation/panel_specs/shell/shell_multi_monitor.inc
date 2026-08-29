/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/shell/shell_multi_monitor.inc
 *
 * PURPOSE:
 *   Define the portable panel presentation contract for umicom.shell.multi-monitor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.shell.multi-monitor", "umicom.icon.shell.multi-monitor",
           UMI_APPLICATION_PRESENTATION_SURFACE_SERVICE,
           UMI_APPLICATION_PRESENTATION_CHROME_SERVICE,
           UMI_APPLICATION_COMPONENT_REGION_FLOATING, 720U, 480U,
           UMI_APPLICATION_PRESENTATION_EMPTY_CONNECT,
           1, 1, 0, 1),
