/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/candidate_provenance.h
 * PURPOSE: Capture complete operation, agent, action and candidate provenance.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* A candidate can be reproduced only when its parent, source
 * revision, responsible agent and action are recorded together. */
#ifndef INCLUDE_UMICOM_HELIX_CANDIDATE_PROVENANCE_H
#define INCLUDE_UMICOM_HELIX_CANDIDATE_PROVENANCE_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/execution_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixCandidateProvenance {
    char operation_id[UMI_HELIX_ID_CAPACITY];
    char candidate_id[UMI_HELIX_ID_CAPACITY];
    char parent_candidate_id[UMI_HELIX_ID_CAPACITY];
    char agent_id[UMI_HELIX_ID_CAPACITY];
    char action_id[UMI_HELIX_ID_CAPACITY];
    char source_revision[UMI_HELIX_ID_CAPACITY];
    uint64_t content_hash;
} UmiHelixCandidateProvenance;

UmiStatus umi_helix_candidate_provenance_init(
    UmiHelixCandidateProvenance *provenance,
    const char *operation_id,
    const char *candidate_id,
    const char *parent_candidate_id,
    const char *agent_id,
    const char *action_id,
    const char *source_revision,
    const char *content);
uint64_t umi_helix_candidate_provenance_hash_text(const char *text);

#ifdef __cplusplus
}
#endif

#endif
