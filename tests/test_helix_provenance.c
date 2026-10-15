/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_provenance.c
 * PURPOSE: Verify reproducible Helix v2 provenance hashes.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "value_archive/construction_cases.h"
#include "umicom/helix/candidate_provenance.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted candidate provenance value. */
static int UmiHelixCandidateProvenanceConstructionCases(void)
{
    UmiHelixCandidateProvenance *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, "op", "candidate", "parent", "helix.v2.builder", "build", "abc123", "complete source") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->operation_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, oversized, "candidate", "parent", "helix.v2.builder", "build", "abc123", "complete source") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, value->operation_id, "candidate", "parent", "helix.v2.builder", "build", "abc123", "complete source") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->operation_id, "op") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->candidate_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, "op", oversized, "parent", "helix.v2.builder", "build", "abc123", "complete source") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, "op", value->candidate_id, "parent", "helix.v2.builder", "build", "abc123", "complete source") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->candidate_id, "candidate") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->parent_candidate_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, "op", "candidate", oversized, "helix.v2.builder", "build", "abc123", "complete source") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, "op", "candidate", value->parent_candidate_id, "helix.v2.builder", "build", "abc123", "complete source") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->parent_candidate_id, "parent") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->agent_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, "op", "candidate", "parent", oversized, "build", "abc123", "complete source") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, "op", "candidate", "parent", value->agent_id, "build", "abc123", "complete source") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->agent_id, "helix.v2.builder") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->action_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, "op", "candidate", "parent", "helix.v2.builder", oversized, "abc123", "complete source") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, "op", "candidate", "parent", "helix.v2.builder", value->action_id, "abc123", "complete source") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->action_id, "build") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->source_revision) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, "op", "candidate", "parent", "helix.v2.builder", "build", oversized, "complete source") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_candidate_provenance_init(value, "op", "candidate", "parent", "helix.v2.builder", "build", value->source_revision, "complete source") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->source_revision, "abc123") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiHelixCandidateProvenanceConstructionCases() != 0) return 1;

    UmiHelixCandidateProvenance provenance;
    assert(umi_helix_candidate_provenance_init(&provenance, "op", "candidate", "parent",
        "helix.v2.builder", "build", "abc123", "complete source")
        == UMI_STATUS_OK);
    assert(provenance.content_hash ==
           umi_helix_candidate_provenance_hash_text("complete source"));
    assert(provenance.content_hash !=
           umi_helix_candidate_provenance_hash_text("changed source"));
    return 0;
}
