/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/operations/shared_operations.inc
 *
 * PURPOSE:
 *   Define the umicom.shared.operations reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.shared.operations", "operations", "Operations Centre", "umicom.operations",
         "Operational queues, controls, incidents and recovery actions.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "workflow task monitoring"),
