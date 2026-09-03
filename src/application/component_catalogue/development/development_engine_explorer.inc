/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/development/development_engine_explorer.inc
 *
 * PURPOSE:
 *   Register a shared Engine Explorer that projects the authoritative
 *   Framework engine catalogue into development applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* The explorer reads one Framework catalogue instead of maintaining a second
 * application-owned list that could become stale or contradictory. */
COMP("umicom.development.engine-explorer", "development", "Engine Explorer",
     "umicom.engine",
     "Discover reusable engines, maturity and project requirements.",
     UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 0, 1,
                 "code build debug test project"),
