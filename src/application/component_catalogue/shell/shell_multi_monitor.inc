/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/shell/shell_multi_monitor.inc
 *
 * PURPOSE:
 *   Define the umicom.shell.multi-monitor reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.shell.multi-monitor", "shell", "Monitor Manager", "umicom.ui",
         "Persist windows, bounds, scale and monitor affinity.",
         UMI_APPLICATION_COMPONENT_SERVICE_SURFACE, UMI_CAPABILITY_IMPLEMENTED, 0, 1,
                 "desktop window launcher navigation"),
