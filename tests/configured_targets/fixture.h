/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/configured_targets/fixture.h
 * PURPOSE: Create small CMake reply fixtures without invoking CMake or a compiler.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONFIGURED_TARGET_FIXTURE_H
#define UMICOM_CONFIGURED_TARGET_FIXTURE_H
#include "umicom/developer_project/target_catalogue.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static UmiStatus FixtureFile(const char *root, const char *leaf, const char *text)
{
    char path[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status = umi_path_join(root, leaf, path, sizeof(path));
    return status == UMI_STATUS_OK ? umi_fs_write_text(path, text) : status;
}
/* Encode environment-supplied temporary paths as JSON strings. The tests must
 * not accidentally depend on a machine having an ASCII path without spaces. */
static int FixtureQuote(const char *input, char *out, size_t capacity)
{
    size_t used = 0U;
    for (size_t i = 0U; input[i] != '\0'; ++i)
    {
        unsigned char c = (unsigned char)input[i];
        if (c < 0x20U || capacity - used < 3U)
            return 0;
        if (c == '"' || c == '\\')
            out[used++] = '\\';
        out[used++] = (char)c;
    }
    out[used] = '\0';
    return 1;
}
static UmiStatus FixtureReply(const char *root, const char *mode, char *build)
{
    char reply[UMI_BUILD_PATH_CAPACITY], sourceJson[UMI_BUILD_PATH_CAPACITY * 2U],
        buildJson[UMI_BUILD_PATH_CAPACITY * 2U];
    UmiStatus status = umi_path_join(root, "build", build, UMI_BUILD_PATH_CAPACITY);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(build, ".cmake/api/v1/reply", reply, sizeof(reply));
    if (status == UMI_STATUS_OK)
        status = umi_fs_make_directories(reply);
    if (status != UMI_STATUS_OK)
        return status;
    if (!FixtureQuote(root, sourceJson, sizeof(sourceJson)) ||
        !FixtureQuote(build, buildJson, sizeof(buildJson)))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *index = "{\"objects\":[{\"kind\":\"codemodel\",\"version\":{\"major\":2,\"minor\":0},"
                        "\"jsonFile\":\"model.json\"}]}";
    if (strcmp(mode, "syntax") == 0)
        index = "{\"objects\":[}";
    if (strcmp(mode, "traversal") == 0)
        index = "{\"objects\":[{\"kind\":\"codemodel\",\"version\":{\"major\":2,\"minor\":0},\"jsonFile\":\"."
                "./model.json\"}]}";
    if (strcmp(mode, "duplicate-field") == 0)
        index = "{\"objects\":[],\"obj\\u0065cts\":[]}";
    if (strcmp(mode, "no-model") == 0)
        index = "{\"objects\":[]}";
    if (strcmp(mode, "unsupported") == 0)
        index = "{\"objects\":[{\"kind\":\"codemodel\",\"version\":{\"major\":3,\"minor\":0},\"jsonFile\":"
                "\"model.json\"}]}";
    status = FixtureFile(reply, "index-200.json", index);
    if (status != UMI_STATUS_OK)
        return status;
    if (strcmp(mode, "latest") == 0)
        status = FixtureFile(reply, "index-100.json", "invalid older reply");
    if (strcmp(mode, "new-error") == 0)
        status = FixtureFile(reply, "error-300.json", "{}");
    if (strcmp(mode, "old-error") == 0)
        status = FixtureFile(reply, "error-100.json", "{}");
    if (status != UMI_STATUS_OK)
        return status;
    const char *targets = "{\"name\":\"notes\",\"id\":\"notes::test\",\"jsonFile\":\"target.json\"}";
    if (strcmp(mode, "empty") == 0)
        targets = "";
    if (strcmp(mode, "duplicate-target") == 0)
        targets = "{\"name\":\"notes\",\"id\":\"notes::test\",\"jsonFile\":\"target.json\"},{\"name\":"
                  "\"notes\",\"id\":\"other\",\"jsonFile\":\"target.json\"}";
    const char *configuration = strcmp(mode, "unnamed") == 0 ? "" : "Debug";
    const char *extra = strcmp(mode, "multi") == 0 ? ",{\"name\":\"Release\",\"targets\":[]}" : "";
    if (strcmp(mode, "duplicate-config") == 0)
        extra = ",{\"name\":\"Debug\",\"targets\":[]}";
    char model[16384];
    int length =
        snprintf(model, sizeof(model),
                 "{\"kind\":\"codemodel\",\"version\":{\"major\":2,\"minor\":0},\"paths\":{\"source\":\"%s%"
                 "s\",\"build\":\"%s%s\"},\"configurations\":[{\"name\":\"%s\",\"targets\":[%s]}%s]}",
                 sourceJson, strcmp(mode, "wrong-root") == 0 ? "/other" : "", buildJson,
                 strcmp(mode, "wrong-build") == 0 ? "/other" : "", configuration, targets, extra);
    if (length < 0 || (size_t)length >= sizeof(model))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(mode, "missing-model") != 0)
        status = FixtureFile(reply, "model.json", model);
    if (status != UMI_STATUS_OK)
        return status;
    const char *type = strcmp(mode, "library") == 0 ? "STATIC_LIBRARY" : "EXECUTABLE";
    const char *artifact = "{\"path\":\"bin/notes.exe\"},{\"path\":\"bin/notes.pdb\"}";
    if (strcmp(mode, "ambiguous") == 0)
        artifact = "{\"path\":\"one/notes.exe\"},{\"path\":\"two/notes.exe\"}";
    if (strcmp(mode, "no-artifact") == 0)
        artifact = "";
    if (strcmp(mode, "bad-artifact") == 0)
        artifact = "{\"path\":9}";
    if (strcmp(mode, "nul-artifact") == 0)
        artifact = "{\"path\":\"bin/notes\\u0000.exe\"}";
    const char *flags = strcmp(mode, "abstract") == 0 ? ",\"abstract\":true" : "";
    if (strcmp(mode, "launcher") == 0)
        flags = ",\"launchers\":[{\"type\":\"emulator\",\"command\":\"qemu\"}]";
    char target[2048];
    length = snprintf(target, sizeof(target),
                      "{\"name\":\"notes\",\"id\":\"%s\",\"type\":\"%s\",\"nameOnDisk\":\"notes.exe\","
                      "\"artifacts\":[%s]%s}",
                      strcmp(mode, "identity") == 0 ? "wrong" : "notes::test", type, artifact, flags);
    if (length < 0 || (size_t)length >= sizeof(target))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(mode, "missing-target") != 0)
        status = FixtureFile(reply, "target.json", target);
    if (strcmp(mode, "byte-limit") == 0)
    {
        char *large = malloc(UMI_PROJECT_TARGET_DOCUMENT_LIMIT + 2U);
        if (large == NULL)
            return UMI_STATUS_OUT_OF_MEMORY;
        memset(large, ' ', UMI_PROJECT_TARGET_DOCUMENT_LIMIT + 1U);
        large[UMI_PROJECT_TARGET_DOCUMENT_LIMIT + 1U] = '\0';
        status = FixtureFile(reply, "target.json", large);
        free(large);
    }
    return status;
}
#endif
