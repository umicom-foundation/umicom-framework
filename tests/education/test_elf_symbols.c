/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_elf_symbols.c
 * PURPOSE:
 *   Verify native ELF32/64 symbols and relocation table bounds, report
 *   atomicity, fuzzed byte sequences and a real path containing spaces.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/elf_symbols.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAILED %s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)

static void WriteUnsigned(unsigned char *bytes, size_t where, uint64_t value,
                          size_t count, int little)
{
    for (size_t i = 0U; i < count; ++i) {
        size_t shift = little ? i * 8U : (count - i - 1U) * 8U;
        bytes[where + i] = (unsigned char)(value >> shift);
    }
}

static void Elf64Fixture(unsigned char bytes[512])
{
    static const char names[] = "\0umicom_export\0umicom_missing\0";
    memset(bytes, 0, 512U);
    memcpy(bytes, "\177ELF", 4U);
    bytes[4] = 2U; bytes[5] = 1U; bytes[6] = 1U;
    WriteUnsigned(bytes, 16U, 1U, 2U, 1);       /* ET_REL */
    WriteUnsigned(bytes, 18U, 62U, 2U, 1);      /* EM_X86_64 */
    WriteUnsigned(bytes, 20U, 1U, 4U, 1);
    WriteUnsigned(bytes, 40U, 64U, 8U, 1);      /* SHT begins at byte 64 */
    WriteUnsigned(bytes, 52U, 64U, 2U, 1);
    WriteUnsigned(bytes, 58U, 64U, 2U, 1);
    WriteUnsigned(bytes, 60U, 4U, 2U, 1);
    /* Section 1: SYMTAB, offset 320, 3 * 24 byte entries; link to section 2. */
    WriteUnsigned(bytes, 128U + 4U, 2U, 4U, 1);
    WriteUnsigned(bytes, 128U + 24U, 320U, 8U, 1);
    WriteUnsigned(bytes, 128U + 32U, 72U, 8U, 1);
    WriteUnsigned(bytes, 128U + 40U, 2U, 4U, 1);
    WriteUnsigned(bytes, 128U + 56U, 24U, 8U, 1);
    /* Section 2: STRTAB immediately after symbols. */
    WriteUnsigned(bytes, 192U + 4U, 3U, 4U, 1);
    WriteUnsigned(bytes, 192U + 24U, 392U, 8U, 1);
    WriteUnsigned(bytes, 192U + 32U, sizeof names, 8U, 1);
    memcpy(bytes + 392U, names, sizeof names);
    /* Section 3: RELA, pointing to section 1 SYMTAB. */
    WriteUnsigned(bytes, 256U + 4U, 4U, 4U, 1);
    WriteUnsigned(bytes, 256U + 24U, 440U, 8U, 1);
    WriteUnsigned(bytes, 256U + 32U, 24U, 8U, 1);
    WriteUnsigned(bytes, 256U + 40U, 1U, 4U, 1);
    WriteUnsigned(bytes, 256U + 56U, 24U, 8U, 1);
    /* Symbol 1: externally visible function, defined in section 1. */
    WriteUnsigned(bytes, 344U, 1U, 4U, 1);
    bytes[348U] = 0x12U; /* GLOBAL FUNC */
    WriteUnsigned(bytes, 350U, 1U, 2U, 1);
    WriteUnsigned(bytes, 352U, 0x10U, 8U, 1);
    WriteUnsigned(bytes, 360U, 32U, 8U, 1);
    /* Symbol 2: unresolved global symbol. */
    WriteUnsigned(bytes, 368U, 15U, 4U, 1);
    bytes[372U] = 0x10U; /* GLOBAL NOTYPE */
    /* One relocation referencing symbol 2 (x86-64 absolute 64-bit). */
    WriteUnsigned(bytes, 440U, 8U, 8U, 1);
    WriteUnsigned(bytes, 448U, ((uint64_t)2U << 32U) | 1U, 8U, 1);
}

static void Elf32BigEndian(unsigned char bytes[256])
{
    static const char names[] = "\0kernel_start\0";
    memset(bytes, 0, 256U);
    memcpy(bytes, "\177ELF", 4U);
    bytes[4] = 1U; bytes[5] = 2U; bytes[6] = 1U;
    WriteUnsigned(bytes, 16U, 1U, 2U, 0);
    WriteUnsigned(bytes, 18U, 243U, 2U, 0); /* EM_RISCV */
    WriteUnsigned(bytes, 20U, 1U, 4U, 0);
    WriteUnsigned(bytes, 32U, 52U, 4U, 0);
    WriteUnsigned(bytes, 40U, 52U, 2U, 0);
    WriteUnsigned(bytes, 46U, 40U, 2U, 0);
    WriteUnsigned(bytes, 48U, 3U, 2U, 0);
    /* ELF32: section 1 symtab, 2 entries * 16 at 172. */
    WriteUnsigned(bytes, 92U + 4U, 2U, 4U, 0);
    WriteUnsigned(bytes, 92U + 16U, 172U, 4U, 0);
    WriteUnsigned(bytes, 92U + 20U, 32U, 4U, 0);
    WriteUnsigned(bytes, 92U + 24U, 2U, 4U, 0);
    WriteUnsigned(bytes, 92U + 36U, 16U, 4U, 0);
    WriteUnsigned(bytes, 132U + 4U, 3U, 4U, 0);
    WriteUnsigned(bytes, 132U + 16U, 204U, 4U, 0);
    WriteUnsigned(bytes, 132U + 20U, sizeof names, 4U, 0);
    memcpy(bytes + 204U, names, sizeof names);
    WriteUnsigned(bytes, 188U, 1U, 4U, 0);
    WriteUnsigned(bytes, 192U, 0x40U, 4U, 0);
    WriteUnsigned(bytes, 196U, 8U, 4U, 0);
    bytes[200U] = 0x12U;
    WriteUnsigned(bytes, 202U, 1U, 2U, 0);
}

static int CheckUnchangedOnError(const unsigned char *bytes, size_t length,
                                 UmiStatus expected)
{
    UmiEducationElfReport output;
    memset(&output, 0xa5, sizeof output);
    UmiEducationElfReport old = output;
    UmiStatus result = UmiEducationElfInspectBytes(bytes, length, &output);
    return result == expected && memcmp(&old, &output, sizeof output) == 0;
}

static int TestSynthetic(void)
{
    unsigned char elf64[512], elf32[256];
    Elf64Fixture(elf64);
    Elf32BigEndian(elf32);
    UmiEducationElfReport result = {0};
    CHECK(UmiEducationElfInspectBytes(elf64, sizeof elf64, &result) == UMI_STATUS_OK);
    CHECK(result.fileBytes == 512U && result.sectionCount == 4U);
    CHECK(result.addressBits == 64U && result.littleEndian == 1U && result.machine == 62U);
    CHECK(result.staticSymbolTables == 1U && result.dynamicSymbolTables == 0U);
    CHECK(result.symbolCount == 2U && result.globalCount == 2U);
    CHECK(result.definedCount == 1U && result.undefinedCount == 1U);
    CHECK(result.functionCount == 1U && result.relaCount == 1U && result.relCount == 0U);
    CHECK(result.displayedCount == 2U);
    CHECK(strcmp(result.displayed[0].name, "umicom_export") == 0);
    CHECK(strcmp(result.displayed[1].name, "umicom_missing") == 0);
    CHECK(UmiEducationElfInspectBytes(elf32, sizeof elf32, &result) == UMI_STATUS_OK);
    CHECK(result.addressBits == 32U && result.littleEndian == 0U && result.machine == 243U);
    CHECK(result.symbolCount == 1U && result.displayedCount == 1U);
    CHECK(strcmp(result.displayed[0].name, "kernel_start") == 0);
    CHECK(result.definedCount == 1U && result.relocationCount == 0U);
    return 0;
}

static int TestInvalidTables(void)
{
    unsigned char original[512], corrupt[512];
    Elf64Fixture(original);
    CHECK(CheckUnchangedOnError((const unsigned char *)"COFF", 4U, UMI_STATUS_NOT_IMPLEMENTED));
    CHECK(CheckUnchangedOnError(original, 20U, UMI_STATUS_PARSE_ERROR));
    CHECK(CheckUnchangedOnError(NULL, 0U, UMI_STATUS_NOT_IMPLEMENTED));
    for (size_t length = 4U; length < 464U; ++length)
        CHECK(CheckUnchangedOnError(original, length, UMI_STATUS_PARSE_ERROR));
    memcpy(corrupt, original, sizeof corrupt);
    WriteUnsigned(corrupt, 40U, UINT64_MAX, 8U, 1);
    CHECK(CheckUnchangedOnError(corrupt, sizeof corrupt, UMI_STATUS_PARSE_ERROR));
    /* Names are metadata from an untrusted file. Do not echo ANSI escapes
     * into a terminal when printing the bounded symbol preview. */
    memcpy(corrupt, original, sizeof corrupt);
    corrupt[393U] = 0x1bU;
    UmiEducationElfReport cleaned = {0};
    CHECK(UmiEducationElfInspectBytes(corrupt, sizeof corrupt, &cleaned) == UMI_STATUS_OK);
    CHECK(strcmp(cleaned.displayed[0].name, "?micom_export") == 0);
    memcpy(corrupt, original, sizeof corrupt);
    WriteUnsigned(corrupt, 128U + 40U, 7U, 4U, 1); /* bad linked string table */
    CHECK(CheckUnchangedOnError(corrupt, sizeof corrupt, UMI_STATUS_PARSE_ERROR));
    memcpy(corrupt, original, sizeof corrupt);
    WriteUnsigned(corrupt, 128U + 56U, 0U, 8U, 1); /* zero entry size */
    CHECK(CheckUnchangedOnError(corrupt, sizeof corrupt, UMI_STATUS_PARSE_ERROR));
    memcpy(corrupt, original, sizeof corrupt);
    WriteUnsigned(corrupt, 368U, 900U, 4U, 1); /* out-of-bounds symbol name */
    CHECK(CheckUnchangedOnError(corrupt, sizeof corrupt, UMI_STATUS_PARSE_ERROR));
    memcpy(corrupt, original, sizeof corrupt);
    WriteUnsigned(corrupt, 256U + 40U, 0U, 4U, 1); /* relocation not linked to symbols */
    CHECK(CheckUnchangedOnError(corrupt, sizeof corrupt, UMI_STATUS_PARSE_ERROR));
    memcpy(corrupt, original, sizeof corrupt);
    WriteUnsigned(corrupt, 256U + 32U, UINT64_MAX, 8U, 1);
    CHECK(CheckUnchangedOnError(corrupt, sizeof corrupt, UMI_STATUS_PARSE_ERROR));
    /* A declared extended count must be bounded before section iteration. */
    memcpy(corrupt, original, sizeof corrupt);
    WriteUnsigned(corrupt, 60U, 0U, 2U, 1);
    WriteUnsigned(corrupt, 64U + 32U, 0x12345678U, 8U, 1);
    CHECK(CheckUnchangedOnError(corrupt, sizeof corrupt, UMI_STATUS_PARSE_ERROR));
    /* The synthetic image also works when its count is legitimately extended. */
    memcpy(corrupt, original, sizeof corrupt);
    WriteUnsigned(corrupt, 60U, 0U, 2U, 1);
    WriteUnsigned(corrupt, 64U + 32U, 4U, 8U, 1);
    UmiEducationElfReport checked = {0};
    CHECK(UmiEducationElfInspectBytes(corrupt, sizeof corrupt, &checked) == UMI_STATUS_OK);
    CHECK(checked.sectionCount == 4U && checked.symbolCount == 2U);
    return 0;
}

static int TestMutatedInputs(void)
{
    unsigned char original[512], candidate[512];
    Elf64Fixture(original);
    uint32_t state = 0x9e3779b9U;
    for (unsigned index = 0U; index < 12000U; ++index) {
        memcpy(candidate, original, sizeof candidate);
        state = state * UINT32_C(1664525) + UINT32_C(1013904223);
        size_t at = (size_t)(state % sizeof candidate);
        state = state * UINT32_C(1664525) + UINT32_C(1013904223);
        candidate[at] ^= (unsigned char)(1U << (state % 8U));
        UmiEducationElfReport report;
        memset(&report, 0x5a, sizeof report);
        UmiEducationElfReport before = report;
        UmiStatus status = UmiEducationElfInspectBytes(candidate, sizeof candidate, &report);
        if (status == UMI_STATUS_OK) {
            CHECK(report.symbolCount <= UMI_EDUCATION_ELF_MAX_RECORDS);
            CHECK(report.relocationCount <= UMI_EDUCATION_ELF_MAX_RECORDS);
        } else {
            CHECK(memcmp(&before, &report, sizeof before) == 0);
        }
    }
    puts("[OK] 12000 deterministic mutations and error-atomic reports");
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    CHECK(TestSynthetic() == 0);
    CHECK(TestInvalidTables() == 0);
    CHECK(TestMutatedInputs() == 0);
    unsigned char fixture[512];
    Elf64Fixture(fixture);
    FILE *file = fopen(argv[1], "wb");
    CHECK(file != NULL);
    CHECK(fwrite(fixture, 1U, sizeof fixture, file) == sizeof fixture);
    CHECK(fclose(file) == 0);
    UmiEducationElfReport report = {0};
    CHECK(UmiEducationElfInspectFile(argv[1], &report) == UMI_STATUS_OK);
    CHECK(report.symbolCount == 2U && report.relocationCount == 1U);
    printf("[OK] ELF symbols, relocations, endian variants, spaced path: %s\n", argv[1]);
    return 0;
}
