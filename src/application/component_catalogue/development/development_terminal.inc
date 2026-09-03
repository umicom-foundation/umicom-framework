/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/development/development_terminal.inc
 *
 * PURPOSE:
 *   Define the umicom.development.terminal reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.development.terminal", "development", "Terminal", "umicom.process",
         "Supervised terminal sessions and task output.", UMI_APPLICATION_COMPONENT_TOOL,
         UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "code build debug test project"),
