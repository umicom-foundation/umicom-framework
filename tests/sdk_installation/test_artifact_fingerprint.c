/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_artifact_fingerprint.c
 *
 * PURPOSE:
 *   Check receipt-bound content identity, relocation, byte limits and fail-closed native file observation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/sdk/artifact_fingerprint.h"
#include "umicom/platform/filesystem.h"
#include <stdint.h>

#define RECEIPT "schema=umicom.sdk.artifacts/1\nconfiguration=Debug\nentries=3\n" \
    "a|archive|lib/a.a\ntool|tool|bin/sdk-tool.exe\napi|interface|-\n"
static const char receipt[] = RECEIPT;
static const char expected[] = "beab3af1b5a6e91196243c1bf9a86a6a0117d74a7874ce7fe581493e6f463c9b";
static const char expected_receipt[] = "9c667c3120a9a7f8a86783a6598bcaf4ec99108f3fc9f0b14ca072cf0e99d8ed";

static void WriteBytes(const char *path, const void *bytes, size_t length)
{
    CHECK(umi_fs_write_bytes(path, bytes, length) == UMI_STATUS_OK);
}
static void MakeSdk(const char *root, const char *leaf, char *prefix)
{
    char path[UMI_PATH_CAPACITY];
    FixturePath(prefix, root, leaf);
    FixturePath(path, prefix, "share/umicom/framework");
    CHECK(umi_fs_make_directories(path) == UMI_STATUS_OK);
    FixturePath(path, prefix, "lib");
    CHECK(umi_fs_make_directories(path) == UMI_STATUS_OK);
    FixturePath(path, prefix, "bin");
    CHECK(umi_fs_make_directories(path) == UMI_STATUS_OK);
    FixturePath(path, prefix, "share/umicom/framework/sdk-artifacts.txt");
    WriteBytes(path, receipt, sizeof receipt - 1U);
    FixturePath(path, prefix, "lib/a.a"); WriteBytes(path, "abc", 3U);
    FixturePath(path, prefix, "bin/sdk-tool.exe"); WriteBytes(path, "tool bytes\n", 11U);
}
static void ReceiptText(const char *prefix, const char *text)
{
    char path[UMI_PATH_CAPACITY];
    FixturePath(path, prefix, "share/umicom/framework/sdk-artifacts.txt");
    WriteBytes(path, text, strlen(text));
}
static void EmptyOutput(const UmiSdkFingerprint *out)
{
    const unsigned char *p = (const unsigned char *)out;
    for (size_t i = 0U; i < sizeof *out; ++i) CHECK(p[i] == 0U);
}
static void ReadGood(const char *prefix, UmiSdkFingerprint *out)
{
    CHECK(UmiSdkArtifactFingerprint(prefix, NULL, NULL, out) == UMI_STATUS_OK);
    CHECK(out->records == 3U && out->files == 2U && out->bytes == 14U);
    CHECK(strcmp(out->configuration, "Debug") == 0);
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], prefix[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    MakeSdk(root, strcmp(mode, "unicode") == 0 ? "SDK \xce\xa9" : "SDK", prefix);
    UmiSdkFingerprint out, first;
    memset(&out, 0xa5, sizeof out);
    UmiSdkFingerprintLimits limits = {11U, 14U};
    if (strcmp(mode, "vector") == 0 || strcmp(mode, "unicode") == 0) {
        /* This fixed preimage was calculated independently, not with the
         * production hash/framing routines. It locks down version-1 bytes. */
        ReadGood(prefix, &out);
        CHECK(strcmp(out.sha256, expected) == 0);
        CHECK(strcmp(out.receipt_sha256, expected_receipt) == 0);
    } else if (strcmp(mode, "relocated") == 0) {
        char second[UMI_PATH_CAPACITY];
        MakeSdk(root, "another SDK", second);
        ReadGood(prefix, &first); ReadGood(second, &out);
        CHECK(strcmp(first.sha256, out.sha256) == 0);
    } else if (strcmp(mode, "same-size") == 0 || strcmp(mode, "content-change") == 0) {
        ReadGood(prefix, &first);
        FixturePath(path, prefix, "lib/a.a");
        const char *changed = strcmp(mode, "same-size") == 0 ? "abd" : "longer archive";
        WriteBytes(path, changed, strlen(changed));
        CHECK(UmiSdkArtifactFingerprint(prefix, NULL, NULL, &out) == UMI_STATUS_OK);
        CHECK(strcmp(first.sha256, out.sha256) != 0);
        CHECK(strcmp(first.receipt_sha256, out.receipt_sha256) == 0);
    } else if (strcmp(mode, "unlisted") == 0) {
        FixturePath(path, prefix, "unlisted.h"); WriteBytes(path, "not covered", 11U);
        ReadGood(prefix, &out); CHECK(strcmp(out.sha256, expected) == 0);
    } else if (strcmp(mode, "empty") == 0 || strcmp(mode, "zero-budget-empty") == 0) {
        FixturePath(path, prefix, "lib/a.a"); WriteBytes(path, "", 0U);
        FixturePath(path, prefix, "bin/sdk-tool.exe"); WriteBytes(path, "", 0U);
        limits.maximum_file_bytes = 0U; limits.maximum_total_bytes = 0U;
        CHECK(UmiSdkArtifactFingerprint(prefix,
            strcmp(mode, "empty") == 0 ? NULL : &limits, NULL, &out) == UMI_STATUS_OK);
        CHECK(out.bytes == 0U && out.files == 2U && out.sha256[0] != '\0');
    } else if (strcmp(mode, "multichunk") == 0) {
        unsigned char *large = malloc(131077U); CHECK(large != NULL);
        for (size_t i = 0U; i < 131077U; ++i) large[i] = (unsigned char)(i % 251U);
        FixturePath(path, prefix, "lib/a.a"); WriteBytes(path, large, 131077U); free(large);
        CHECK(UmiSdkArtifactFingerprint(prefix, NULL, NULL, &out) == UMI_STATUS_OK);
        CHECK(out.bytes == 131088U && strcmp(out.sha256, "2e85642e70d6c8cb94af1376b64a82f9ce1062fa5e8fd5ee752ce2c1093f0df3") == 0);
    } else if (strcmp(mode, "receipt-crlf") == 0) {
        char crlf[sizeof receipt * 2U]; size_t used = 0U;
        for (size_t i = 0U; i < sizeof receipt - 1U; ++i) {
            if (receipt[i] == '\n') crlf[used++] = '\r';
            crlf[used++] = receipt[i];
        }
        crlf[used] = '\0'; ReceiptText(prefix, crlf);
        ReadGood(prefix, &out);
        CHECK(strcmp(out.sha256, expected) != 0 && strcmp(out.receipt_sha256, expected_receipt) != 0);
    } else if (strcmp(mode, "record-order") == 0) {
        ReceiptText(prefix, "schema=umicom.sdk.artifacts/1\nconfiguration=Debug\nentries=3\n"
            "tool|tool|bin/sdk-tool.exe\na|archive|lib/a.a\napi|interface|-\n");
        ReadGood(prefix, &out); CHECK(strcmp(out.sha256, expected) != 0);
    } else if (strcmp(mode, "configuration") == 0) {
        ReceiptText(prefix, "schema=umicom.sdk.artifacts/1\nconfiguration=Release\nentries=3\n"
            "a|archive|lib/a.a\ntool|tool|bin/sdk-tool.exe\napi|interface|-\n");
        CHECK(UmiSdkArtifactFingerprint(prefix, NULL, NULL, &out) == UMI_STATUS_OK);
        CHECK(strcmp(out.configuration, "Release") == 0 && strcmp(out.sha256, expected) != 0);
    } else if (strcmp(mode, "renamed-artifact") == 0) {
        FixturePath(path, prefix, "lib/renamed.a"); WriteBytes(path, "abc", 3U);
        ReceiptText(prefix, "schema=umicom.sdk.artifacts/1\nconfiguration=Debug\nentries=3\n"
            "a|archive|lib/renamed.a\ntool|tool|bin/sdk-tool.exe\napi|interface|-\n");
        ReadGood(prefix, &out); CHECK(strcmp(out.sha256, expected) != 0);
    } else if (strcmp(mode, "shared-import") == 0 || strcmp(mode, "repeat-path") == 0) {
        ReceiptText(prefix, strcmp(mode, "shared-import") == 0 ?
            "schema=umicom.sdk.artifacts/1\nconfiguration=Debug\nentries=2\n"
            "a|shared|lib/a.a\na|import|bin/sdk-tool.exe\n" :
            "schema=umicom.sdk.artifacts/1\nconfiguration=Debug\nentries=2\n"
            "a|archive|lib/a.a\nb|archive|lib/a.a\n");
        CHECK(UmiSdkArtifactFingerprint(prefix, NULL, NULL, &out) == UMI_STATUS_OK);
        CHECK(out.files == 2U && out.bytes == (strcmp(mode, "repeat-path") == 0 ? 6U : 14U));
    } else if (strcmp(mode, "interfaces-only") == 0) {
        ReceiptText(prefix, "schema=umicom.sdk.artifacts/1\nconfiguration=Debug\nentries=1\napi|interface|-\n");
        CHECK(UmiSdkArtifactFingerprint(prefix, NULL, NULL, &out) == UMI_STATUS_INVALID_STATE);
        EmptyOutput(&out);
    } else if (strcmp(mode, "malformed") == 0 || strcmp(mode, "traversal") == 0 ||
               strcmp(mode, "duplicate-targets") == 0) {
        ReceiptText(prefix, strcmp(mode, "malformed") == 0 ? "schema=unknown\n" :
            strcmp(mode, "traversal") == 0 ?
            "schema=umicom.sdk.artifacts/1\nconfiguration=Debug\nentries=1\na|archive|../outside.a\n" :
            "schema=umicom.sdk.artifacts/1\nconfiguration=Debug\nentries=2\na|archive|lib/a.a\na|archive|lib/a.a\n");
        CHECK(UmiSdkArtifactFingerprint(prefix, NULL, NULL, &out) == UMI_STATUS_PARSE_ERROR);
        EmptyOutput(&out);
    } else if (strcmp(mode, "missing") == 0 || strcmp(mode, "reset-on-failure") == 0 ||
               strcmp(mode, "leaf-directory") == 0 || strcmp(mode, "leaf-symlink") == 0) {
        if (strcmp(mode, "reset-on-failure") == 0) ReadGood(prefix, &out);
        char old[UMI_PATH_CAPACITY]; FixturePath(old, prefix, "lib/a.a");
        FixturePath(path, prefix, "saved.a"); CHECK(umi_fs_rename(old, path) == UMI_STATUS_OK);
        if (strcmp(mode, "leaf-directory") == 0) CHECK(umi_fs_make_directories(old) == UMI_STATUS_OK);
        if (strcmp(mode, "leaf-symlink") == 0) {
#ifdef _WIN32
            if (!FixtureSymbolicLink(old, path, 0)) {
                (void)fputs("SKIP: symbolic-link fixture requires Windows link capability.\n", stderr); return 77;
            }
#else
            CHECK(symlink(path, old) == 0);
#endif
        }
        CHECK(UmiSdkArtifactFingerprint(prefix, NULL, NULL, &out) != UMI_STATUS_OK);
        EmptyOutput(&out);
    } else if (strcmp(mode, "absent") == 0 || strcmp(mode, "receipt-directory") == 0 ||
               strcmp(mode, "receipt-symlink") == 0) {
        char old[UMI_PATH_CAPACITY]; FixturePath(old, prefix, "share/umicom/framework/sdk-artifacts.txt");
        FixturePath(path, prefix, "saved-receipt"); CHECK(umi_fs_rename(old, path) == UMI_STATUS_OK);
        if (strcmp(mode, "receipt-directory") == 0) CHECK(umi_fs_make_directories(old) == UMI_STATUS_OK);
        if (strcmp(mode, "receipt-symlink") == 0) {
#ifdef _WIN32
            if (!FixtureSymbolicLink(old, path, 0)) {
                (void)fputs("SKIP: symbolic-link fixture requires Windows link capability.\n", stderr); return 77;
            }
#else
            CHECK(symlink(path, old) == 0);
#endif
        }
        CHECK(UmiSdkArtifactFingerprint(prefix, NULL, NULL, &out) != UMI_STATUS_OK);
        EmptyOutput(&out);
    } else if (strcmp(mode, "limit-file") == 0 || strcmp(mode, "limit-total") == 0 ||
               strcmp(mode, "exact-budget") == 0 || strcmp(mode, "zero-budget") == 0) {
        if (strcmp(mode, "limit-file") == 0) limits.maximum_file_bytes = 10U;
        if (strcmp(mode, "limit-total") == 0) limits.maximum_total_bytes = 13U;
        if (strcmp(mode, "zero-budget") == 0) limits.maximum_total_bytes = 0U;
        UmiStatus status = UmiSdkArtifactFingerprint(prefix, &limits, NULL, &out);
        if (strcmp(mode, "exact-budget") == 0) CHECK(status == UMI_STATUS_OK && strcmp(out.sha256, expected) == 0);
        else { CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED); EmptyOutput(&out); }
    } else if (strcmp(mode, "cancelled") == 0) {
        UmiCancellationToken *token = NULL; CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
        umi_cancellation_token_request(token);
        CHECK(UmiSdkArtifactFingerprint(prefix, NULL, token, &out) == UMI_STATUS_CANCELLED);
        EmptyOutput(&out);
        umi_cancellation_token_reset(token);
        CHECK(UmiSdkArtifactFingerprint(prefix, NULL, token, &out) == UMI_STATUS_OK);
        CHECK(strcmp(out.sha256, expected) == 0);
        umi_cancellation_token_destroy(token);
    } else if (strcmp(mode, "arguments") == 0 || strcmp(mode, "limits-overflow") == 0) {
        CHECK(UmiSdkArtifactFingerprint(NULL, NULL, NULL, &out) == UMI_STATUS_INVALID_ARGUMENT); EmptyOutput(&out);
        CHECK(UmiSdkArtifactFingerprint("relative", NULL, NULL, &out) == UMI_STATUS_INVALID_ARGUMENT); EmptyOutput(&out);
        CHECK(UmiSdkArtifactFingerprint(prefix, NULL, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        limits.maximum_file_bytes = UMI_SDK_FINGERPRINT_FILE_LIMIT + 1U;
        CHECK(UmiSdkArtifactFingerprint(prefix, &limits, NULL, &out) == UMI_STATUS_INVALID_ARGUMENT); EmptyOutput(&out);
        limits.maximum_file_bytes = 0U; limits.maximum_total_bytes = UINT64_MAX;
        CHECK(UmiSdkArtifactFingerprint(prefix, &limits, NULL, &out) == UMI_STATUS_INVALID_ARGUMENT); EmptyOutput(&out);
    } else return 2;
    return 0;
}
