/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_clean_cache_identity.c
 *
 * PURPOSE:
 *   Verify clean refuses ambiguous or mismatched cache identities before removing files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/build_execution.h"
#include "umicom/toolchain/cmake_cache.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/clock.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char *mode;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s line %d: %s\n", mode, __LINE__, #x); failed = 1; goto finish; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    mode = argv[1];
    const char *cases[] = {"valid", "quoted", "bom", "crlf", "missing-source",
        "missing-build", "empty-source", "empty-build", "wrong-source", "wrong-build",
        "duplicate-source", "duplicate-build", "wrong-type", "invalid-utf8", "nul",
        "oversized", "cache-directory", "missing-cache", "preview"};
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(mode, cases[i])) known = 1;
    if (!known) return 2;
    int failed = 0, owns = 0;
    char temp[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY] = {0}, binary[UMI_PATH_CAPACITY];
    char cache_path[UMI_PATH_CAPACITY], marker[UMI_PATH_CAPACITY], leaf[160];
    char content[2U * UMI_PATH_CAPACITY + 512U];
    unsigned char *large = NULL, *readback = NULL;
    size_t content_size = 0U, read_size = 0U;
    UmiBuildReport *report = calloc(1U, sizeof *report);
    CHECK(report != NULL);
    UmiClock clock = umi_clock_system();
    int n = snprintf(leaf, sizeof leaf, "umicom-clean-identity-%s-%" PRIu64,
        mode, clock.monotonic_nanoseconds(&clock));
    umi_clock_dispose(&clock);
    CHECK(n > 0 && (size_t)n < sizeof leaf);
    CHECK(umi_fs_temp_directory(temp, sizeof temp) == UMI_STATUS_OK);
    CHECK(umi_path_join(temp, leaf, root, sizeof root) == UMI_STATUS_OK);
    CHECK(umi_path_is_within(temp, root) && !umi_path_equal(temp, root) && !umi_fs_exists(root));
    CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK); owns = 1;
    CHECK(umi_path_join(root, "build", binary, sizeof binary) == UMI_STATUS_OK);
    CHECK(umi_path_join(binary, "CMakeCache.txt", cache_path, sizeof cache_path) == UMI_STATUS_OK);
    CHECK(umi_path_join(binary, "owned.txt", marker, sizeof marker) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(binary) == UMI_STATUS_OK);
    CHECK(umi_fs_write_text(marker, "preserve on admission failure") == UMI_STATUS_OK);
    const char *source = !strcmp(mode, "empty-source") ? "" : !strcmp(mode, "wrong-source") ? binary : root;
    const char *build = !strcmp(mode, "empty-build") ? "" : !strcmp(mode, "wrong-build") ? root : binary;
    const char *newline = !strcmp(mode, "crlf") ? "\r\n" : "\n";
    const char *source_key = !strcmp(mode, "quoted") ? "\"CMAKE_HOME_DIRECTORY\"" : "CMAKE_HOME_DIRECTORY";
    const char *build_key = !strcmp(mode, "quoted") ? "\"CMAKE_CACHEFILE_DIR\"" : "CMAKE_CACHEFILE_DIR";
    n = snprintf(content, sizeof content, "%s%s:%s=%s%s%s:INTERNAL=%s%s",
        !strcmp(mode, "bom") ? "\xef\xbb\xbf" : "", source_key,
        !strcmp(mode, "wrong-type") ? "BOOL" : "INTERNAL", source, newline, build_key, build, newline);
    CHECK(n > 0 && (size_t)n < sizeof content); content_size = (size_t)n;
    if (!strcmp(mode, "missing-source") || !strcmp(mode, "missing-build")) {
        n = snprintf(content, sizeof content, "%s:INTERNAL=%s\n",
            !strcmp(mode, "missing-source") ? build_key : source_key,
            !strcmp(mode, "missing-source") ? build : source);
        CHECK(n > 0 && (size_t)n < sizeof content); content_size = (size_t)n;
    } else if (!strcmp(mode, "duplicate-source") || !strcmp(mode, "duplicate-build")) {
        n = snprintf(content + content_size, sizeof content - content_size, "%s:INTERNAL=%s\n",
            !strcmp(mode, "duplicate-source") ? source_key : build_key,
            !strcmp(mode, "duplicate-source") ? source : build);
        CHECK(n > 0 && (size_t)n < sizeof content - content_size); content_size += (size_t)n;
    } else if (!strcmp(mode, "invalid-utf8")) {
        content[content_size++] = (char)0xff;
    } else if (!strcmp(mode, "nul")) {
        content[content_size++] = '\0';
    }
    const void *bytes = content;
    if (!strcmp(mode, "oversized")) {
        content_size = UMI_CMAKE_CACHE_BYTE_LIMIT + 1U;
        large = malloc(content_size); CHECK(large != NULL);
        memset(large, '#', content_size); bytes = large;
    }
    if (!strcmp(mode, "cache-directory")) CHECK(umi_fs_make_directories(cache_path) == UMI_STATUS_OK);
    else if (strcmp(mode, "missing-cache"))
        CHECK(umi_fs_write_bytes(cache_path, bytes, content_size) == UMI_STATUS_OK);
    static UmiToolchainProfile profile; umi_toolchain_profile_init(&profile);
    UmiBuildRequest request; umi_build_request_init(&request);
    request.source_root = root; request.build_directory = binary;
    UmiBuildExecutionOptions options = {0}; options.preview = !strcmp(mode, "preview");
    UmiStatus status = UmiBuildExecuteWithOptions(&profile, NULL,
        UMI_BUILD_CLEAN, &request, &options, report);
    int removes = !strcmp(mode, "valid") || !strcmp(mode, "quoted") ||
        !strcmp(mode, "bom") || !strcmp(mode, "crlf");
    CHECK(report->last_action == UMI_BUILD_CLEAN && report->last_status == status);
    CHECK(report->configure_exit_code == -1 && report->build_exit_code == -1 &&
        report->test_exit_code == -1 && report->run_exit_code == -1);
    CHECK(umi_fs_is_directory(root));
    if (removes) CHECK(status == UMI_STATUS_OK && !umi_fs_exists(binary));
    else {
        CHECK(umi_fs_is_directory(binary) && umi_fs_exists(marker));
        if (options.preview) CHECK(status == UMI_STATUS_OK);
        else CHECK(status != UMI_STATUS_OK);
        if (strcmp(mode, "cache-directory") && strcmp(mode, "missing-cache")) {
            CHECK(umi_fs_read_bytes(cache_path, &readback, &read_size) == UMI_STATUS_OK);
            CHECK(read_size == content_size && !memcmp(readback, bytes, content_size));
        }
    }
finish:
    free(readback); free(large); free(report);
    /* Cleanup is restricted to the newly owned, non-root temporary child. */
    if (owns && umi_path_is_within(temp, root) && !umi_path_equal(temp, root))
        if (umi_fs_remove_tree(root) != UMI_STATUS_OK) failed = 1;
    return failed;
}
