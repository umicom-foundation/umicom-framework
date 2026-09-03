/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/treasury/treasury_workflow.inc
 *
 * PURPOSE:
 *   Define the umicom.treasury.workflow reusable component record in the versioned
 *   Framework application-component catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
COMP("umicom.treasury.workflow", "treasury", "Workflow Inbox", "umicom.workflow",
         "Tasks, approvals, exceptions and four-eyes controls.", UMI_APPLICATION_COMPONENT_VIEW,
         UMI_CAPABILITY_FOUNDATION, 1, 1,
                 "cash liquidity settlement payments"),
