/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/types.c
 *
 * PURPOSE:
 *   Translate Helix enum values into stable text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Stable text values are useful in journals, command-line tools and Studio views without exposing internal enum numbers to users.
 */

#include "umicom/helix/types.h"

const char *umi_helix_agent_kind_text(UmiHelixAgentKind kind)
{
    switch (kind) {
        case UMI_HELIX_AGENT_DESIGNER: return "designer";
        case UMI_HELIX_AGENT_BUILDER: return "builder";
        case UMI_HELIX_AGENT_SUGGESTION: return "suggestion";
        case UMI_HELIX_AGENT_DIAGNOSTIC: return "diagnostic";
        case UMI_HELIX_AGENT_SECURITY: return "security";
        case UMI_HELIX_AGENT_RELEASE: return "release";
        default: return "unknown";
    }
}

const char *umi_helix_candidate_state_text(UmiHelixCandidateState state)
{
    switch (state) {
        case UMI_HELIX_CANDIDATE_DRAFT: return "draft";
        case UMI_HELIX_CANDIDATE_BUILT: return "built";
        case UMI_HELIX_CANDIDATE_VALIDATED: return "validated";
        case UMI_HELIX_CANDIDATE_APPROVED: return "approved";
        case UMI_HELIX_CANDIDATE_REJECTED: return "rejected";
        case UMI_HELIX_CANDIDATE_PROMOTED: return "promoted";
        default: return "unknown";
    }
}
