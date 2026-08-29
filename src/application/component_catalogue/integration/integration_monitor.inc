/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/integration/integration_monitor.inc
 *
 * PURPOSE:
 *   Define the umicom.integration.monitor reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.integration.monitor", "integration", "Integration Monitor", "umicom.integration",
         "Throughput, failures, replay, dead letters and lineage.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
