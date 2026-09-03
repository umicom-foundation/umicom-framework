/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_specs/shell/shell_workspace_profiles.inc
 *
 * PURPOSE:
 *   Define the portable panel presentation contract for umicom.shell.workspace-profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
PANEL_SPEC("umicom.shell.workspace-profiles", "umicom.icon.shell.workspace-profiles",
           UMI_APPLICATION_PRESENTATION_SURFACE_INSPECTOR,
           UMI_APPLICATION_PRESENTATION_CHROME_INSPECTOR,
           UMI_APPLICATION_COMPONENT_REGION_LEFT, 320U, 360U,
           UMI_APPLICATION_PRESENTATION_EMPTY_SELECT,
           1, 1, 0, 1,
                 UMI_APPLICATION_PRESENTATION_COMPACT_AUTO_HIDE),
