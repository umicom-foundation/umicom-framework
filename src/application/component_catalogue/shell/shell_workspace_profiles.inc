/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/shell/shell_workspace_profiles.inc
 *
 * PURPOSE:
 *   Define the umicom.shell.workspace-profiles reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.shell.workspace-profiles", "shell", "Workspace Profiles", "umicom.workspace",
         "Save, restore, import and export application-agnostic layouts.",
         UMI_APPLICATION_COMPONENT_INSPECTOR, UMI_CAPABILITY_IMPLEMENTED, 0, 1,
                 "desktop window launcher navigation"),
