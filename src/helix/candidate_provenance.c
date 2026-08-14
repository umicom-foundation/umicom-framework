/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/candidate_provenance.c
 * PURPOSE: Implement deterministic Helix v2 provenance records.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/candidate_provenance.h"

#include <stdio.h>
#include <string.h>

static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

uint64_t umi_helix_candidate_provenance_hash_text(const char *text)
{
    uint64_t value = UINT64_C(1469598103934665603);
    if (text == NULL) return 0U;
    while (*text != '\0') {
        value ^= (uint64_t)(unsigned char)*text++;
        value *= UINT64_C(1099511628211);
    }
    return value;
}

UmiStatus umi_helix_candidate_provenance_init(
    UmiHelixCandidateProvenance *provenance,
    const char *operation_id,
    const char *candidate_id,
    const char *parent_candidate_id,
    const char *agent_id,
    const char *action_id,
    const char *source_revision,
    const char *content)
{
    if (provenance == NULL || operation_id == NULL || candidate_id == NULL ||
        parent_candidate_id == NULL || agent_id == NULL || action_id == NULL ||
        source_revision == NULL || content == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(provenance, 0, sizeof(*provenance));
    if (!copy_text(provenance->operation_id,
                   sizeof(provenance->operation_id), operation_id) ||
        !copy_text(provenance->candidate_id,
                   sizeof(provenance->candidate_id), candidate_id) ||
        !copy_text(provenance->parent_candidate_id,
                   sizeof(provenance->parent_candidate_id), parent_candidate_id) ||
        !copy_text(provenance->agent_id, sizeof(provenance->agent_id), agent_id) ||
        !copy_text(provenance->action_id, sizeof(provenance->action_id), action_id) ||
        !copy_text(provenance->source_revision,
                   sizeof(provenance->source_revision), source_revision)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    provenance->content_hash = umi_helix_candidate_provenance_hash_text(content);
    return UMI_STATUS_OK;
}
