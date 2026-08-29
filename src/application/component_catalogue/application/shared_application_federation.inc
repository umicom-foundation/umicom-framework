/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/application/shared_application_federation.inc
 *
 * PURPOSE:
 *   Define the umicom.shared.application-federation reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.shared.application-federation", "application", "Application Federation",
         "umicom.application.federation",
         "Cross-application discovery, selection, context and lifecycle projection.",
         UMI_APPLICATION_COMPONENT_SERVICE_SURFACE, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
