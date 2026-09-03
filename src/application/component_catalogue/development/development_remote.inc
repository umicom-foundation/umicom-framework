/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/development/development_remote.inc
 *
 * PURPOSE:
 *   Register the shared remote-development surface while keeping transport
 *   credentials and connection providers outside application presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Applications render this contract and delegate actual remote connections to
 * permissioned adapters selected by the user. */
COMP("umicom.development.remote", "development", "Remote Development",
     "umicom.terminal.remote",
     "Remote hosts, workspaces, toolchains, tunnels and connection health.",
     UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_FOUNDATION, 0, 1,
                 "code build debug test project"),
