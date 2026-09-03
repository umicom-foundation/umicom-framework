/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/integration/shared_messaging.inc
 *
 * PURPOSE:
 *   Define the umicom.shared.messaging reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.shared.messaging", "integration", "Messaging Centre", "umicom.messaging",
         "Queues, topics, subscriptions, deliveries and dead letters.",
         UMI_APPLICATION_COMPONENT_DASHBOARD, UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "messaging workflow connector"),
