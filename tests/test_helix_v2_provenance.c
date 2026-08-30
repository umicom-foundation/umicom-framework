/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_v2_provenance.c
 * PURPOSE: Verify reproducible Helix v2 provenance hashes.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/helix/provenance_v2.h"
int main(void)
{
    UmiHelixProvenanceV2 provenance;
    assert(umi_helix_provenance_v2_init(&provenance, "op", "candidate", "parent",
        "helix.v2.builder", "build", "abc123", "complete source")
        == UMI_STATUS_OK);
    assert(provenance.content_hash ==
           umi_helix_provenance_v2_hash_text("complete source"));
    assert(provenance.content_hash !=
           umi_helix_provenance_v2_hash_text("changed source"));
    return 0;
}
