/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_installation/test_artifact_inventory.c
 * PURPOSE: Reject incomplete SDK receipts and separate parsing from file evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/sdk/artifact_inventory.h"
#include "umicom/platform/native_arguments.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { (void)fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
#define RECEIPT "schema=umicom.sdk.artifacts/1\nconfiguration=Debug\n"
static const char good[] = RECEIPT "entries=3\n"
    "umicom_base|archive|lib/libumicom_base.a\n"
    "umicom_source_contracts|tool|bin/source-contracts.exe\n"
    "umicom_framework|interface|-\n";

static int Reject(const char *text)
{
    UmiSdkArtifactInventory *inventory = NULL;
    UmiStatus status = UmiSdkArtifactInventoryParse(text, strlen(text), &inventory);
    if (status == UMI_STATUS_OK) UmiSdkArtifactInventoryDestroy(inventory);
    return status != UMI_STATUS_OK && inventory == NULL;
}
/* Retained entry signature for review; the native argument owner supplies UTF-8 Windows paths before inspection. */
#if 0
int main(int argc, char **argv)
#endif
static int InspectMain(int argc, char **argv)
{
    CHECK(argc == 2 || argc == 4);
    const char *mode = argv[1];
    UmiSdkArtifactInventory *inventory = NULL;
    UmiSdkArtifactSummary summary;
    if (strcmp(mode, "inspect") == 0) {
        CHECK(argc == 4);
        UmiStatus status = UmiSdkArtifactInventoryInspect(argv[2], &inventory);
        if (strcmp(argv[3], "absent") == 0) {
            CHECK(status == UMI_STATUS_NOT_FOUND && inventory == NULL);
            return 0;
        }
        CHECK(status == UMI_STATUS_OK);
        CHECK(UmiSdkArtifactInventoryReadSummary(inventory, &summary) == UMI_STATUS_OK);
        CHECK(summary.inspected && summary.entries == 3U && summary.files == 2U);
        size_t missing = strcmp(argv[3], "complete") == 0 ? 0U : 1U;
        CHECK(summary.missing == missing && summary.present == 2U - missing);
        CHECK(UmiSdkArtifactInventoryEntryAt(inventory, 0U)->present);
        CHECK(UmiSdkArtifactInventoryEntryAt(inventory, 1U)->present == (missing == 0U));
    } else if (strcmp(mode, "valid") == 0 || strcmp(mode, "owned") == 0) {
        char mutable[sizeof(good)]; memcpy(mutable, good, sizeof(good));
        CHECK(UmiSdkArtifactInventoryParse(mutable, strlen(mutable), &inventory) == UMI_STATUS_OK);
        memset(mutable, 'x', sizeof(mutable));
        CHECK(UmiSdkArtifactInventoryReadSummary(inventory, &summary) == UMI_STATUS_OK);
        CHECK(summary.entries == 3U && summary.files == 2U && !summary.inspected);
        CHECK(summary.missing == 0U && summary.present == 0U && strcmp(summary.configuration, "Debug") == 0);
        CHECK(strcmp(UmiSdkArtifactInventoryEntryAt(inventory, 1U)->relative_path, "bin/source-contracts.exe") == 0);
        CHECK(UmiSdkArtifactInventoryEntryAt(inventory, 3U) == NULL);
    } else if (strcmp(mode, "crlf") == 0) {
        char crlf[sizeof(good) * 2U]; size_t used = 0U;
        for (size_t i = 0U; i < strlen(good); ++i) {
            if (good[i] == '\n') crlf[used++] = '\r';
            crlf[used++] = good[i];
        }
        CHECK(UmiSdkArtifactInventoryParse(crlf, used, &inventory) == UMI_STATUS_OK);
    } else if (strcmp(mode, "truncated") == 0) {
        /* Include record-boundary cuts: the declared total prevents a shorter
         * partial installation record from looking like an all-present SDK. */
        for (size_t i = 0U; i < strlen(good); ++i) {
            CHECK(UmiSdkArtifactInventoryParse(good, i, &inventory) != UMI_STATUS_OK);
            CHECK(inventory == NULL);
        }
    } else if (strcmp(mode, "nul") == 0 || strcmp(mode, "control") == 0) {
        char text[sizeof(good)]; memcpy(text, good, sizeof(good));
        text[60] = strcmp(mode, "nul") == 0 ? '\0' : '\x1b';
        CHECK(UmiSdkArtifactInventoryParse(text, strlen(good), &inventory) == UMI_STATUS_PARSE_ERROR);
        CHECK(inventory == NULL);
    } else if (strcmp(mode, "duplicates") == 0) {
        CHECK(Reject(RECEIPT "entries=2\na|archive|lib/a.a\na|archive|lib/b.a\n"));
        CHECK(Reject(RECEIPT "entries=2\na|archive|lib/a.a\na|tool|bin/a\n"));
        CHECK(Reject(RECEIPT "entries=3\na|shared|bin/a.dll\na|import|lib/a.dll.a\na|import|lib/b.a\n"));
    } else if (strcmp(mode, "shared-import") == 0) {
        const char text[] = RECEIPT "entries=2\na|shared|bin/a.dll\na|import|lib/a.dll.a\n";
        CHECK(UmiSdkArtifactInventoryParse(text, strlen(text), &inventory) == UMI_STATUS_OK);
    } else if (strcmp(mode, "paths") == 0) {
        const char *paths[] = {"/lib/a", "../a", "lib/../a", "lib/./a", "lib//a", "lib/a/",
            "C:/a", "lib\\a", "lib/a:stream", "$<CONFIG>/a", "lib/a;other", "lib/a.", "lib/a ", "lib/ a", ""};
        for (size_t i = 0U; i < sizeof(paths) / sizeof(paths[0]); ++i) {
            char text[1024];
            (void)snprintf(text, sizeof(text), RECEIPT "entries=1\na|archive|%s\n", paths[i]);
            CHECK(Reject(text));
        }
    } else if (strcmp(mode, "grammar") == 0) {
        CHECK(Reject("schema=unknown\nconfiguration=Debug\nentries=1\na|archive|lib/a\n"));
        CHECK(Reject(RECEIPT "entries=1\na|unknown|lib/a\n"));
        CHECK(Reject(RECEIPT "entries=1\n|archive|lib/a\n"));
        CHECK(Reject(RECEIPT "entries=1\na|archive|lib/a|extra\n"));
        CHECK(Reject(RECEIPT "entries=1\na|interface|lib/a\n"));
        CHECK(Reject("schema=umicom.sdk.artifacts/1\nconfiguration=$<CONFIG>\nentries=1\na|archive|lib/a\n"));
        CHECK(Reject(RECEIPT "entries=2\na|archive|lib/a\n"));
        CHECK(Reject(RECEIPT "entries=1\na|archive|lib/a\nb|archive|lib/b\n"));
        CHECK(Reject(RECEIPT "entries=999999999999999999\na|archive|lib/a\n"));
        CHECK(Reject(RECEIPT "entries=0\n"));
        CHECK(Reject(RECEIPT "entries=1\na|archive|lib/a\r"));
    } else if (strcmp(mode, "capacity") == 0) {
        CHECK(UmiSdkArtifactInventoryParse(good, UMI_SDK_ARTIFACT_BYTE_LIMIT + 1U, &inventory) == UMI_STATUS_CAPACITY_EXCEEDED);
        char text[2048], large[UMI_SDK_ARTIFACT_PATH_CAPACITY + 1U];
        memset(large, 'a', sizeof(large) - 1U); large[sizeof(large) - 1U] = '\0';
        (void)snprintf(text, sizeof(text), RECEIPT "entries=1\na|archive|%s\n", large);
        CHECK(Reject(text));
        (void)snprintf(text, sizeof(text), RECEIPT "entries=1\n%s|archive|lib/a\n", large);
        CHECK(Reject(text));
    } else if (strcmp(mode, "record-limit") == 0) {
        size_t capacity = 300000U, used;
        char *text = malloc(capacity);
        CHECK(text != NULL);
        int written = snprintf(text, capacity, RECEIPT "entries=%u\n", UMI_SDK_ARTIFACT_COUNT_LIMIT);
        CHECK(written > 0); used = (size_t)written;
        for (unsigned i = 0U; i < UMI_SDK_ARTIFACT_COUNT_LIMIT; ++i) {
            written = snprintf(text + used, capacity - used, "t%u|archive|lib/a%u.a\n", i, i);
            CHECK(written > 0 && (size_t)written < capacity - used); used += (size_t)written;
        }
        CHECK(UmiSdkArtifactInventoryParse(text, used, &inventory) == UMI_STATUS_OK);
        CHECK(UmiSdkArtifactInventoryReadSummary(inventory, &summary) == UMI_STATUS_OK);
        CHECK(summary.entries == UMI_SDK_ARTIFACT_COUNT_LIMIT);
        UmiSdkArtifactInventoryDestroy(inventory); inventory = NULL;
        written = snprintf(text + used, capacity - used, "overflow|archive|lib/overflow.a\n");
        CHECK(written > 0 && (size_t)written < capacity - used); used += (size_t)written;
        CHECK(UmiSdkArtifactInventoryParse(text, used, &inventory) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(inventory == NULL);
        free(text);
    } else if (strcmp(mode, "invalid") == 0) {
        CHECK(UmiSdkArtifactInventoryParse(NULL, 1U, &inventory) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiSdkArtifactInventoryParse(good, strlen(good), NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiSdkArtifactInventoryInspect("relative", &inventory) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiSdkArtifactInventoryInspect(NULL, &inventory) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiSdkArtifactInventoryReadSummary(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(summary.entries == 0U && !summary.inspected);
        CHECK(UmiSdkArtifactInventoryReadSummary(NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiSdkArtifactInventoryEntryAt(NULL, 0U) == NULL);
    } else { CHECK(!"unknown case"); }
    UmiSdkArtifactInventoryDestroy(inventory);
    UmiSdkArtifactInventoryDestroy(NULL);
    return 0;
}

/* Use the same Unicode argument boundary as installed native tools. */
int main(int argc, char **argv)
{
    int result = 2;
    UmiStatus status = UmiNativeArgumentsDispatch(argc, argv, InspectMain, &result);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Native argument capture failed: %d\n", (int)status);
        return 2;
    }
    return result;
}
