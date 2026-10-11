/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_binary_inspect.c
 * PURPOSE: Verify native ELF/PE/COFF parsing and failure-atomic reports.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/binary_inspect.h"
#include <stdio.h>
#include <string.h>

#define CHECK(EXPR) do { if (!(EXPR)) { \
    fprintf(stderr, "[FAIL] line %d: %s\n", __LINE__, #EXPR); return 1; \
} } while (0)

static void Put16(unsigned char *bytes, size_t at, uint16_t value, int little)
{
    bytes[at + (little ? 0U : 1U)] = (unsigned char)value;
    bytes[at + (little ? 1U : 0U)] = (unsigned char)(value >> 8U);
}
static void Put32(unsigned char *bytes, size_t at, uint32_t value, int little)
{
    for (size_t i = 0U; i < 4U; ++i)
        bytes[at + (little ? i : 3U - i)] = (unsigned char)(value >> (8U * i));
}
static void Put64(unsigned char *bytes, size_t at, uint64_t value, int little)
{
    for (size_t i = 0U; i < 8U; ++i)
        bytes[at + (little ? i : 7U - i)] = (unsigned char)(value >> (8U * i));
}

static int TestElf(void)
{
    unsigned char bytes[512] = {0};
    memcpy(bytes, "\x7f" "ELF", 4U);
    bytes[4] = 2U; /* ELF64 */
    bytes[5] = 1U; /* little-endian */
    bytes[6] = 1U;
    Put16(bytes, 16U, 2U, 1);
    Put16(bytes, 18U, 243U, 1); /* RISC-V */
    Put64(bytes, 24U, UINT64_C(0x80200000), 1);
    Put64(bytes, 40U, 128U, 1);
    Put16(bytes, 52U, 64U, 1);
    Put16(bytes, 58U, 64U, 1);
    Put16(bytes, 60U, 2U, 1);
    UmiEducationBinaryHeader result = {0};
    CHECK(UmiEducationBinaryInspectBytes(bytes, sizeof(bytes), sizeof(bytes), &result) == UMI_STATUS_OK);
    CHECK(result.format == UMI_EDUCATION_BINARY_ELF && result.addressBits == 64U);
    CHECK(result.sections == 2U && result.entryPoint == UINT64_C(0x80200000));
    CHECK(result.machine == 243U && strcmp(UmiEducationBinaryMachineText(&result), "RISC-V") == 0);
    CHECK(strcmp(UmiEducationBinaryTypeText(&result), "Executable image") == 0);
    UmiEducationBinaryHeader snapshot = result;
    /* Refuse a section table whose declared range exceeds the file length. */
    Put64(bytes, 40U, 450U, 1);
    CHECK(UmiEducationBinaryInspectBytes(bytes, sizeof(bytes), sizeof(bytes), &result) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&snapshot, &result, sizeof(result)) == 0);
    Put64(bytes, 40U, 128U, 1);
    CHECK(UmiEducationBinaryInspectBytes(bytes, 63U, sizeof(bytes), &result) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&snapshot, &result, sizeof(result)) == 0);
    /* Nonzero shoff and shnum=0 is the ELF extended-count convention. */
    Put16(bytes, 60U, 0U, 1);
    CHECK(UmiEducationBinaryInspectBytes(bytes, sizeof(bytes), sizeof(bytes), &result) == UMI_STATUS_OK);
    CHECK(result.sectionCountExtended && result.sections == 0U);
    /* The minimum ELF32 header also works when written big endian. */
    memset(bytes, 0, sizeof(bytes));
    memcpy(bytes, "\x7f" "ELF", 4U);
    bytes[4] = 1U; bytes[5] = 2U; bytes[6] = 1U;
    Put16(bytes, 16U, 1U, 0);
    Put16(bytes, 18U, 3U, 0);
    Put32(bytes, 24U, 0x12345678U, 0);
    Put16(bytes, 40U, 52U, 0);
    CHECK(UmiEducationBinaryInspectBytes(bytes, 52U, 52U, &result) == UMI_STATUS_OK);
    CHECK(result.addressBits == 32U && !result.littleEndian && result.entryPoint == 0x12345678U);
    CHECK(strcmp(UmiEducationBinaryTypeText(&result), "Relocatable object") == 0);
    return 0;
}

static int TestPe(void)
{
    unsigned char bytes[1024] = {0};
    bytes[0] = 'M'; bytes[1] = 'Z';
    Put32(bytes, 0x3cU, 128U, 1);
    memcpy(bytes + 128U, "PE\0\0", 4U);
    Put16(bytes, 132U, 0x8664U, 1);
    Put16(bytes, 134U, 2U, 1);
    Put16(bytes, 148U, 240U, 1);
    Put16(bytes, 152U, 0x20bU, 1);
    Put32(bytes, 168U, 0x1800U, 1);
    UmiEducationBinaryHeader result = {0};
    CHECK(UmiEducationBinaryInspectBytes(bytes, sizeof(bytes), sizeof(bytes), &result) == UMI_STATUS_OK);
    CHECK(result.format == UMI_EDUCATION_BINARY_PE && result.machine == 0x8664U);
    CHECK(result.entryPoint == 0x1800U && result.entryIsRva && result.addressBits == 64U);
    CHECK(result.sections == 2U && result.littleEndian);
    UmiEducationBinaryHeader snapshot = result;
    Put32(bytes, 0x3cU, 2048U, 1);
    CHECK(UmiEducationBinaryInspectBytes(bytes, sizeof(bytes), sizeof(bytes), &result) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&snapshot, &result, sizeof(result)) == 0);
    Put32(bytes, 0x3cU, 128U, 1);
    Put16(bytes, 148U, 900U, 1); /* section table exceeds the file */
    CHECK(UmiEducationBinaryInspectBytes(bytes, sizeof(bytes), sizeof(bytes), &result) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&snapshot, &result, sizeof(result)) == 0);
    Put16(bytes, 148U, 240U, 1);
    CHECK(UmiEducationBinaryInspectBytes(bytes, 80U, sizeof(bytes), &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&snapshot, &result, sizeof(result)) == 0);
    /* A PE32 image has the same four-byte entry-point field. */
    Put16(bytes, 152U, 0x10bU, 1);
    CHECK(UmiEducationBinaryInspectBytes(bytes, sizeof(bytes), sizeof(bytes), &result) == UMI_STATUS_OK);
    CHECK(result.addressBits == 32U && result.entryIsRva);
    return 0;
}

static int TestCoff(void)
{
    unsigned char bytes[100] = {0};
    Put16(bytes, 0U, 0x8664U, 1);
    Put16(bytes, 2U, 2U, 1);
    UmiEducationBinaryHeader result = {0};
    CHECK(UmiEducationBinaryInspectBytes(bytes, sizeof(bytes), sizeof(bytes), &result) == UMI_STATUS_OK);
    CHECK(result.format == UMI_EDUCATION_BINARY_COFF && result.sections == 2U);
    CHECK(strcmp(UmiEducationBinaryTypeText(&result), "Relocatable object") == 0);
    UmiEducationBinaryHeader old = result;
    CHECK(UmiEducationBinaryInspectBytes(bytes, 30U, 30U, &result) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&result, &old, sizeof(result)) == 0);
    CHECK(UmiEducationBinaryInspectBytes(NULL, 4U, 4U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEducationBinaryInspectBytes(bytes, sizeof(bytes), 10U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEducationBinaryInspectBytes(bytes, sizeof(bytes), sizeof(bytes), NULL) == UMI_STATUS_INVALID_ARGUMENT);
    const unsigned char unknown[] = {'N', 'O', 'P', 'E'};
    CHECK(UmiEducationBinaryInspectBytes(unknown, sizeof(unknown), sizeof(unknown), &result) != UMI_STATUS_OK);
    CHECK(memcmp(&result, &old, sizeof(result)) == 0);
    return 0;
}


/* Supply different bounded byte spans without guessing that fuzz inputs are
 * text. Every rejected parse must preserve the caller's previous report. */
static int TestMalformedSamples(void)
{
    unsigned char bytes[256] = {0};
    uint32_t state = UINT32_C(0x18ce9843);
    for (unsigned attempt = 0U; attempt < 12000U; ++attempt) {
        for (size_t i = 0U; i < sizeof(bytes); ++i) {
            state ^= state << 13U;
            state ^= state >> 17U;
            state ^= state << 5U;
            bytes[i] = (unsigned char)state;
        }
        UmiEducationBinaryHeader report = {0};
        report.format = UMI_EDUCATION_BINARY_PE;
        report.entryPoint = UINT64_C(0x0123456789abcdef);
        UmiEducationBinaryHeader before = report;
        const size_t length = (size_t)(attempt % (sizeof(bytes) + 1U));
        UmiStatus status = UmiEducationBinaryInspectBytes(bytes, length, length, &report);
        if (status != UMI_STATUS_OK)
            CHECK(memcmp(&before, &report, sizeof(report)) == 0);
    }
    return 0;
}

static int TestFileRoundTrip(const char *runningExecutable, const char *fixture)
{
    UmiEducationBinaryHeader info = {0};
    /* Validate one real compiler/linker-produced image in addition to fake
     * test headers. It must be ELF on Linux or PE on Windows. */
    CHECK(UmiEducationBinaryInspectFile(runningExecutable, &info) == UMI_STATUS_OK);
    CHECK(info.format == UMI_EDUCATION_BINARY_PE || info.format == UMI_EDUCATION_BINARY_ELF);
    CHECK(info.fileBytes > 100U);
    unsigned char coff[60] = {0};
    Put16(coff, 0U, 0x8664U, 1);
    Put16(coff, 2U, 1U, 1);
    FILE *output = fopen(fixture, "wb");
    CHECK(output != NULL);
    CHECK(fwrite(coff, 1U, sizeof(coff), output) == sizeof(coff));
    CHECK(fclose(output) == 0);
    CHECK(UmiEducationBinaryInspectFile(fixture, &info) == UMI_STATUS_OK);
    CHECK(info.format == UMI_EDUCATION_BINARY_COFF && info.sections == 1U);
    CHECK(remove(fixture) == 0);
    CHECK(UmiEducationBinaryInspectFile("nonexistent-umicom-education-image-404", &info) == UMI_STATUS_IO_ERROR);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Expected executable and temporary fixture path.\n");
        return 2;
    }
    if (TestElf() || TestPe() || TestCoff() || TestMalformedSamples() ||
        TestFileRoundTrip(argv[1], argv[2])) return 1;
    puts("[OK] ELF32/ELF64, PE32/PE32+, COFF, 12,000 malformed samples, actual image and failure atomicity.");
    return 0;
}
