/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/types.h
 *
 * PURPOSE:
 *   Define shared Helix identifiers, lifecycle states, agent kinds and capacities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Helix uses explicit typed states so an autonomous workflow cannot silently jump from a draft change to a production generation.
 */

#ifndef INCLUDE_UMICOM_HELIX_TYPES_H
#define INCLUDE_UMICOM_HELIX_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_HELIX_ID_CAPACITY 128U
#define UMI_HELIX_TEXT_CAPACITY 1024U
#define UMI_HELIX_MAX_EVIDENCE 64U
#define UMI_HELIX_MAX_AGENTS 32U
#define UMI_HELIX_MAX_CANDIDATES 16U

typedef enum UmiHelixAgentKind {
    UMI_HELIX_AGENT_DESIGNER = 1,
    UMI_HELIX_AGENT_BUILDER = 2,
    UMI_HELIX_AGENT_SUGGESTION = 3,
    UMI_HELIX_AGENT_DIAGNOSTIC = 4,
    UMI_HELIX_AGENT_SECURITY = 5,
    UMI_HELIX_AGENT_RELEASE = 6
} UmiHelixAgentKind;

typedef enum UmiHelixCandidateState {
    UMI_HELIX_CANDIDATE_DRAFT = 1,
    UMI_HELIX_CANDIDATE_BUILT = 2,
    UMI_HELIX_CANDIDATE_VALIDATED = 3,
    UMI_HELIX_CANDIDATE_APPROVED = 4,
    UMI_HELIX_CANDIDATE_REJECTED = 5,
    UMI_HELIX_CANDIDATE_PROMOTED = 6
} UmiHelixCandidateState;

const char *umi_helix_agent_kind_text(UmiHelixAgentKind kind);
const char *umi_helix_candidate_state_text(UmiHelixCandidateState state);

#ifdef __cplusplus
}
#endif

#endif
