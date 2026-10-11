/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_elf_load_plan.c
 * PURPOSE:
 *   Validate ELF32/ELF64 PT_LOAD descriptions, file-size boundaries,
 *   atomic failure reports, malformed metadata and a path containing spaces.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/elf_load_plan.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; \
} } while (0)

static void Store(unsigned char *bytes, size_t at, uint64_t value,
                  unsigned size, int little)
{
    for (unsigned i = 0U; i < size; ++i) {
        const unsigned shift = (little ? i : size - i - 1U) * 8U;
        bytes[at + i] = (unsigned char)(value >> shift);
    }
}

static void Fixture64(unsigned char bytes[1024])
{
    memset(bytes, 0, 1024U);
    memcpy(bytes, "\177ELF", 4U);
    bytes[4] = 2U; bytes[5] = 1U; bytes[6] = 1U;
    Store(bytes, 16U, 2U, 2U, 1);             /* ET_EXEC */
    Store(bytes, 18U, 243U, 2U, 1);           /* RISC-V */
    Store(bytes, 20U, 1U, 4U, 1);             /* EV_CURRENT */
    Store(bytes, 24U, 0x80200010U, 8U, 1);   /* _start */
    Store(bytes, 32U, 64U, 8U, 1);            /* program table offset */
    Store(bytes, 52U, 64U, 2U, 1);            /* ELF header size */
    Store(bytes, 54U, 56U, 2U, 1);            /* programme-header entry size */
    Store(bytes, 56U, 2U, 2U, 1);             /* two entries */
    /* Executable code: 128 file bytes, 512 mapped bytes; 384 zero-fill. */
    Store(bytes, 64U, 1U, 4U, 1);
    Store(bytes, 68U, 5U, 4U, 1);             /* READ | EXECUTE */
    Store(bytes, 72U, 0x200U, 8U, 1);         /* file offset */
    Store(bytes, 80U, 0x80200000U, 8U, 1);    /* VMA */
    Store(bytes, 88U, 0x80200000U, 8U, 1);    /* physical hint */
    Store(bytes, 96U, 0x80U, 8U, 1);
    Store(bytes, 104U, 0x200U, 8U, 1);
    Store(bytes, 112U, 0x100U, 8U, 1);
    /* Read/write data: 32 bytes on disk, 256 in memory; 224 zero-fill. */
    Store(bytes, 120U, 1U, 4U, 1);
    Store(bytes, 124U, 6U, 4U, 1);
    Store(bytes, 128U, 0x300U, 8U, 1);
    Store(bytes, 136U, 0x80202000U, 8U, 1);
    Store(bytes, 144U, 0x80202000U, 8U, 1);
    Store(bytes, 152U, 0x20U, 8U, 1);
    Store(bytes, 160U, 0x100U, 8U, 1);
    Store(bytes, 168U, 0x100U, 8U, 1);
}

static void Fixture32BE(unsigned char bytes[384])
{
    memset(bytes, 0, 384U);
    memcpy(bytes, "\177ELF", 4U);
    bytes[4] = 1U; bytes[5] = 2U; bytes[6] = 1U;
    Store(bytes, 16U, 3U, 2U, 0);             /* ET_DYN */
    Store(bytes, 18U, 3U, 2U, 0);             /* i386 */
    Store(bytes, 20U, 1U, 4U, 0);
    Store(bytes, 24U, 0x08048010U, 4U, 0);
    Store(bytes, 28U, 52U, 4U, 0);
    Store(bytes, 40U, 52U, 2U, 0);
    Store(bytes, 42U, 32U, 2U, 0);
    Store(bytes, 44U, 1U, 2U, 0);
    Store(bytes, 52U, 1U, 4U, 0);
    Store(bytes, 56U, 0x100U, 4U, 0);
    Store(bytes, 60U, 0x08048000U, 4U, 0);
    Store(bytes, 64U, 0x08048000U, 4U, 0);
    Store(bytes, 68U, 64U, 4U, 0);
    Store(bytes, 72U, 128U, 4U, 0);
    Store(bytes, 76U, 5U, 4U, 0);
    Store(bytes, 80U, 256U, 4U, 0);
}

static int ExpectFailure(const unsigned char *bytes, size_t length,
                         uint64_t complete, UmiStatus expected)
{
    UmiEducationElfLoadPlan out;
    memset(&out, 0xa5, sizeof out);
    const UmiEducationElfLoadPlan previous = out;
    const UmiStatus status = UmiEducationElfLoadPlanInspectBytes(bytes, length, complete, &out);
    return status == expected && memcmp(&out, &previous, sizeof out) == 0;
}

static int TestValid(void)
{
    unsigned char elf64[1024];
    unsigned char elf32[384];
    Fixture64(elf64);
    Fixture32BE(elf32);
    UmiEducationElfLoadPlan plan = {0};
    CHECK(UmiEducationElfLoadPlanInspectBytes(elf64, sizeof elf64,
        sizeof elf64, &plan) == UMI_STATUS_OK);
    CHECK(plan.addressBits == 64U && plan.littleEndian == 1U && plan.machine == 243U);
    CHECK(plan.segmentCount == 2U && plan.programHeaderCount == 2U);
    CHECK(plan.entryAddress == UINT64_C(0x80200010) && plan.entryInExecutableSegment == 1U);
    CHECK(plan.fileBackedBytes == 160U && plan.zeroFillBytes == 608U);
    CHECK(plan.minimumVirtualAddress == 0x80200000U);
    CHECK(plan.maximumVirtualAddressExclusive == 0x80202100U);
    CHECK(plan.segments[0].flags == 5U && plan.segments[1].flags == 6U);
    CHECK(UmiEducationElfLoadPlanInspectBytes(elf32, sizeof elf32,
        sizeof elf32, &plan) == UMI_STATUS_OK);
    CHECK(plan.addressBits == 32U && plan.littleEndian == 0U && plan.machine == 3U);
    CHECK(plan.segmentCount == 1U && plan.zeroFillBytes == 64U);
    /* A complete programme-header prefix is sufficient to inspect a large
     * file: the content bytes are not read or loaded into the process. */
    CHECK(UmiEducationElfLoadPlanInspectBytes(elf64, 176U, 1024U, &plan) == UMI_STATUS_OK);
    return 0;
}

static int TestRefusal(void)
{
    unsigned char bytes[1024];
    Fixture64(bytes);
    CHECK(ExpectFailure((const unsigned char *)"MZ", 2U, 2U,
        UMI_STATUS_NOT_IMPLEMENTED));
    CHECK(ExpectFailure(bytes, 16U, 1024U, UMI_STATUS_PARSE_ERROR));
    CHECK(ExpectFailure(bytes, 64U, 1024U, UMI_STATUS_PARSE_ERROR));
    CHECK(ExpectFailure(bytes, 1024U, 64U, UMI_STATUS_PARSE_ERROR));
    for (size_t n = 4U; n < 176U; ++n)
        CHECK(ExpectFailure(bytes, n, sizeof bytes, UMI_STATUS_PARSE_ERROR));
    unsigned char bad[1024];
#define CASE(at,value,width,expected) do { \
    memcpy(bad, bytes, sizeof bad); Store(bad, at, value, width, 1); \
    CHECK(ExpectFailure(bad, sizeof bad, sizeof bad, expected)); \
} while (0)
    CASE(16U, 1U, 2U, UMI_STATUS_NOT_IMPLEMENTED);  /* relocatable object */
    CASE(32U, UINT64_MAX, 8U, UMI_STATUS_PARSE_ERROR); /* table offset overflow */
    CASE(54U, 0U, 2U, UMI_STATUS_PARSE_ERROR);     /* no ph entry bytes */
    CASE(56U, 0U, 2U, UMI_STATUS_PARSE_ERROR);     /* no headers */
    CASE(56U, 65535U, 2U, UMI_STATUS_NOT_IMPLEMENTED); /* PN_XNUM */
    CASE(56U, 4097U, 2U, UMI_STATUS_CAPACITY_EXCEEDED);
    CASE(24U, 0x80204000U, 8U, UMI_STATUS_PARSE_ERROR); /* entry not mapped */
    CASE(104U, 0x7fU, 8U, UMI_STATUS_PARSE_ERROR); /* file bigger than 127 memory bytes */
#undef CASE
    memcpy(bad, bytes, sizeof bad);
    Store(bad, 96U, 0x400U, 8U, 1); /* fileBytes > memBytes */
    CHECK(ExpectFailure(bad, sizeof bad, sizeof bad, UMI_STATUS_PARSE_ERROR));
    memcpy(bad, bytes, sizeof bad);
    Store(bad, 72U, 0x3f0U, 8U, 1); /* load segment runs past EOF */
    CHECK(ExpectFailure(bad, sizeof bad, sizeof bad, UMI_STATUS_PARSE_ERROR));
    memcpy(bad, bytes, sizeof bad);
    Store(bad, 136U, 0x80200100U, 8U, 1); /* overlapping VM ranges */
    CHECK(ExpectFailure(bad, sizeof bad, sizeof bad, UMI_STATUS_PARSE_ERROR));
    memcpy(bad, bytes, sizeof bad);
    Store(bad, 112U, 3U, 8U, 1); /* invalid alignment */
    CHECK(ExpectFailure(bad, sizeof bad, sizeof bad, UMI_STATUS_PARSE_ERROR));
    memcpy(bad, bytes, sizeof bad);
    Store(bad, 112U, 4096U, 8U, 1); /* offset/VMA incongruent */
    CHECK(ExpectFailure(bad, sizeof bad, sizeof bad, UMI_STATUS_PARSE_ERROR));
    memcpy(bad, bytes, sizeof bad);
    Store(bad, 80U, UINT64_MAX - 1U, 8U, 1); /* virtual range wraps */
    CHECK(ExpectFailure(bad, sizeof bad, sizeof bad, UMI_STATUS_PARSE_ERROR));
    memcpy(bad, bytes, sizeof bad);
    bad[4] = 42U; /* unknown ELF class */
    CHECK(ExpectFailure(bad, sizeof bad, sizeof bad, UMI_STATUS_PARSE_ERROR));
    return 0;
}

static int TestMutation(void)
{
    unsigned char standard[1024], damaged[1024];
    Fixture64(standard);
    uint32_t seed = UINT32_C(0x91d25f3a);
    UmiEducationElfLoadPlan result;
    for (unsigned i = 0U; i < 12000U; ++i) {
        memcpy(damaged, standard, sizeof damaged);
        seed ^= seed << 13U;
        seed ^= seed >> 17U;
        seed ^= seed << 5U;
        const size_t where = (size_t)(seed % 176U);
        damaged[where] ^= (unsigned char)(seed >> 19U);
        memset(&result, 0xa5, sizeof result);
        UmiEducationElfLoadPlan old = result;
        const UmiStatus status = UmiEducationElfLoadPlanInspectBytes(
            damaged, sizeof damaged, sizeof damaged, &result);
        if (status != UMI_STATUS_OK)
            CHECK(memcmp(&result, &old, sizeof result) == 0);
        else
            CHECK(result.segmentCount != 0U && result.segmentCount <= UMI_EDUCATION_ELF_LOAD_MAX_SEGMENTS);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fputs("Test requires one fixture path containing spaces.\n", stderr);
        return 2;
    }
    CHECK(TestValid() == 0);
    CHECK(TestRefusal() == 0);
    CHECK(TestMutation() == 0);
    unsigned char example[1024];
    Fixture64(example);
    FILE *file = fopen(argv[1], "wb");
    CHECK(file != NULL);
    CHECK(fwrite(example, 1U, sizeof example, file) == sizeof example);
    CHECK(fclose(file) == 0);
    UmiEducationElfLoadPlan report = {0};
    CHECK(UmiEducationElfLoadPlanInspectFile(argv[1], &report) == UMI_STATUS_OK);
    CHECK(report.segmentCount == 2U && report.entryInExecutableSegment != 0U);
    CHECK(UmiEducationElfLoadPlanInspectFile("this-file-does-not-exist-0077", &report)
          == UMI_STATUS_NOT_FOUND);
    puts("[OK] ELF programme headers: ELF32/ELF64, overflow, alignment, overlap, entry,"
         " prefix bounds, 12,000 deterministic mutations, spaced file path.");
    return 0;
}
