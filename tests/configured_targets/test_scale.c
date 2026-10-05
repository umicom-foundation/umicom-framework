/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/configured_targets/test_scale.c
 * PURPOSE: Verify large configured projects and duplicate reference rejection without invoking build tools.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    int failed = 0, owned = 0;
    char temporary[UMI_BUILD_PATH_CAPACITY], root[UMI_BUILD_PATH_CAPACITY], build[UMI_BUILD_PATH_CAPACITY],
        reply[UMI_BUILD_PATH_CAPACITY], leaf[160], sourceJson[UMI_BUILD_PATH_CAPACITY * 2U],
        buildJson[UMI_BUILD_PATH_CAPACITY * 2U];
    UmiProjectTargetCatalogue *catalogue = NULL;
    char *model = NULL, *target = NULL;
#ifdef _WIN32
    unsigned long process = (unsigned long)GetCurrentProcessId();
#else
    unsigned long process = (unsigned long)getpid();
#endif
    int largeTarget = strcmp(mode, "large-target") == 0;
    int duplicateName = strcmp(mode, "duplicate-name") == 0,
        duplicateIdentity = strcmp(mode, "duplicate-identity") == 0;
    CHECK(largeTarget || duplicateName || duplicateIdentity || strcmp(mode, "many-targets") == 0);
    int length = snprintf(leaf, sizeof(leaf), "umicom-target-scale-%lu-%s", process, mode);
    CHECK(length > 0 && (size_t)length < sizeof(leaf));
    CHECK(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    CHECK(umi_path_join(temporary, leaf, root, sizeof(root)) == UMI_STATUS_OK && !umi_fs_exists(root));
    CHECK(umi_fs_make_directories(root) == UMI_STATUS_OK);
    owned = 1;
    CHECK(FixtureReply(root, "valid", build) == UMI_STATUS_OK);
    CHECK(umi_path_join(build, ".cmake/api/v1/reply", reply, sizeof(reply)) == UMI_STATUS_OK);
    CHECK(FixtureQuote(root, sourceJson, sizeof(sourceJson)) &&
          FixtureQuote(build, buildJson, sizeof(buildJson)));
    size_t count = largeTarget ? 1U : 1200U, capacity = count * 200U + 20000U, used = 0U;
    model = malloc(capacity);
    target = malloc(150000U);
    CHECK(model != NULL && target != NULL);
    length = snprintf(model, capacity,
                      "{\"kind\":\"codemodel\",\"version\":{\"major\":2,\"minor\":0},\"paths\":{\"source\":"
                      "\"%s\",\"build\":\"%s\"},\"configurations\":[{\"name\":\"Debug\",\"targets\":[",
                      sourceJson, buildJson);
    CHECK(length > 0 && (size_t)length < capacity);
    used = (size_t)length;
    for (size_t i = 0U; i < count; ++i)
    {
        size_t nameIndex = duplicateName && i + 1U == count ? 0U : i,
               identityIndex = duplicateIdentity && i + 1U == count ? 0U : i;
        length = snprintf(model + used, capacity - used,
                          "%s{\"name\":\"program%zu\",\"id\":\"id%zu\",\"jsonFile\":\"target%zu.json\"}",
                          i == 0U ? "" : ",", nameIndex, identityIndex, i);
        CHECK(length > 0 && (size_t)length < capacity - used);
        used += (size_t)length;
        /* Duplicate references must fail before opening any referenced file.
         * Leave them absent so this checks failure precedence as well. */
        if (duplicateName || duplicateIdentity)
            continue;
        length = snprintf(target, 150000U,
                          "{\"name\":\"program%zu\",\"id\":\"id%zu\",\"type\":\"EXECUTABLE\",\"nameOnDisk\":"
                          "\"app.exe\",\"artifacts\":[{\"path\":\"bin/app.exe\"}]",
                          i, i);
        CHECK(length > 0 && (size_t)length < 150000U);
        size_t at = (size_t)length;
        if (largeTarget)
        {
            /* Uninterpreted metadata is still fully parsed and validated. */
            const char *prefix = ",\"sourceGroups\":[";
            memcpy(target + at, prefix, strlen(prefix));
            at += strlen(prefix);
            for (size_t j = 0U; j < 12000U; ++j)
            {
                memcpy(target + at, "\"source\"", 8U);
                at += 8U;
                target[at++] = j + 1U == 12000U ? ']' : ',';
            }
        }
        target[at++] = '}';
        target[at] = '\0';
        length = snprintf(leaf, sizeof(leaf), "target%zu.json", i);
        CHECK(length > 0 && (size_t)length < sizeof(leaf));
        CHECK(FixtureFile(reply, leaf, target) == UMI_STATUS_OK);
    }
    CHECK(capacity - used > 5U);
    memcpy(model + used, "]}]}", 5U);
    CHECK(FixtureFile(reply, "model.json", model) == UMI_STATUS_OK);
    UmiStatus status = UmiProjectTargetCatalogueRead(root, build, "Debug", &catalogue);
    if (duplicateName || duplicateIdentity)
        CHECK(status == UMI_STATUS_ALREADY_EXISTS && catalogue == NULL);
    else
    {
        CHECK(status == UMI_STATUS_OK);
        UmiProjectTargetSummary summary;
        CHECK(UmiProjectTargetCatalogueSummary(catalogue, &summary) == UMI_STATUS_OK &&
              summary.count == count);
        for (size_t i = 0U; i < count; ++i)
        {
            UmiProjectTargetChoice row;
            CHECK(UmiProjectTargetCatalogueAt(catalogue, i, &row) == UMI_STATUS_OK);
            length = snprintf(leaf, sizeof(leaf), "program%zu", i);
            CHECK(length > 0 && (size_t)length < sizeof(leaf));
            CHECK(strcmp(row.name, leaf) == 0 && row.executable && row.program[0] != '\0');
        }
    }
cleanup:
    free(model);
    free(target);
    UmiProjectTargetCatalogueDestroy(catalogue);
    if (owned && umi_fs_remove_tree(root) != UMI_STATUS_OK)
        failed = 1;
    return failed;
}
