/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_attestation_roundtrip.c
 * PURPOSE: Sign an actual four-file publication using an ephemeral Ed25519
 *          key, then validate signer-key binding and no-clobber semantics.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "training_attestation_fixture.h"

#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); \
    return 1; } } while (0)

int main(void)
{
    UmiAttestationFixture fixture;
    UmiAiTrainingPublicationEvidence inspected = {0};
    unsigned char record[UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES];
    FILE *file;
    size_t length;
    CHECK(UmiTestMakeFixture("roundtrip", &fixture));
    /* A detached signature cannot exist before the publication marker. */
    CHECK(UmiAiTrainingAttestationSignNewFile(fixture.base,
          fixture.privatePem) == UMI_STATUS_IO_ERROR);
    CHECK(UmiAiTrainingPublicationSaveNewFile(fixture.base) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationVerifyFile(fixture.base,
          fixture.trustedPem) == UMI_STATUS_IO_ERROR);
    CHECK(UmiAiTrainingAttestationSignNewFile(fixture.base,
          fixture.privatePem) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationVerifyFile(fixture.base,
          fixture.trustedPem) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAttestationVerifyAndInspectFile(fixture.base,
          fixture.trustedPem, &inspected) == UMI_STATUS_OK);
    CHECK(inspected.checkpoint.byteCount == 17U);
    CHECK(strlen(inspected.sourceLock.sha256) == 64U);
    CHECK(UmiAiTrainingAttestationVerifyFile(fixture.base,
          fixture.wrongPem) != UMI_STATUS_OK);
    /* The file is deterministic for the same key and publication. A second
     * sign must not replace an existing signature or alter its bytes. */
    file = fopen(fixture.signature, "rb");
    CHECK(file != NULL);
    length = fread(record, 1U, sizeof(record), file);
    CHECK(fclose(file) == 0);
    CHECK(length == sizeof(record));
    CHECK(UmiAiTrainingAttestationSignNewFile(fixture.base,
          fixture.privatePem) == UMI_STATUS_ALREADY_EXISTS);
    file = fopen(fixture.signature, "rb");
    CHECK(file != NULL);
    {
        unsigned char again[UMI_AI_TRAINING_ATTESTATION_RECORD_BYTES];
        CHECK(fread(again, 1U, sizeof(again), file) == sizeof(again));
        CHECK(memcmp(again, record, sizeof(record)) == 0);
    }
    CHECK(fclose(file) == 0);
    UmiTestDeleteFixture(&fixture);
    puts("Native Ed25519 publication: sign, trusted-key verification and no-clobber PASS");
    return 0;
}
