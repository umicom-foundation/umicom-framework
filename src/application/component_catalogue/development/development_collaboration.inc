/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component_catalogue/development/development_collaboration.inc
 *
 * PURPOSE:
 *   Register a collaboration surface for presence, comments and shared review
 *   context with explicit permission boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Shared visibility does not imply permission to edit files, run commands or
 * approve automated work; those decisions remain separate governed actions. */
COMP("umicom.development.collaboration", "development", "Collaboration",
     "umicom.workbench.collaboration",
     "Presence, shared review context, comments and session permissions.",
     UMI_APPLICATION_COMPONENT_VIEW, UMI_CAPABILITY_FOUNDATION, 0, 1),
