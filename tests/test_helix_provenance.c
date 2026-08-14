/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_provenance.c
 * PURPOSE: Verify reproducible Helix v2 provenance hashes.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/helix/candidate_provenance.h"
int main(void)
{
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
