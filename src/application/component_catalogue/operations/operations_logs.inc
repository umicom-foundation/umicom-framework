/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/operations/operations_logs.inc
 *
 * PURPOSE:
 *   Define the umicom.operations.logs reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.operations.logs", "operations", "Log Explorer", "umicom.tracing",
         "Correlated diagnostics, events, traces and redaction.", UMI_APPLICATION_COMPONENT_VIEW,
         UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "workflow task monitoring"),
