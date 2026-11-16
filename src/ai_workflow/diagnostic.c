/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workflow/diagnostic.c
 *
 * PURPOSE:
 *   Provide stable human-readable explanations for native workflow admission diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workflow/preflight.h"

const char *umi_ai_workflow_issue_text(UmiAiWorkflowIssue issue)
{
    switch (issue) {
    case UMI_AI_WORKFLOW_ISSUE_NONE: return "No issue";
    case UMI_AI_WORKFLOW_ISSUE_ARGUMENT: return "Required workflow storage or argument is missing";
    case UMI_AI_WORKFLOW_ISSUE_CAPACITY: return "Workflow count exceeds a supported bound";
    case UMI_AI_WORKFLOW_ISSUE_IDENTIFIER: return "Identifier is empty, unterminated or contains unsupported characters";
    case UMI_AI_WORKFLOW_ISSUE_DUPLICATE: return "Workflow identifier is repeated";
    case UMI_AI_WORKFLOW_ISSUE_VALUE_KIND: return "Port value kind is unknown";
    case UMI_AI_WORKFLOW_ISSUE_NODE_TYPE: return "Node references an unknown operation type";
    case UMI_AI_WORKFLOW_ISSUE_PORT: return "Link references a missing port";
    case UMI_AI_WORKFLOW_ISSUE_LINK_NODE: return "Link references a missing node";
    case UMI_AI_WORKFLOW_ISSUE_TYPE_MISMATCH: return "Connected port value kinds differ";
    case UMI_AI_WORKFLOW_ISSUE_BINDING: return "Input has conflicting bindings or an invalid supplied mask";
    case UMI_AI_WORKFLOW_ISSUE_REQUIRED_INPUT: return "Selected node has an unbound required input";
    case UMI_AI_WORKFLOW_ISSUE_CYCLE: return "Workflow contains a dependency cycle";
    case UMI_AI_WORKFLOW_ISSUE_OUTPUT: return "Selected output node is invalid or repeated";
    case UMI_AI_WORKFLOW_ISSUE_EFFECT_DEFINITION: return "Declared policy contains an unknown effect";
    case UMI_AI_WORKFLOW_ISSUE_EFFECT_DENIED: return "Selected operation requires an effect that is not allowed";
    case UMI_AI_WORKFLOW_ISSUE_ALLOCATION: return "Temporary planning storage could not be allocated";
    case UMI_AI_WORKFLOW_ISSUE_GRAPH: return "Canonical dependency planning refused the assembled graph";
    default: return "Unknown workflow diagnostic";
    }
}
