/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_qualification/test_inventory_identity.c
 * Purpose: Guard canonical inventory fingerprint compatibility and output preservation.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <string.h>
int main(void)
{
    Fixture text = {0};
    UmiReleaseInventory *inventory = NULL;
    REQUIRE(FixtureStart(&text));
    REQUIRE(FixtureSource(&text, "/repo/a.c", "library"));
    REQUIRE(FixtureParse(&text, &inventory) == UMI_STATUS_OK);
    char digest[65];
    REQUIRE(UmiReleaseInventoryFingerprint(inventory, digest) == UMI_STATUS_OK);
    /* Independently encoded vector: length-prefixed UTF-8 context, one source
     * record, big-endian integer fields. Keep this stable across API extraction. */
    REQUIRE(strcmp(digest, "df35fd007dccb986fec4fc3d79a9572a52e01ce8958d1c84527443df703b75e9") == 0);
    char preserved[65]; memcpy(preserved, digest, sizeof(preserved));
    REQUIRE(UmiReleaseInventoryFingerprint(NULL, digest) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(strcmp(digest, preserved) == 0);
    REQUIRE(UmiReleaseInventoryFingerprint(inventory, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiReleaseImpactInventoryFingerprint(NULL) == NULL);
    REQUIRE(UmiReleaseSourceObservationInventoryFingerprint(NULL) == NULL);
    UmiReleaseInventoryDestroy(inventory);
    FixtureDestroy(&text);
    QualificationFixture f;
    REQUIRE(QualificationStart(&f, 1U, false) == 0);
    REQUIRE(UmiReleaseInventoryFingerprint(f.inventory, digest) == UMI_STATUS_OK);
    REQUIRE(strcmp(digest, UmiReleaseImpactInventoryFingerprint(f.plan)) == 0);
    REQUIRE(strcmp(digest, UmiReleaseSourceObservationInventoryFingerprint(f.before)) == 0);
    QualificationDestroy(&f);
    return 0;
}
