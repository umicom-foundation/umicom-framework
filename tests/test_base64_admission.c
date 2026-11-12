/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_base64_admission.c
 *
 * PURPOSE:
 *   Exercise canonical decoding, exact spans, alias refusal and unchanged outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "umicom/base/base64.h"

typedef struct Vector {
    const char *name, *input, *expected;
    size_t input_length, expected_length;
    unsigned flags;
    UmiStatus status;
} Vector;
static const Vector vectors[] = {
    {"empty", "", "", 0U, 0U, 0U, UMI_STATUS_OK},
    {"one", "Zg==", "f", 4U, 1U, 0U, UMI_STATUS_OK},
    {"two", "Zm8=", "fo", 4U, 2U, 0U, UMI_STATUS_OK},
    {"three", "Zm9v", "foo", 4U, 3U, 0U, UMI_STATUS_OK},
    {"four", "Zm9vYg==", "foob", 8U, 4U, 0U, UMI_STATUS_OK},
    {"five", "Zm9vYmE=", "fooba", 8U, 5U, 0U, UMI_STATUS_OK},
    {"six", "Zm9vYmFy", "foobar", 8U, 6U, 0U, UMI_STATUS_OK},
    {"zero", "AA==", "\000", 4U, 1U, 0U, UMI_STATUS_OK},
    {"whitespace", " Z m\0119v\015\012Yg==\014\013", "foob", 15U, 4U, 1U, UMI_STATUS_OK},
    {"whitespace-only", " \011\015\012", "", 4U, 0U, 1U, UMI_STATUS_OK},
    {"space-strict", "Z g==", "", 5U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"single", "Z", "", 1U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"unpadded-two", "Zg", "", 2U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"unpadded-three", "Zm8", "", 3U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"padding-missing", "Zg=", "", 3U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"padding-leading", "=m9v", "", 4U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"padding-second", "Z=9v", "", 4U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"padding-middle", "Zg=A", "", 4U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"padding-only", "====", "", 4U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"padding-excess", "Zg===", "", 5U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"after-padding", "Zg==Zm9v", "", 8U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"after-padding-space", "Zg== \012A", "", 7U, 0U, 1U, UMI_STATUS_PARSE_ERROR},
    {"one-pad-bits", "Zh==", "", 4U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"two-pad-bits", "Zm9=", "", 4U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"punctuation", "Zm9!", "", 4U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"url-dash", "-AAA", "", 4U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"url-underscore", "_AAA", "", 4U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"embedded-zero", "Zg==\000", "", 5U, 0U, 0U, UMI_STATUS_PARSE_ERROR},
    {"non-ascii", "Zg==\302\200", "", 6U, 0U, 1U, UMI_STATUS_PARSE_ERROR},
    {"unknown-flags", "Zg==", "", 4U, 0U, 2U, UMI_STATUS_INVALID_ARGUMENT}
};
static void CheckVector(const Vector *v)
{
    unsigned char output[32], original[32];
    size_t count = 123U, measured = 321U;
    memset(output, 0xA5, sizeof(output));
    memcpy(original, output, sizeof(output));
    assert(UmiBase64DecodedSize(v->input, v->input_length, v->flags, &measured) == v->status);
    assert(UmiBase64Decode(v->input, v->input_length, v->flags,
        output, sizeof(output), &count) == v->status);
    if (v->status == UMI_STATUS_OK) {
        assert(count == v->expected_length && measured == count);
        assert(memcmp(output, v->expected, count) == 0);
        assert(memcmp(output + count, original + count, sizeof(output) - count) == 0);
    } else {
        assert(count == 123U && measured == 321U);
        assert(memcmp(output, original, sizeof(output)) == 0);
    }
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    for (size_t i = 0U; i < sizeof(vectors) / sizeof(vectors[0]); ++i)
        if (strcmp(argv[1], vectors[i].name) == 0) { CheckVector(&vectors[i]); return 0; }
    unsigned char output[16], original[16];
    size_t count = 123U;
    memset(output, 0xA5, sizeof(output));
    memcpy(original, output, sizeof(output));
    if (strcmp(argv[1], "capacity") == 0) {
        assert(UmiBase64Decode("Zm9vYmFy", 8U, 0U, output, 5U, &count) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(count == 123U && memcmp(output, original, sizeof(output)) == 0);
    } else if (strcmp(argv[1], "exact-capacity") == 0) {
        assert(UmiBase64Decode("Zm9vYmFy", 8U, 0U, output, 6U, &count) == UMI_STATUS_OK);
        assert(count == 6U && memcmp(output, "foobar", 6U) == 0);
        assert(output[6] == 0xA5U);
    } else if (strcmp(argv[1], "exact-span") == 0) {
        const char text[] = {'Z', 'g', '=', '=', '!'};
        assert(UmiBase64Decode(text, 4U, 0U, output, sizeof(output), &count) == UMI_STATUS_OK);
        assert(count == 1U && output[0] == 'f');
        count = 123U;
        memcpy(original, output, sizeof(output));
        assert(UmiBase64Decode(text, sizeof(text), 0U, output, sizeof(output), &count) == UMI_STATUS_PARSE_ERROR);
        assert(count == 123U && memcmp(output, original, sizeof(output)) == 0);
    } else if (strcmp(argv[1], "null-empty") == 0) {
        assert(UmiBase64DecodedSize(NULL, 0U, 0U, &count) == UMI_STATUS_OK && count == 0U);
        count = 123U;
        assert(UmiBase64Decode(NULL, 0U, 0U, NULL, 0U, &count) == UMI_STATUS_OK && count == 0U);
    } else if (strcmp(argv[1], "null-input") == 0) {
        assert(UmiBase64DecodedSize(NULL, 4U, 0U, &count) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiBase64Decode(NULL, 4U, 0U, output, sizeof(output), &count) == UMI_STATUS_INVALID_ARGUMENT);
        assert(count == 123U && memcmp(output, original, sizeof(output)) == 0);
    } else if (strcmp(argv[1], "null-output") == 0) {
        assert(UmiBase64Decode("Zg==", 4U, 0U, NULL, 1U, &count) == UMI_STATUS_INVALID_ARGUMENT);
        assert(count == 123U);
    } else if (strcmp(argv[1], "null-count") == 0) {
        assert(UmiBase64DecodedSize("Zg==", 4U, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiBase64Decode("Zg==", 4U, 0U, output, sizeof(output), NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(output, original, sizeof(output)) == 0);
    } else if (strcmp(argv[1], "overlap") == 0) {
        char text[16] = "Zm9vYmFy";
        assert(UmiBase64Decode(text, 8U, 0U, (unsigned char *)text, sizeof(text), &count) == UMI_STATUS_INVALID_ARGUMENT);
        assert(count == 123U && strcmp(text, "Zm9vYmFy") == 0);
        assert(UmiBase64Decode(text, 8U, 0U, (unsigned char *)text + 3U, 6U, &count) == UMI_STATUS_INVALID_ARGUMENT);
        assert(count == 123U && strcmp(text, "Zm9vYmFy") == 0);
    } else if (strcmp(argv[1], "count-alias") == 0) {
        union { size_t count; unsigned char bytes[32]; } storage;
        unsigned char saved[sizeof(storage)];
        memset(&storage, 0, sizeof(storage));
        memcpy(storage.bytes, "Zm9vYmFy", 8U);
        memcpy(saved, &storage, sizeof(storage));
        assert(UmiBase64DecodedSize((const char *)storage.bytes, 8U, 0U, &storage.count) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiBase64Decode((const char *)storage.bytes, 8U, 0U, output, sizeof(output), &storage.count) == UMI_STATUS_INVALID_ARGUMENT);
        assert(UmiBase64Decode("Zm9vYmFy", 8U, 0U, storage.bytes, sizeof(storage.bytes), &storage.count) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(saved, &storage, sizeof(storage)) == 0);
        assert(memcmp(output, original, sizeof(output)) == 0);
    } else if (strcmp(argv[1], "long") == 0) {
        char text[12000]; unsigned char bytes[9000];
        for (size_t i = 0U; i < sizeof(text); i += 4U) memcpy(text + i, "AP/+", 4U);
        assert(UmiBase64Decode(text, sizeof(text), 0U, bytes, sizeof(bytes), &count) == UMI_STATUS_OK);
        assert(count == sizeof(bytes));
        for (size_t i = 0U; i < count; i += 3U)
            assert(bytes[i] == 0U && bytes[i + 1U] == 255U && bytes[i + 2U] == 254U);
    } else return 2;
    return 0;
}
