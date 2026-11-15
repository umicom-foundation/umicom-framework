/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_bounded_source_native.c
 *
 * PURPOSE:
 *   Verify bounded CodeGuard analysis against real files including UTF-8 names and chunk boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/source.h"
#include "umicom/codeguard/normalizer.h"
#include "../../src/codeguard/source_reader.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1];
    static const char *cases[] = {"ordinary", "exact", "over", "zero", "empty",
        "unicode", "long", "native-boundary", "nul-text", "nul-hash", "missing",
        "directory", "cancel", "alias", "hash-exact", "hash-over"};
    size_t selected = 0U;
    while (selected < sizeof cases / sizeof cases[0] && strcmp(cases[selected], name)) ++selected;
    if (selected == sizeof cases / sizeof cases[0]) return 2;
    char path[1024];
    const char *leaf = "ordinary.c";
    if (!strcmp(name, "empty")) leaf = "empty.c";
    if (!strcmp(name, "unicode")) leaf = "\xe6\xa8\xa1\xe5\x9e\x8b.c";
    if (!strcmp(name, "long")) leaf = "long.c";
    if (!strcmp(name, "native-boundary")) leaf = "boundary.c";
    if (!strcmp(name, "nul-text") || !strcmp(name, "nul-hash")) leaf = "binary.bin";
    if (!strcmp(name, "missing")) leaf = "absent.c";
    if (!strcmp(name, "directory")) leaf = "directory.c";
    int length = snprintf(path, sizeof path, "%s/%s", argv[2], leaf);
    CHECK(length > 0 && (size_t)length < sizeof path);
    UmiCancellationToken *token = NULL;
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    if (!strcmp(name, "cancel")) umi_cancellation_token_request(token);
    UmiCodeGuardSourceInfo info;
    memset(&info, 0xa5, sizeof info);
    unsigned char prior[sizeof info];
    uint64_t maximum = 1024U * 1024U;
    /* The fixture's ordinary ASCII payload is seven bytes, including LF. */
    if (!strcmp(name, "exact") || !strcmp(name, "hash-exact")) maximum = 7U;
    if (!strcmp(name, "over") || !strcmp(name, "hash-over")) maximum = 6U;
    if (!strcmp(name, "zero") || !strcmp(name, "empty")) maximum = 0U;
    const char *input_path = path;
    if (!strcmp(name, "alias")) { strcpy(info.path, path); input_path = info.path; }
    memcpy(prior, &info, sizeof info);
    if (!strncmp(name, "hash-", 5U) || !strcmp(name, "nul-hash") || !strcmp(name, "native-boundary")) {
        uint64_t raw = 11U, normalized = 22U;
        size_t size = 33U;
        UmiStatus status = umi_codeguard_hash_file_bounded(path, maximum,
            &raw, &normalized, &size, token);
        if (!strcmp(name, "hash-over"))
            CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && raw == 11U && normalized == 22U && size == 33U);
        else {
            CHECK(status == UMI_STATUS_OK);
            uint64_t legacy_raw = 0U, legacy_normalized = 0U;
            size_t legacy_size = 0U;
            CHECK(umi_codeguard_hash_file(path, &legacy_raw, &legacy_normalized, &legacy_size) == UMI_STATUS_OK);
            CHECK(raw == legacy_raw && normalized == legacy_normalized && size == legacy_size);
            if (!strcmp(name, "nul-hash")) {
                CHECK(size == 3U && raw == umi_codeguard_hash_bytes("a\0b", 3U));
                CHECK(normalized == raw);
            }
            if (!strcmp(name, "native-boundary")) {
                CHECK(size == 65545U);
                /* Spaces and a block comment cross the 64 KiB native boundary.
                 * Only a+b remains in the normalized byte stream. */
                CHECK(normalized == umi_codeguard_hash_bytes("a+b", 3U));
            }
        }
    } else {
        UmiStatus status = umi_codeguard_source_measure_bounded(input_path, maximum, &info, token);
        if (!strcmp(name, "over") || !strcmp(name, "zero"))
            CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
        else if (!strcmp(name, "nul-text")) CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
        else if (!strcmp(name, "missing")) CHECK(status == UMI_STATUS_IO_ERROR);
        else if (!strcmp(name, "directory")) CHECK(status != UMI_STATUS_OK);
        else if (!strcmp(name, "cancel")) CHECK(status == UMI_STATUS_CANCELLED);
        else {
            CHECK(status == UMI_STATUS_OK && !strcmp(info.path, path));
            CHECK(info.include_count == 0U && info.function_hint_count == 0U);
            if (!strcmp(name, "empty")) CHECK(info.bytes == 0U && info.lines == 0U);
            else if (!strcmp(name, "long")) CHECK(info.bytes == 8194U && info.lines == 2U);
            else CHECK(info.bytes == 7U && info.lines == 1U);
        }
        if (status != UMI_STATUS_OK) CHECK(!memcmp(prior, &info, sizeof info));
    }
    umi_cancellation_token_destroy(token);
    return 0;
}
