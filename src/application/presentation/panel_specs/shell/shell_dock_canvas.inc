/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/shell/shell_dock_canvas.inc
 *
 * PURPOSE:
 *   Define the portable panel presentation contract for umicom.shell.dock-canvas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.shell.dock-canvas", "umicom.icon.shell.dock-canvas",
           UMI_APPLICATION_PRESENTATION_SURFACE_SERVICE,
           UMI_APPLICATION_PRESENTATION_CHROME_SERVICE,
           UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 800U, 600U,
           UMI_APPLICATION_PRESENTATION_EMPTY_CREATE,
           0, 0, 1, 1),
