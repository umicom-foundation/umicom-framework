/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_hash_failures.c
 *
 * PURPOSE:
 *   Verify streaming fingerprint outputs remain private until input and close succeed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/normalizer.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
static const unsigned char *input;
static size_t input_size;
static unsigned opens, reads, closes;
static int fail_open, fail_read, fail_close;
static FILE *HashOpen(const char *path, const char *mode)
{
    (void)path; (void)mode;
    ++opens;
    if (fail_open) return NULL;
    FILE *file = tmpfile();
    if (file == NULL) return NULL;
    if (fwrite(input, 1U, input_size, file) != input_size || fflush(file) != 0) {
        (void)fclose(file);
        return NULL;
    }
    rewind(file);
    return file;
}
static size_t HashRead(void *buffer, size_t size, size_t count, FILE *file)
{
    ++reads;
    if (fail_read && reads > 1U) return 0U;
    return fread(buffer, size, count, file);
}
static int HashError(FILE *file) { return (fail_read && reads != 0U) || ferror(file); }
static int HashClose(FILE *file) { ++closes; int result = fclose(file); return fail_close ? EOF : result; }
/* Actual streaming normalization with controlled stream failures. */
#define fopen HashOpen
#define fread HashRead
#define ferror HashError
#define fclose HashClose
#include "../../src/codeguard/normalizer.c"
#undef fclose
#undef ferror
#undef fread
#undef fopen

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    const char *text = "int x = 1;\n", *normalized = "intx=1;";
    unsigned char boundary[4200], binary[] = {'a', 0, 'b'};
    if (!strcmp(name, "comments")) text = "int/* note */ x=1;// tail\n";
    else if (!strcmp(name, "literals")) {
        text = "char *s=\"a b//c\"; char c=' ';";
        normalized = "char*s=\"a b//c\";charc=' ';";
    } else if (!strcmp(name, "escape")) {
        text = "char *s=\"a\\\" /* still literal */ b\";";
        normalized = "char*s=\"a\\\" /* still literal */ b\";";
    } else if (!strcmp(name, "slash")) { text = "value/"; normalized = "value/"; }
    else if (!strcmp(name, "empty")) { text = ""; normalized = ""; }
    else if (!strcmp(name, "open")) fail_open = 1;
    else if (!strcmp(name, "read")) fail_read = 1;
    else if (!strcmp(name, "close")) fail_close = 1;
    else if (!strcmp(name, "read-close")) { fail_read = 1; fail_close = 1; }
    else if (strcmp(name, "ordinary") && strcmp(name, "arguments") &&
        strcmp(name, "boundary") && strcmp(name, "binary")) return 2;
    input = (const unsigned char *)text;
    input_size = strlen(text);
    if (!strcmp(name, "boundary")) {
        memset(boundary, ' ', 4095U);
        const char *tail = "/* split comment */int x;";
        memcpy(boundary + 4095U, tail, strlen(tail));
        input = boundary; input_size = 4095U + strlen(tail); normalized = "intx;";
    }
    if (!strcmp(name, "binary")) { input = binary; input_size = sizeof binary; }
    uint64_t raw = 123U, normal = 456U;
    size_t size = 789U;
    if (!strcmp(name, "arguments")) {
        CHECK(umi_codeguard_hash_file(NULL, &raw, &normal, &size) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_hash_file("fixture.c", NULL, &normal, &size) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_hash_file("fixture.c", &raw, NULL, &size) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_hash_file("fixture.c", &raw, &normal, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(opens == 0U && raw == 123U && normal == 456U && size == 789U);
        return 0;
    }
    UmiStatus status = umi_codeguard_hash_file("fixture.c", &raw, &normal, &size);
    if (fail_open || fail_read || fail_close) {
        CHECK(status == UMI_STATUS_IO_ERROR && raw == 123U && normal == 456U && size == 789U);
        CHECK(closes == (fail_open ? 0U : 1U));
    } else {
        CHECK(status == UMI_STATUS_OK && opens == 1U && closes == 1U && size == input_size);
        CHECK(raw == umi_codeguard_hash_bytes(input, input_size));
        CHECK(normal == (!strcmp(name, "binary") ? umi_codeguard_hash_bytes(binary, sizeof binary) :
            umi_codeguard_hash_bytes(normalized, strlen(normalized))));
    }
    return 0;
}
