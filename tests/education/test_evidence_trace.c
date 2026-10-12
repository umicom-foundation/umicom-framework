/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_evidence_trace.c
 * PURPOSE: Check a sealed teaching trace, failure-atomic refusal, spaced paths,
 *          stale driver generations and deterministic damaged input.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/evidence_trace.h"
#include "umicom/base/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COUNT 20U
#define TOTAL (UMI_EDUCATION_EVIDENCE_HEADER_BYTES + \
               COUNT * UMI_EDUCATION_EVIDENCE_RECORD_BYTES)
#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

static void Write16(unsigned char *p, uint16_t value)
{
    p[0] = (unsigned char)value;
    p[1] = (unsigned char)(value >> 8U);
}
static void Write32(unsigned char *p, uint32_t value)
{
    for (unsigned i = 0U; i < 4U; ++i) p[i] = (unsigned char)(value >> (8U * i));
}
static void Write64(unsigned char *p, uint64_t value)
{
    for (unsigned i = 0U; i < 8U; ++i) p[i] = (unsigned char)(value >> (8U * i));
}
static unsigned char *Record(unsigned char *buffer, size_t index)
{
    return buffer + UMI_EDUCATION_EVIDENCE_HEADER_BYTES +
           index * UMI_EDUCATION_EVIDENCE_RECORD_BYTES;
}
static void Add(unsigned char *buffer, unsigned ordinal, unsigned kind,
                unsigned severity, unsigned source, uint32_t owner, uint32_t generation,
                uint32_t code, uint64_t evidenceId, uint64_t checkpoint)
{
    unsigned char *p = Record(buffer, (size_t)ordinal - 1U);
    Write32(p, ordinal);
    p[4U] = (unsigned char)kind;
    p[5U] = (unsigned char)severity;
    Write32(p + 8U, source);
    Write32(p + 12U, owner);
    Write32(p + 16U, generation);
    Write32(p + 20U, code);
    Write64(p + 24U, ordinal);
    Write64(p + 32U, evidenceId);
    Write64(p + 40U, checkpoint);
}

/* This producer is a test fixture only. The public CLI has no authoring or
 * signing operation; it can read arbitrary untrusted files without executing
 * them. Sealing after semantic mutations exercises the state machine independently
 * from the digest mismatch detector. */
static int Seal(unsigned char *buffer)
{
    unsigned char previous[UMI_EDUCATION_EVIDENCE_HASH_BYTES] = {0};
    for (size_t i = 0U; i < COUNT; ++i) {
        unsigned char *record = Record(buffer, i);
        unsigned char digest[UMI_EDUCATION_EVIDENCE_HASH_BYTES];
        UmiSha256 hash;
        UmiSha256Init(&hash);
        if (UmiSha256Update(&hash, previous, sizeof(previous)) != UMI_STATUS_OK ||
            UmiSha256Update(&hash, record, UMI_EDUCATION_EVIDENCE_METADATA_BYTES) != UMI_STATUS_OK ||
            UmiSha256Final(&hash, digest) != UMI_STATUS_OK)
            return 0;
        memcpy(record + UMI_EDUCATION_EVIDENCE_METADATA_BYTES, digest, sizeof(digest));
        memcpy(previous, digest, sizeof(digest));
    }
    return 1;
}
static int Create(unsigned char *buffer)
{
    const unsigned char magic[8] = {'U','M','I','E','V','D','1',0};
    memset(buffer, 0, TOTAL);
    memcpy(buffer, magic, sizeof(magic));
    Write16(buffer + 8U, 1U);
    Write32(buffer + 12U, COUNT);
    Add(buffer,  1U, 1U, 0U, 1U, 11U, 1U, 0U, 0U, 0U);
    Add(buffer,  2U, 2U, 0U, 1U, 11U, 1U, 0U, 0U, 0U);
    Add(buffer,  3U, 3U, 0U, 1U, 11U, 1U, 21U, 1U, 0U);
    Add(buffer,  4U, 4U, 0U, 1U, 11U, 1U, 0U, 0U, 1U);
    Add(buffer,  5U, 1U, 0U, 2U, 33U, 1U, 0U, 0U, 0U);
    Add(buffer,  6U, 2U, 0U, 2U, 33U, 1U, 0U, 0U, 0U);
    Add(buffer,  7U, 5U, 2U, 1U, 11U, 1U, 35U, 2U, 0U);
    Add(buffer,  8U, 3U, 1U, 1U, 11U, 1U, 42U, 3U, 0U);
    Add(buffer,  9U, 6U, 0U, 1U, 11U, 1U, 0U, 0U, 1U);
    Add(buffer, 10U, 7U, 0U, 1U, 11U, 1U, 0U, 0U, 0U);
    Add(buffer, 11U, 8U, 0U, 1U, 11U, 1U, 0U, 0U, 0U);
    Add(buffer, 12U, 9U, 0U, 1U, 11U, 1U, 0U, 0U, 0U);
    Add(buffer, 13U, 5U, 2U, 2U, 33U, 1U, 7U, 4U, 0U);
    Add(buffer, 14U, 8U, 0U, 2U, 33U, 1U, 0U, 0U, 0U);
    Add(buffer, 15U, 9U, 0U, 2U, 33U, 1U, 0U, 0U, 0U);
    Add(buffer, 16U, 1U, 0U, 1U, 22U, 2U, 0U, 0U, 0U);
    Add(buffer, 17U, 2U, 0U, 1U, 22U, 2U, 0U, 0U, 0U);
    Add(buffer, 18U, 7U, 0U, 1U, 22U, 2U, 0U, 0U, 0U);
    Add(buffer, 19U, 8U, 0U, 1U, 22U, 2U, 0U, 0U, 0U);
    Add(buffer, 20U, 9U, 0U, 1U, 22U, 2U, 0U, 0U, 0U);
    return Seal(buffer);
}
static int CheckRejected(const unsigned char *buffer, size_t length)
{
    UmiEducationEvidenceReport unchanged;
    UmiEducationEvidenceReport copy;
    memset(&unchanged, 0xa5, sizeof(unchanged));
    memcpy(&copy, &unchanged, sizeof(copy));
    const UmiStatus status = UmiEducationEvidenceTraceInspect(buffer, length, &unchanged);
    return status != UMI_STATUS_OK && memcmp(&unchanged, &copy, sizeof(copy)) == 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2 && argv[1] != NULL);
    const char *fixture = argv[1];
    unsigned char valid[TOTAL], changed[TOTAL];
    CHECK(Create(valid));
    UmiEducationEvidenceReport report = {0};
    CHECK(UmiEducationEvidenceTraceInspect(valid, sizeof(valid), &report) == UMI_STATUS_OK);
    CHECK(report.recordCount == 20U && report.attached == 3U && report.ready == 3U);
    CHECK(report.observations == 2U && report.checkpoints == 1U);
    CHECK(report.faults == 2U && report.recoveries == 1U && report.quiesced == 2U);
    CHECK(report.stopped == 3U && report.released == 3U);
    CHECK(report.peakLive == 2U && report.liveAtEnd == 0U);
    CHECK(report.faultedAtEnd == 0U && report.stoppedAtEnd == 0U);
    CHECK(report.lastTick == 20U && report.lastObservationId == 4U);
    unsigned char zero[UMI_EDUCATION_EVIDENCE_HASH_BYTES] = {0};
    CHECK(memcmp(report.lastDigest, zero, sizeof(zero)) != 0);

    char hash[UMI_SHA256_HEX_CAPACITY];
    CHECK(UmiSha256Buffer("abc", 3U, hash) == UMI_STATUS_OK);
    CHECK(strcmp(hash,
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") == 0);

    /* The CLI fixture is a real file with spaces in its path. */
    FILE *stream = fopen(fixture, "wb");
    CHECK(stream != NULL);
    CHECK(fwrite(valid, 1U, sizeof(valid), stream) == sizeof(valid));
    CHECK(fclose(stream) == 0);
    UmiEducationEvidenceReport fromFile = {0};
    CHECK(UmiEducationEvidenceTraceInspectFile(fixture, &fromFile) == UMI_STATUS_OK);
    CHECK(memcmp(&fromFile, &report, sizeof(report)) == 0);
    CHECK(CheckRejected(valid, sizeof(valid) - 1U));
    CHECK(CheckRejected(valid, UMI_EDUCATION_EVIDENCE_HEADER_BYTES - 1U));
    CHECK(UmiEducationEvidenceTraceInspectFile("not-found-evidence-fixture-404", &fromFile)
          == UMI_STATUS_NOT_FOUND);

    memcpy(changed, valid, sizeof(changed));
    changed[16U] = 1U; /* Unsupported header flags. */
    CHECK(CheckRejected(changed, sizeof(changed)));
    memcpy(changed, valid, sizeof(changed));
    changed[12U] = 0xffU; changed[13U] = 0xffU; /* Count beyond bound. */
    CHECK(CheckRejected(changed, sizeof(changed)));
    memcpy(changed, valid, sizeof(changed));
    changed[UMI_EDUCATION_EVIDENCE_HEADER_BYTES + 6U] = 1U; /* Reserved byte. */
    CHECK(Seal(changed) && CheckRejected(changed, sizeof(changed)));
    memcpy(changed, valid, sizeof(changed));
    Write32(Record(changed, 15U) + 16U, 1U); /* Stale reuse: must use generation 2. */
    CHECK(Seal(changed) && CheckRejected(changed, sizeof(changed)));
    memcpy(changed, valid, sizeof(changed));
    Write64(Record(changed, 8U) + 40U, 2U); /* Recovery references wrong checkpoint. */
    CHECK(Seal(changed) && CheckRejected(changed, sizeof(changed)));
    memcpy(changed, valid, sizeof(changed));
    Write32(Record(changed, 8U) + 12U, 55U); /* Wrong owner. */
    CHECK(Seal(changed) && CheckRejected(changed, sizeof(changed)));
    memcpy(changed, valid, sizeof(changed));
    Write64(Record(changed, 3U) + 40U, 12U); /* Invented checkpoint. */
    CHECK(Seal(changed) && CheckRejected(changed, sizeof(changed)));
    memcpy(changed, valid, sizeof(changed));
    Write64(Record(changed, 12U) + 32U, 3U); /* Reused observation identifier. */
    CHECK(Seal(changed) && CheckRejected(changed, sizeof(changed)));
    memcpy(changed, valid, sizeof(changed));
    Record(changed, 9U)[4U] = 9U; /* Release before stop. */
    CHECK(Seal(changed) && CheckRejected(changed, sizeof(changed)));

    /* The integrity chain detects ANY one-byte change to the already sealed
     * metadata or digest; exercise many repeatable byte flips. */
    uint32_t rng = UINT32_C(0x18ab34ef);
    for (unsigned attempt = 0U; attempt < 10000U; ++attempt) {
        memcpy(changed, valid, sizeof(changed));
        rng = rng * UINT32_C(1664525) + UINT32_C(1013904223);
        size_t position = (size_t)(rng % (uint32_t)sizeof(changed));
        changed[position] ^= (unsigned char)(1U << ((rng >> 16U) & 7U));
        CHECK(CheckRejected(changed, sizeof(changed)));
    }
    puts("[OK] Evidence format, chained digest, driver stages, stale generations, 10000 corruptions.");
    return 0;
}
