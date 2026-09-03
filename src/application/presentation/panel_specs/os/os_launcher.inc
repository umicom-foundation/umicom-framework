/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/os/os_launcher.inc
 *
 * PURPOSE:
 *   Define the portable panel presentation contract for umicom.os.launcher.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.os.launcher", "umicom.icon.os.launcher",
           UMI_APPLICATION_PRESENTATION_SURFACE_VIEW,
           UMI_APPLICATION_PRESENTATION_CHROME_STANDARD,
           UMI_APPLICATION_COMPONENT_REGION_LEFT, 360U, 280U,
           UMI_APPLICATION_PRESENTATION_EMPTY_CREATE,
           1, 1, 0, 1,
                 UMI_APPLICATION_PRESENTATION_COMPACT_AUTO_HIDE),
