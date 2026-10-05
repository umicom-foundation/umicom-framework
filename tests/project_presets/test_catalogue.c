/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_presets/test_catalogue.c
 * PURPOSE: Exercise declared preset boundaries and non-executing profile selection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/preset_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(expression)                                                                                    \
    do                                                                                                       \
    {                                                                                                        \
        if (!(expression))                                                                                   \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression);                                 \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)

typedef struct InvalidDocument
{
    const char *name, *json;
    UmiStatus status;
} InvalidDocument;
/* Each malformed document isolates a boundary that must not publish a partial
 * catalogue. Unknown vendor data is allowed only if the whole JSON is valid. */
static const InvalidDocument invalid[] = {
    {"syntax", "{\"version\":3,}", UMI_STATUS_PARSE_ERROR},
    {"trailing", "{\"version\":3} false", UMI_STATUS_PARSE_ERROR},
    {"root", "[]", UMI_STATUS_PARSE_ERROR},
    {"missing-version", "{}", UMI_STATUS_PARSE_ERROR},
    {"fraction-version", "{\"version\":1.5}", UMI_STATUS_PARSE_ERROR},
    {"overflow-version", "{\"version\":99999999999999999999999}", UMI_STATUS_PARSE_ERROR},
    {"duplicate-field", "{\"version\":3,\"vers\\u0069on\":3}", UMI_STATUS_PARSE_ERROR},
    {"group-type", "{\"version\":3,\"buildPresets\":{}}", UMI_STATUS_PARSE_ERROR},
    {"row-type", "{\"version\":3,\"buildPresets\":[null]}", UMI_STATUS_PARSE_ERROR},
    {"missing-name", "{\"version\":3,\"buildPresets\":[{}]}", UMI_STATUS_PARSE_ERROR},
    {"empty-name", "{\"version\":3,\"buildPresets\":[{\"name\":\"\"}]}", UMI_STATUS_PARSE_ERROR},
    {"duplicate-name-field", "{\"version\":3,\"buildPresets\":[{\"name\":\"a\",\"na\\u006de\":\"b\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"duplicate-name",
     "{\"version\":3,\"buildPresets\":[{\"name\":\"same\"},{\"name\":\"same\",\"hidden\":true}]}",
     UMI_STATUS_ALREADY_EXISTS},
    {"control-name", "{\"version\":3,\"buildPresets\":[{\"name\":\"a\\nb\"}]}", UMI_STATUS_PARSE_ERROR},
    {"nul-name", "{\"version\":3,\"buildPresets\":[{\"name\":\"a\\u0000b\"}]}", UMI_STATUS_PARSE_ERROR},
    {"surrogate", "{\"version\":3,\"buildPresets\":[{\"name\":\"\\ud800\"}]}", UMI_STATUS_PARSE_ERROR},
    {"hidden-type", "{\"version\":3,\"buildPresets\":[{\"name\":\"a\",\"hidden\":1}]}",
     UMI_STATUS_PARSE_ERROR},
    {"condition-type", "{\"version\":3,\"buildPresets\":[{\"name\":\"a\",\"condition\":\"false\"}]}",
     UMI_STATUS_PARSE_ERROR},
    {"inherit-type", "{\"version\":3,\"buildPresets\":[{\"name\":\"a\",\"inherits\":[1]}]}",
     UMI_STATUS_PARSE_ERROR},
    {"include-type", "{\"version\":3,\"include\":\"other.json\"}", UMI_STATUS_PARSE_ERROR},
    {"reference-type", "{\"version\":3,\"buildPresets\":[{\"name\":\"a\",\"configurePreset\":false}]}",
     UMI_STATUS_PARSE_ERROR},
    {"metadata-type", "{\"version\":3,\"buildPresets\":[{\"name\":\"a\",\"description\":[]}]}",
     UMI_STATUS_PARSE_ERROR},
    {"unknown-invalid", "{\"version\":3,\"vendor\":{\"x\":[1,]}}", UMI_STATUS_PARSE_ERROR}};
static const char declarations[] =
    "{\"version\":3,\"configurePresets\":[{\"name\":\"notes "
    "configure\",\"binaryDir\":\"${sourceDir}/build/"
    "notes\"}],\"buildPresets\":[{\"name\":\"notes-build\",\"configurePreset\":\"notes "
    "configure\"}],\"testPresets\":[{\"name\":\"notes-test\"}]}";
int main(int argc, char **argv)
{
    int failed = 0;
    UmiProjectPresetCatalogue *catalogue = NULL;
    char *owned = NULL;
    UmiProjectPresetChoice choice;
    UmiProjectPresetSummary summary;
    UmiBuildProfile profile, output, expected;
    umi_build_profile_init(&profile);
    CHECK(argc == 2);
    const char *mode = argv[1];
    for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
    {
        if (strcmp(mode, invalid[i].name) == 0)
        {
            CHECK(UmiProjectPresetCatalogueCreate(invalid[i].json, strlen(invalid[i].json), NULL, 0U,
                                                  &catalogue) == invalid[i].status);
            CHECK(catalogue == NULL);
            goto cleanup;
        }
    }
    if (strcmp(mode, "absent") == 0)
    {
        CHECK(UmiProjectPresetCatalogueCreate(NULL, 0U, NULL, 0U, &catalogue) == UMI_STATUS_NOT_FOUND);
        CHECK(catalogue == NULL);
    }
    else if (strcmp(mode, "arguments") == 0)
    {
        CHECK(UmiProjectPresetCatalogueCreate(NULL, 1U, NULL, 0U, &catalogue) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProjectPresetCatalogueCreate("", 0U, NULL, 0U, &catalogue) == UMI_STATUS_PARSE_ERROR);
        CHECK(UmiProjectPresetCatalogueRead(NULL, &catalogue) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProjectPresetCatalogueRead("relative/project", &catalogue) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProjectPresetCatalogueCreate(declarations, strlen(declarations), NULL, 0U, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "embedded-nul") == 0)
    {
        const char nul[] = "{\"version\":3}\0 ";
        CHECK(UmiProjectPresetCatalogueCreate(nul, sizeof(nul) - 1U, NULL, 0U, &catalogue) ==
              UMI_STATUS_PARSE_ERROR);
    }
    else if (strcmp(mode, "byte-limit") == 0)
    {
        /* Rejection happens before reading beyond this small fixture. */
        CHECK(UmiProjectPresetCatalogueCreate("{}", UMI_PROJECT_PRESET_FILE_LIMIT + 1U, NULL, 0U,
                                              &catalogue) == UMI_STATUS_CAPACITY_EXCEEDED);
    }
    else if (strcmp(mode, "name-limit") == 0)
    {
        char name[UMI_BUILD_NAME_CAPACITY + 1U], document[512];
        memset(name, 'n', sizeof(name) - 1U);
        name[sizeof(name) - 1U] = '\0';
        (void)snprintf(document, sizeof(document), "{\"version\":3,\"buildPresets\":[{\"name\":\"%s\"}]}",
                       name);
        CHECK(UmiProjectPresetCatalogueCreate(document, strlen(document), NULL, 0U, &catalogue) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
    }
    else if (strcmp(mode, "row-limit") == 0)
    {
        owned = calloc(1U, 8192U);
        CHECK(owned != NULL);
        size_t used = (size_t)snprintf(owned, 8192U, "{\"version\":3,\"buildPresets\":[");
        for (size_t i = 0U; i <= UMI_PROJECT_PRESET_LIMIT; ++i)
            used += (size_t)snprintf(owned + used, 8192U - used, "%s{\"name\":\"build-%zu\"}",
                                     i == 0U ? "" : ",", i);
        (void)snprintf(owned + used, 8192U - used, "]}");
        CHECK(UmiProjectPresetCatalogueCreate(owned, strlen(owned), NULL, 0U, &catalogue) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
    }
    else if (strcmp(mode, "cross-file") == 0 || strcmp(mode, "malformed-user") == 0)
    {
        const char *user = strcmp(mode, "cross-file") == 0 ? declarations : "{";
        UmiStatus wanted =
            strcmp(mode, "cross-file") == 0 ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_PARSE_ERROR;
        CHECK(UmiProjectPresetCatalogueCreate(declarations, strlen(declarations), user, strlen(user),
                                              &catalogue) == wanted);
        CHECK(catalogue == NULL);
    }
    else if (strcmp(mode, "user-only") == 0 || strcmp(mode, "empty") == 0)
    {
        const char *user = strcmp(mode, "empty") == 0 ? "{\"version\":3}" : declarations;
        CHECK(UmiProjectPresetCatalogueCreate(NULL, 0U, user, strlen(user), &catalogue) == UMI_STATUS_OK);
        CHECK(UmiProjectPresetCatalogueSummary(catalogue, &summary) == UMI_STATUS_OK);
        CHECK(summary.user_file && !summary.project_file &&
              summary.count == (strcmp(mode, "empty") == 0 ? 0U : 3U));
        if (summary.count != 0U)
        {
            CHECK(UmiProjectPresetCatalogueAt(catalogue, 0U, &choice) == UMI_STATUS_OK);
            CHECK(choice.from_user_file);
        }
    }
    else if (strcmp(mode, "metadata") == 0)
    {
        const char *json =
            "{\"version\":6,\"include\":[\"../"
            "other.json\"],\"configurePresets\":[{\"name\":\"same\",\"hidden\":true}],\"buildPresets\":[{"
            "\"name\":\"same\",\"inherits\":[\"base\"],\"condition\":{\"type\":\"equals\",\"lhs\":\"$env{"
            "HOST}\",\"rhs\":\"linux\"}}],\"testPresets\":[{\"name\":\"same\",\"condition\":false}]}";
        CHECK(UmiProjectPresetCatalogueCreate(json, strlen(json), NULL, 0U, &catalogue) == UMI_STATUS_OK);
        CHECK(UmiProjectPresetCatalogueSummary(catalogue, &summary) == UMI_STATUS_OK &&
              summary.includes_not_read && summary.count == 3U);
        CHECK(UmiProjectPresetCatalogueAt(catalogue, 1U, &choice) == UMI_STATUS_OK && choice.inherits &&
              choice.condition_deferred);
        CHECK(UmiProjectPresetCatalogueSelect(catalogue, 1U, &profile, &output) == UMI_STATUS_OK);
        memcpy(&expected, &output, sizeof(expected));
        CHECK(UmiProjectPresetCatalogueSelect(catalogue, 0U, &profile, &output) ==
              UMI_STATUS_PERMISSION_DENIED);
        CHECK(memcmp(&expected, &output, sizeof(output)) == 0);
        CHECK(UmiProjectPresetCatalogueSelect(catalogue, 2U, &profile, &output) ==
              UMI_STATUS_PERMISSION_DENIED);
        CHECK(memcmp(&expected, &output, sizeof(output)) == 0);
    }
    else if (strcmp(mode, "unicode-owned") == 0)
    {
        const char *json = "\xef\xbb\xbf{\"version\":3,\"buildPresets\":[{\"name\":\"caf\\u00e9 "
                           "\\ud83d\\ude80\",\"displayName\":\"Local build\",\"description\":\"First "
                           "line\\nSecond line\"}]}";
        owned = malloc(strlen(json) + 1U);
        CHECK(owned != NULL);
        memcpy(owned, json, strlen(json) + 1U);
        CHECK(UmiProjectPresetCatalogueCreate(owned, strlen(owned), NULL, 0U, &catalogue) == UMI_STATUS_OK);
        memset(owned, 'x', strlen(owned));
        CHECK(UmiProjectPresetCatalogueAt(catalogue, 0U, &choice) == UMI_STATUS_OK);
        CHECK(strcmp(choice.name, "caf\xc3\xa9 \xf0\x9f\x9a\x80") == 0);
        CHECK(strcmp(choice.description, "First line\nSecond line") == 0);
        CHECK(strcmp(choice.display_name, "Local build") == 0);
    }
    else
    {
        CHECK(UmiProjectPresetCatalogueCreate(declarations, strlen(declarations), NULL, 0U, &catalogue) ==
              UMI_STATUS_OK);
        CHECK(UmiProjectPresetCatalogueSummary(catalogue, &summary) == UMI_STATUS_OK && summary.count == 3U);
        if (strcmp(mode, "stages") == 0)
        {
            const char *names[] = {"notes configure", "notes-build", "notes-test"};
            for (size_t i = 0U; i < 3U; ++i)
            {
                CHECK(UmiProjectPresetCatalogueAt(catalogue, i, &choice) == UMI_STATUS_OK);
                CHECK((size_t)choice.stage == i && strcmp(choice.name, names[i]) == 0);
            }
            CHECK(UmiProjectPresetCatalogueAt(catalogue, 0U, &choice) == UMI_STATUS_OK);
            CHECK(strcmp(choice.binary_directory, "${sourceDir}/build/notes") == 0);
        }
        else if (strcmp(mode, "select") == 0)
        {
            strcpy(profile.configure_preset, "manual-configure");
            strcpy(profile.build_preset, "manual-build");
            strcpy(profile.test_preset, "manual-test");
            strcpy(profile.run_working_directory, "sample data");
            for (size_t i = 0U; i < 3U; ++i)
            {
                expected = profile;
                const char *name = i == 0U ? "notes configure" : i == 1U ? "notes-build" : "notes-test";
                strcpy(i == 0U   ? expected.configure_preset
                       : i == 1U ? expected.build_preset
                                 : expected.test_preset,
                       name);
                CHECK(UmiProjectPresetCatalogueSelect(catalogue, i, &profile, &output) == UMI_STATUS_OK);
                CHECK(umi_build_profile_equal(&expected, &output));
            }
        }
        else if (strcmp(mode, "atomic") == 0)
        {
            output = profile;
            memcpy(&expected, &output, sizeof(expected));
            strcpy(profile.preset, "legacy");
            CHECK(UmiProjectPresetCatalogueSelect(catalogue, 0U, &profile, &output) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&expected, &output, sizeof(output)) == 0);
            profile.preset[0] = '\0';
            profile.parallel_jobs = 0U;
            CHECK(UmiProjectPresetCatalogueSelect(catalogue, 0U, &profile, &output) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(&expected, &output, sizeof(output)) == 0);
            memset(&choice, 0x5a, sizeof(choice));
            UmiProjectPresetChoice before;
            memcpy(&before, &choice, sizeof(before));
            CHECK(UmiProjectPresetCatalogueAt(catalogue, 3U, &choice) == UMI_STATUS_NOT_FOUND);
            CHECK(memcmp(&before, &choice, sizeof(choice)) == 0);
        }
        else
            CHECK(0);
    }
cleanup:
    UmiProjectPresetCatalogueDestroy(catalogue);
    free(owned);
    return failed;
}
