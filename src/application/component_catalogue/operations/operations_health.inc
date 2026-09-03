/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/operations/operations_health.inc
 *
 * PURPOSE:
 *   Define the umicom.operations.health reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.operations.health", "operations", "Health Dashboard",
         "umicom.observability.readiness", "Readiness, dependencies, incidents and recovery state.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "workflow task monitoring"),
