/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/development/development_extensions.inc
 *
 * PURPOSE:
 *   Register a reusable extension-management surface backed by Framework
 *   package, trust, permission and plug-in lifecycle services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* The surface can request reviewed lifecycle operations but never loads an
 * untrusted extension merely because it is visible in a catalogue. */
COMP("umicom.development.extensions", "development", "Extensions",
     "umicom.plugin",
     "Discover, verify, install, update and quarantine extensions.",
     UMI_APPLICATION_COMPONENT_TOOL, UMI_CAPABILITY_IMPLEMENTED, 0, 0,
                 "code build debug test project"),
