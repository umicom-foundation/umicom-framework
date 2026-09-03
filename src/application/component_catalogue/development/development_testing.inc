/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/development/development_testing.inc
 *
 * PURPOSE:
 *   Define the umicom.development.testing reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.development.testing", "development", "Test Explorer", "umicom.testing",
         "Discovery, hierarchy, runs, failures, coverage and artifacts.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "code build debug test project"),
