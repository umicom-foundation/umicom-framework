/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_book.c
 * PURPOSE: Verify ordered native book cataloguing, content preservation and
 *          refusal to replace outputs without explicit review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/site.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#if defined(_WIN32)
# include <direct.h>
# define CreateDirectoryLocal(path) _mkdir(path)
#else
# define CreateDirectoryLocal(path) mkdir(path, 0755)
#endif

static void CreateTree(const char *directory)
{
    char part[UMICOM_EDUCATION_PATH_CAPACITY];
    size_t length = strlen(directory);
    assert(length < sizeof(part));
    memcpy(part, directory, length + 1U);
    for (size_t i = 1U; i < length; ++i) {
        if (part[i] == '/' || part[i] == '\\') {
            char remembered = part[i];
            part[i] = '\0';
            assert(CreateDirectoryLocal(part) == 0 || errno == EEXIST);
            part[i] = remembered;
        }
    }
    assert(CreateDirectoryLocal(part) == 0 || errno == EEXIST);
}
static void WriteText(const char *path, const char *value)
{
    FILE *file = fopen(path, "wb");
    assert(file != NULL);
    assert(fwrite(value, 1U, strlen(value), file) == strlen(value));
    assert(fclose(file) == 0);
}
static void CheckContains(const char *path, const char *text, int shouldContain)
{
    FILE *file = fopen(path, "rb");
    assert(file != NULL);
    char bytes[8192];
    size_t count = fread(bytes, 1U, sizeof(bytes) - 1U, file);
    bytes[count] = '\0';
    assert(fclose(file) == 0);
    assert((strstr(bytes, text) != NULL) == shouldContain);
}
int main(int argc, char **argv)
{
    assert(argc == 2);
    char folder[UMICOM_EDUCATION_PATH_CAPACITY];
    char manifest[UMICOM_EDUCATION_PATH_CAPACITY];
    char first[UMICOM_EDUCATION_PATH_CAPACITY];
    char second[UMICOM_EDUCATION_PATH_CAPACITY];
    char output[UMICOM_EDUCATION_PATH_CAPACITY];
    assert(snprintf(folder, sizeof(folder), "%s/dropzone/documents/example", argv[1]) > 0);
    CreateTree(folder);
    assert(snprintf(manifest, sizeof(manifest), "%s/volume.yml", folder) > 0);
    assert(snprintf(first, sizeof(first), "%s/first.md", folder) > 0);
    assert(snprintf(second, sizeof(second), "%s/second.md", folder) > 0);
    assert(snprintf(output, sizeof(output), "%s/output/native-books/example.md", argv[1]) > 0);
    WriteText(manifest,
        "title: \"Practical C Book\"\n"
        "slug: example\n"
        "chapters:\n"
        "  - first.md\n"
        "  - second.md\n"
        "year: 2026\n");
    WriteText(first, "\xef\xbb\xbf# First chapter\nC23 records\n");
    WriteText(second, "# Second chapter\nThe working code\n");
    assert(UmicomEducationBookList(argv[1]) == 0);
    assert(UmicomEducationBookAssemble(argv[1], "example", 1) == 0);
    CheckContains(output, "# Practical C Book", 1);
    CheckContains(output, "C23 records", 1);
    CheckContains(output, "The working code", 1);
    CheckContains(output, "\xef\xbb\xbf", 0);
    assert(UmicomEducationBookAssemble(argv[1], "example", 0) == 0);
    WriteText(second, "# Second chapter\nEdited after first publication\n");
    assert(UmicomEducationBookAssemble(argv[1], "example", 0) == 3);
    CheckContains(output, "The working code", 1);
    assert(UmicomEducationBookAssemble(argv[1], "example", 1) == 0);
    CheckContains(output, "Edited after first publication", 1);
    WriteText(manifest, "title: Changed\nchapters:\n  - ../outside.md\n");
    assert(UmicomEducationBookAssemble(argv[1], "example", 1) == 2);
    CheckContains(output, "Edited after first publication", 1);
    WriteText(manifest, "title: Changed\nchapters:\n  - missing.md\n");
    assert(UmicomEducationBookAssemble(argv[1], "example", 1) == 2);
    CheckContains(output, "Edited after first publication", 1);
    assert(UmicomEducationBookAssemble(argv[1], "../escape", 1) == 2);
    puts("Native book composition, preservation and invalid-input tests passed.");
    return 0;
}
