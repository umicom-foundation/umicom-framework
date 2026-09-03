/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/workflow/shared_tasks.inc
 *
 * PURPOSE:
 *   Define the umicom.shared.tasks reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.shared.tasks", "workflow", "Task Centre", "umicom.tasks",
         "Assignments, queues, priorities, deadlines and completion evidence.",
         UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_IMPLEMENTED, 1, 1,
                 "workflow process automation tasks"),
