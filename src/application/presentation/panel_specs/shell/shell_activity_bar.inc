/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/shell/shell_activity_bar.inc
 *
 * PURPOSE:
 *   Define the portable panel presentation contract for umicom.shell.activity-bar.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.shell.activity-bar", "umicom.icon.shell.activity-bar",
           UMI_APPLICATION_PRESENTATION_SURFACE_TOOL,
           UMI_APPLICATION_PRESENTATION_CHROME_COMPACT,
           UMI_APPLICATION_COMPONENT_REGION_LEFT, 64U, 480U,
           UMI_APPLICATION_PRESENTATION_EMPTY_SELECT,
           0, 0, 0, 1,
                 UMI_APPLICATION_PRESENTATION_COMPACT_AUTO_HIDE),
