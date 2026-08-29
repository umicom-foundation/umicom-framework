/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/governance/shared_audit.inc
 *
 * PURPOSE:
 *   Define the umicom.shared.audit reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.shared.audit", "governance", "Audit Trail", "umicom.audit",
         "Immutable user, service, decision and business-operation evidence.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1),
