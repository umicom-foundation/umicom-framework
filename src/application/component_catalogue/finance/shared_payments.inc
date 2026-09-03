/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/finance/shared_payments.inc
 *
 * PURPOSE:
 *   Define the umicom.shared.payments reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.shared.payments", "finance", "Payments Hub", "umicom.payments",
         "Payment initiation, routing, status, exceptions and investigation.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "banking accounting payments risk"),
