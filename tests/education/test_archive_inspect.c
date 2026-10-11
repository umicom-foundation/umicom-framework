/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_archive_inspect.c
 * PURPOSE: Exercise ordinary ar, GNU and BSD names, truncation, overflow,
 *          output preservation, selected files and malformed-input handling.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/archive_inspect.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

typedef struct ArchiveBuffer {
    unsigned char bytes[32768];
    size_t used;
} ArchiveBuffer;

static void StartArchive(ArchiveBuffer *archive)
{
    memset(archive, 0, sizeof(*archive));
    memcpy(archive->bytes, "!<arch>\n", 8U);
    archive->used = 8U;
}

static int AddMember(ArchiveBuffer *archive, const char *name,
                      const unsigned char *body, size_t count)
{
    if (name == NULL || strlen(name) > 16U ||
        archive->used + 60U + count + 1U > sizeof(archive->bytes)) return 0;
    unsigned char header[60];
    memset(header, ' ', sizeof(header));
    memcpy(header, name, strlen(name));
    char number[32];
    int n = snprintf(number, sizeof(number), "%zu", count);
    if (n <= 0 || n > 10) return 0;
    memcpy(header + 48U, number, (size_t)n);
    header[58U] = '`'; header[59U] = '\n';
    memcpy(archive->bytes + archive->used, header, sizeof(header));
    archive->used += sizeof(header);
    if (count != 0U) {
        if (body == NULL) return 0;
        memcpy(archive->bytes + archive->used, body, count);
        archive->used += count;
    }
    if ((count & 1U) != 0U) archive->bytes[archive->used++] = '\n';
    return 1;
}

static int EqualOnError(const unsigned char *bytes, size_t length,
                        UmiStatus expected)
{
    UmiEducationArchiveReport report;
    memset(&report, 0x5a, sizeof(report));
    UmiEducationArchiveReport before = report;
    UmiStatus result = UmiEducationArchiveInspectBytes(bytes, length, &report);
    return result == expected && memcmp(&report, &before, sizeof(report)) == 0;
}

static int CheckMemoryFixtures(void)
{
    ArchiveBuffer archive;
    StartArchive(&archive);
    UmiEducationArchiveReport result;
    CHECK(UmiEducationArchiveInspectBytes(archive.bytes, archive.used, &result) == UMI_STATUS_OK);
    CHECK(result.memberCount == 0U && result.archiveBytes == 8U);

    static const unsigned char symbol[] = {0,0,0,0};
    static const unsigned char obj[] = {0x7f,'E','L','F',0};
    static const unsigned char table[] = "an_extremely_long_module_name.obj/\n";
    CHECK(AddMember(&archive, "/", symbol, sizeof(symbol)));
    CHECK(AddMember(&archive, "//", table, sizeof(table) - 1U));
    CHECK(AddMember(&archive, "notes.o/", obj, sizeof(obj)));
    CHECK(AddMember(&archive, "/0", obj, sizeof(obj)));
    CHECK(UmiEducationArchiveInspectBytes(archive.bytes, archive.used, &result) == UMI_STATUS_OK);
    CHECK(result.memberCount == 2U && result.objectCount == 2U);
    CHECK(result.metadataCount == 2U && result.recordedCount == 2U);
    CHECK(strcmp(result.members[0].name, "notes.o") == 0);
    CHECK(strcmp(result.members[1].name, "an_extremely_long_module_name.obj") == 0);
    CHECK(result.members[0].bodyBytes == sizeof(obj));
    CHECK(EqualOnError(archive.bytes, archive.used - 1U, UMI_STATUS_PARSE_ERROR));

    unsigned char corrupt[32768];
    memcpy(corrupt, archive.bytes, archive.used);
    corrupt[0] = '?';
    CHECK(EqualOnError(corrupt, archive.used, UMI_STATUS_PARSE_ERROR));
    memcpy(corrupt, archive.bytes, archive.used);
    corrupt[8U + 58U] = '!';
    CHECK(EqualOnError(corrupt, archive.used, UMI_STATUS_PARSE_ERROR));
    memcpy(corrupt, archive.bytes, archive.used);
    memset(corrupt + 8U + 48U, '9', 10U);
    CHECK(EqualOnError(corrupt, archive.used, UMI_STATUS_PARSE_ERROR));
    memcpy(corrupt, archive.bytes, archive.used);
    memcpy(corrupt + 8U + 48U, "1844674407", 10U);
    CHECK(EqualOnError(corrupt, archive.used, UMI_STATUS_PARSE_ERROR));
    memcpy(corrupt, archive.bytes, archive.used);
    corrupt[8U + 48U] = '-';
    CHECK(EqualOnError(corrupt, archive.used, UMI_STATUS_PARSE_ERROR));
    memcpy(corrupt, archive.bytes, archive.used);
    corrupt[archive.used - 1U] = '#';
    CHECK(EqualOnError(corrupt, archive.used, UMI_STATUS_PARSE_ERROR));

    static const unsigned char thin[] = "!<thin>\n";
    CHECK(EqualOnError(thin, sizeof(thin)-1U, UMI_STATUS_NOT_IMPLEMENTED));

    StartArchive(&archive);
    static const unsigned char bsdBody[] = "module_with_spaces.obj" "PAYLOAD";
    const size_t nameLength = sizeof("module_with_spaces.obj") - 1U;
    char extended[17];
    CHECK(snprintf(extended, sizeof(extended), "#1/%zu", nameLength) > 0);
    CHECK(AddMember(&archive, extended, bsdBody, sizeof(bsdBody)-1U));
    CHECK(UmiEducationArchiveInspectBytes(archive.bytes, archive.used, &result) == UMI_STATUS_OK);
    CHECK(result.memberCount == 1U && result.objectCount == 1U);
    CHECK(strcmp(result.members[0].name, "module_with_spaces.obj") == 0);
    CHECK(result.members[0].bodyBytes == sizeof("PAYLOAD") - 1U);

    /* BSD archives emitted by real archivers commonly NUL-pad embedded names,
     * including their __.SYMDEF symbol index. This metadata is not an object. */
    StartArchive(&archive);
    static const unsigned char paddedIndex[] = {'_', '_', '.', 'S', 'Y', 'M', 'D', 'E', 'F', 0, 0, 0};
    static const unsigned char paddedObject[] = {'l','e','d','g','e','r','.','o','b','j',0,0,7,8};
    CHECK(AddMember(&archive, "#1/12", paddedIndex, sizeof(paddedIndex)));
    CHECK(AddMember(&archive, "#1/12", paddedObject, sizeof(paddedObject)));
    CHECK(UmiEducationArchiveInspectBytes(archive.bytes, archive.used, &result) == UMI_STATUS_OK);
    CHECK(result.metadataCount == 1U && result.memberCount == 1U);
    CHECK(result.objectCount == 1U && result.members[0].bodyBytes == 2U);
    CHECK(strcmp(result.members[0].name, "ledger.obj") == 0);

    StartArchive(&archive);
    CHECK(AddMember(&archive, "/42", obj, sizeof(obj)));
    CHECK(EqualOnError(archive.bytes, archive.used, UMI_STATUS_PARSE_ERROR));

    StartArchive(&archive);
    CHECK(AddMember(&archive, "#1/30", obj, sizeof(obj)));
    CHECK(EqualOnError(archive.bytes, archive.used, UMI_STATUS_PARSE_ERROR));

    /* Reports retain only a bounded first page of archive entries while all
     * ordinary members contribute to the total count. */
    StartArchive(&archive);
    for (unsigned i = 0U; i < 38U; ++i) {
        char objectName[17];
        CHECK(snprintf(objectName, sizeof(objectName), "entry%u.o/", i) > 0);
        CHECK(AddMember(&archive, objectName, obj, sizeof(obj)));
    }
    CHECK(UmiEducationArchiveInspectBytes(archive.bytes, archive.used, &result) == UMI_STATUS_OK);
    CHECK(result.memberCount == 38U && result.objectCount == 38U);
    CHECK(result.recordedCount == UMI_EDUCATION_ARCHIVE_RECORDED_MEMBERS);
    CHECK(strcmp(result.members[0].name, "entry0.o") == 0);

    /* A deliberately bounded mutation run checks that untrusted headers never
     * modify the output report when they are refused. This is deterministic,
     * not a replacement for a fuzzing campaign with sanitizers. */
    StartArchive(&archive);
    CHECK(AddMember(&archive, "entry.obj/", obj, sizeof(obj)));
    uint32_t state = UINT32_C(0x75a51fe2);
    for (unsigned caseIndex = 0U; caseIndex < 12000U; ++caseIndex) {
        memcpy(corrupt, archive.bytes, archive.used);
        state = state * UINT32_C(1664525) + UINT32_C(1013904223);
        size_t position = 8U + ((size_t)(state >> 8U) % (archive.used - 8U));
        corrupt[position] ^= (unsigned char)((state & 0x7fU) + 1U);
        UmiEducationArchiveReport inspected;
        memset(&inspected, 0x3d, sizeof(inspected));
        UmiEducationArchiveReport preserved = inspected;
        UmiStatus status = UmiEducationArchiveInspectBytes(corrupt, archive.used, &inspected);
        if (status != UMI_STATUS_OK)
            CHECK(memcmp(&inspected, &preserved, sizeof(inspected)) == 0);
    }
    CHECK(UmiEducationArchiveInspectBytes(NULL, archive.used, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiEducationArchiveInspectBytes(archive.bytes, archive.used, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

static int CheckFileFixture(const char *path)
{
    ArchiveBuffer archive;
    StartArchive(&archive);
    static const unsigned char body[] = "sample";
    CHECK(AddMember(&archive, "module.obj/", body, sizeof(body)-1U));
    FILE *file = fopen(path, "wb");
    CHECK(file != NULL);
    CHECK(fwrite(archive.bytes, 1U, archive.used, file) == archive.used);
    CHECK(fclose(file) == 0);
    UmiEducationArchiveReport report;
    CHECK(UmiEducationArchiveInspectFile(path, &report) == UMI_STATUS_OK);
    CHECK(report.memberCount == 1U && report.objectCount == 1U);
    CHECK(strcmp(report.members[0].name, "module.obj") == 0);
    CHECK(remove(path) == 0);
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2 && argv[1][0] != '\0');
    CHECK(CheckMemoryFixtures() == 0);
    CHECK(CheckFileFixture(argv[1]) == 0);
    puts("[OK] GNU/BSD ar, ordinary objects, malformed sizes, mutation run and spaced file path.");
    return 0;
}
