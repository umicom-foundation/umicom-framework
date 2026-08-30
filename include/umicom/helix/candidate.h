/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/candidate.h
 *
 * PURPOSE:
 *   Represent an isolated Helix software candidate and lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Candidates are isolated workspaces rather than in-place production mutations, which makes validation and rollback practical.
 */

#ifndef INCLUDE_UMICOM_HELIX_CANDIDATE_H
#define INCLUDE_UMICOM_HELIX_CANDIDATE_H

#include <stdint.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixCandidate {
    char candidate_id[UMI_HELIX_ID_CAPACITY];
    char plan_id[UMI_HELIX_ID_CAPACITY];
    char workspace[UMI_HELIX_TEXT_CAPACITY];
    UmiHelixCandidateState state;
    uint64_t created_at_ns;
} UmiHelixCandidate;

UmiStatus umi_helix_candidate_init(UmiHelixCandidate *value);

#ifdef __cplusplus
}
#endif

#endif
