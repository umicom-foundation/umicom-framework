/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/kitchen/kitchen_planner.inc
 *
 * PURPOSE:
 *   Define the umicom.kitchen.planner reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.kitchen.planner", "kitchen", "Kitchen Planner", "umicom.kitchen",
         "Room capture, parametric placement and rule validation.",
         UMI_APPLICATION_COMPONENT_EDITOR, UMI_CAPABILITY_PLANNED, 1, 1,
                 "room cabinet appliance planner"),
