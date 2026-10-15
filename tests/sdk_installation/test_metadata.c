/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_metadata.c
 * PURPOSE: Reject incomplete, ambiguous and unsafe installed SDK metadata.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/sdk/installed_metadata.h"
#include "umicom/sdk/package.h"
#include "umicom/sdk/installation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); free(metadata); return 1; \
} } while (0)
static const char *complete =
    "schema=umicom.sdk\nframework_version=7.12.3\nframework_abi=17\n"
    "include_directory=include\nlibrary_directory=lib\nbase_library=lib/libumicom_base.a\n";

/* Table-driven malformed values share the same complete envelope, so a refusal
 * tests the changed field rather than an unrelated missing-field condition. */
typedef struct InvalidValue { const char *name, *key, *value; UmiStatus status; } InvalidValue;
static const InvalidValue invalid[] = {
    {"schema", "schema", "foreign.sdk", UMI_STATUS_PARSE_ERROR},
    {"version_suffix", "framework_version", "7.12.3-preview", UMI_STATUS_PARSE_ERROR},
    {"version_parts", "framework_version", "7.12", UMI_STATUS_PARSE_ERROR},
    {"version_extra", "framework_version", "7.12.3.4", UMI_STATUS_PARSE_ERROR},
    {"version_empty", "framework_version", "7..3", UMI_STATUS_PARSE_ERROR},
    {"version_overflow", "framework_version", "65536.0.0", UMI_STATUS_CAPACITY_EXCEEDED},
    {"abi_zero", "framework_abi", "0", UMI_STATUS_PARSE_ERROR},
    {"abi_overflow", "framework_abi", "4294967296", UMI_STATUS_CAPACITY_EXCEEDED},
    {"abi_sign", "framework_abi", "-1", UMI_STATUS_PARSE_ERROR},
    {"absolute", "include_directory", "/include", UMI_STATUS_PARSE_ERROR},
    {"drive", "library_directory", "C:/lib", UMI_STATUS_PARSE_ERROR},
    {"parent", "base_library", "lib/../outside.a", UMI_STATUS_PARSE_ERROR},
    {"dot", "include_directory", "./include", UMI_STATUS_PARSE_ERROR},
    {"empty_segment", "library_directory", "lib//cpu", UMI_STATUS_PARSE_ERROR},
    {"trailing_separator", "library_directory", "lib/", UMI_STATUS_PARSE_ERROR},
    {"backslash", "library_directory", "lib\\cpu", UMI_STATUS_PARSE_ERROR},
    {"list", "library_directory", "lib;other", UMI_STATUS_PARSE_ERROR},
    {"expression", "base_library", "lib/$<TARGET_FILE_NAME:other>", UMI_STATUS_PARSE_ERROR},
    {"trailing_dot", "include_directory", "include.", UMI_STATUS_PARSE_ERROR}
};
int main(int argc, char **argv)
{
    UmiSdkInstalledMetadata *metadata = malloc(sizeof *metadata);
    CHECK(argc == 2 && metadata != NULL);
    const char *name = argv[1];
    char text[4096]; size_t length = strlen(complete);
    memcpy(text, complete, length + 1U);
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "null") == 0) {
        CHECK(UmiSdkInstalledMetadataParse(NULL, 1U, metadata) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiSdkInstalledMetadataParse(complete, 0U, metadata) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiSdkInstalledMetadataParse(complete, length, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiSdkInstalledMetadataLoad("relative", metadata) == UMI_STATUS_INVALID_ARGUMENT);
        free(metadata); return 0;
    }
    if (strcmp(name, "oversize") == 0) {
        CHECK(UmiSdkInstalledMetadataParse(complete, UMI_SDK_METADATA_BYTE_LIMIT + 1U,
            metadata) == UMI_STATUS_CAPACITY_EXCEEDED);
        free(metadata); return 0;
    }
    if (strcmp(name, "crlf") == 0) {
        size_t used = 0U;
        for (size_t i = 0U; i < strlen(complete); ++i) {
            if (complete[i] == '\n') text[used++] = '\r';
            text[used++] = complete[i];
        }
        text[used] = '\0'; length = used;
    } else if (strcmp(name, "duplicate") == 0 || strcmp(name, "unknown") == 0) {
        const char *extra = strcmp(name, "duplicate") == 0 ? "framework_abi=17\n" : "unreviewed=1\n";
        memcpy(text + length, extra, strlen(extra) + 1U); length += strlen(extra);
        expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "missing") == 0) {
        char *last = strstr(text, "base_library="); CHECK(last != NULL);
        *last = '\0'; length = strlen(text); expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "nul") == 0 || strcmp(name, "bare_cr") == 0) {
        text[8] = strcmp(name, "nul") == 0 ? '\0' : '\r'; expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(name, "maximum") == 0) {
        (void)snprintf(text, sizeof text,
            "schema=umicom.sdk\nframework_version=65535.65535.65535\nframework_abi=4294967295\n"
            "include_directory=public headers\nlibrary_directory=lib/cpu-linux\nbase_library=lib/cpu-linux/base.a\n");
        length = strlen(text);
    } else if (strcmp(name, "path_overflow") == 0) {
        char *path = strstr(text, "base_library="); CHECK(path != NULL);
        path += strlen("base_library=");
        memset(path, 'a', UMI_PATH_CAPACITY); path[UMI_PATH_CAPACITY] = '\0';
        length = strlen(text); expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else {
        int found = strcmp(name, "valid") == 0;
        for (size_t i = 0U; i < sizeof invalid / sizeof invalid[0]; ++i) {
            if (strcmp(name, invalid[i].name) != 0) continue;
            const char *start = strstr(complete, invalid[i].key);
            CHECK(start != NULL);
            const char *end = strchr(start, '\n'); CHECK(end != NULL);
            size_t before = (size_t)(start - complete);
            int wrote = snprintf(text + before, sizeof text - before, "%s=%s%s",
                invalid[i].key, invalid[i].value, end);
            CHECK(wrote >= 0 && (size_t)wrote < sizeof text - before);
            length = strlen(text); expected = invalid[i].status; found = 1;
        }
        CHECK(found);
    }
    memset(metadata, 0x5a, sizeof *metadata);
    CHECK(UmiSdkInstalledMetadataParse(text, length, metadata) == expected);
    if (expected == UMI_STATUS_OK) {
        CHECK(metadata->framework_abi == (strcmp(name, "maximum") == 0 ? UINT32_MAX : 17U));
        CHECK(metadata->framework_version.major == (strcmp(name, "maximum") == 0 ? UINT16_MAX : 7U));
    } else {
        const unsigned char *bytes = (const unsigned char *)metadata;
        for (size_t i = 0U; i < sizeof *metadata; ++i) CHECK(bytes[i] == 0U);
    }
    free(metadata);
    return 0;
}
