/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_presets/test_graph.c
 * PURPOSE: Exercise included preset discovery, inheritance visibility, atomic failures and retained provenance.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/developer_project/preset_catalogue.h"
#include "umicom/platform/filesystem.h"
static void Document(const char *root, const char *name, const char *text)
{
    char path[UMI_PATH_CAPACITY];
    FixturePath(path, root, name);
    CHECK(umi_fs_write_text(path, text) == UMI_STATUS_OK);
}
static UmiProjectPresetChoice Choice(const UmiProjectPresetCatalogue *catalogue, const char *name,
                                     UmiProjectPresetStage stage, size_t *outIndex)
{
    UmiProjectPresetSummary summary;
    CHECK(UmiProjectPresetCatalogueSummary(catalogue, &summary) == UMI_STATUS_OK);
    UmiProjectPresetChoice choice = {0};
    for (size_t i = 0U; i < summary.count; ++i)
    {
        CHECK(UmiProjectPresetCatalogueAt(catalogue, i, &choice) == UMI_STATUS_OK);
        if (choice.stage == stage && strcmp(choice.name, name) == 0)
        {
            *outIndex = i;
            return choice;
        }
    }
    CHECK(0);
    return choice;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], directory[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(directory, root, "presets");
    CHECK(umi_fs_make_directories(directory) == UMI_STATUS_OK);
    const char *project =
        "{\"version\":9,\"include\":[\"presets/shared.json\"],"
        "\"configurePresets\":[{\"name\":\"debug\",\"inherits\":\"base\",\"displayName\":\"Debug sources\"}],"
        "\"buildPresets\":[{\"name\":\"build\",\"inherits\":\"build-base\"}]}";
    const char *shared =
        "{\"version\":9,\"configurePresets\":[{\"name\":\"base\",\"hidden\":true,\"binaryDir\":\"${sourceDir}"
        "/build\","
        "\"displayName\":\"Do not inherit this label\",\"condition\":true}],"
        "\"buildPresets\":[{\"name\":\"build-base\",\"hidden\":true,\"configurePreset\":\"debug\"}]}";
    UmiStatus expected = UMI_STATUS_OK;
    bool writeProject = true, writeShared = true;
    if (strcmp(mode, "missing") == 0)
    {
        writeProject = false;
        writeShared = false;
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(mode, "include-missing") == 0)
    {
        writeShared = false;
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(mode, "malformed") == 0)
    {
        shared = "{";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "cycle") == 0)
    {
        shared = "{\"version\":9,\"include\":[\"../CMakePresets.json\"]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "repeat") == 0)
        project = "{\"version\":9,\"include\":[\"presets/shared.json\",\"presets/../presets/shared.json\"]}";
    else if (strcmp(mode, "diamond") == 0)
    {
        project = "{\"version\":9,\"include\":[\"left.json\",\"right.json\"]}";
        Document(root, "left.json", "{\"version\":9,\"include\":[\"presets/shared.json\"]}");
        Document(root, "right.json", "{\"version\":9,\"include\":[\"presets/shared.json\"]}");
    }
    else if (strcmp(mode, "visibility") == 0)
    {
        project = "{\"version\":9,\"include\":[\"left.json\",\"presets/shared.json\"]}";
        Document(root, "left.json",
                 "{\"version\":9,\"configurePresets\":[{\"name\":\"debug\",\"inherits\":\"base\"}]}");
        expected = UMI_STATUS_PERMISSION_DENIED;
    }
    else if (strcmp(mode, "inherit-cycle") == 0)
    {
        project = "{\"version\":9,\"configurePresets\":[{\"name\":\"a\",\"inherits\":\"b\"},{\"name\":\"b\","
                  "\"inherits\":\"a\"}]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "inherit-missing") == 0)
    {
        project = "{\"version\":9,\"configurePresets\":[{\"name\":\"a\",\"inherits\":\"absent\"}]}";
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(mode, "inherit-type") == 0)
    {
        project = "{\"version\":9,\"configurePresets\":[{\"name\":\"a\",\"inherits\":[42]}]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "duplicate") == 0)
    {
        project = "{\"version\":9,\"include\":[\"presets/"
                  "shared.json\"],\"configurePresets\":[{\"name\":\"base\"}]}";
        expected = UMI_STATUS_ALREADY_EXISTS;
    }
    else if (strcmp(mode, "duplicate-field") == 0)
    {
        project = "{\"version\":9,\"include\":[],\"include\":[\"presets/shared.json\"]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "precedence") == 0 || strcmp(mode, "null") == 0 || strcmp(mode, "false") == 0 ||
             strcmp(mode, "deferred") == 0)
    {
        if (strcmp(mode, "precedence") == 0)
            project = "{\"version\":9,\"configurePresets\":[{\"name\":\"debug\",\"inherits\":[\"first\","
                      "\"second\"]},"
                      "{\"name\":\"first\",\"hidden\":true,\"condition\":null},{\"name\":\"second\","
                      "\"hidden\":true,\"binaryDir\":\"second\",\"condition\":false}]}";
        else if (strcmp(mode, "null") == 0)
            project = "{\"version\":9,\"configurePresets\":[{\"name\":\"debug\",\"inherits\":[\"first\","
                      "\"second\"]},"
                      "{\"name\":\"first\",\"hidden\":true,\"condition\":null,\"inherits\":\"second\"},"
                      "{\"name\":\"second\",\"hidden\":true,\"condition\":false}]}";
        else if (strcmp(mode, "false") == 0)
            project = "{\"version\":9,\"configurePresets\":[{\"name\":\"debug\",\"inherits\":\"base\"},"
                      "{\"name\":\"base\",\"hidden\":true,\"condition\":false}]}";
        else
            project = "{\"version\":9,\"configurePresets\":[{\"name\":\"debug\",\"inherits\":\"base\"},"
                      "{\"name\":\"base\",\"hidden\":true,\"condition\":{\"type\":\"equals\",\"lhs\":\"x\","
                      "\"rhs\":\"y\"}}]}";
    }
    else if (strcmp(mode, "override") == 0)
    {
        project = "{\"version\":9,\"configurePresets\":[{\"name\":\"debug\",\"inherits\":[\"first\","
                  "\"second\"],\"condition\":null,\"binaryDir\":\"\"},"
                  "{\"name\":\"first\",\"hidden\":true,\"binaryDir\":\"first\",\"condition\":false},"
                  "{\"name\":\"second\",\"hidden\":true,\"binaryDir\":\"second\"}]}";
    }
    else if (strcmp(mode, "first-parent") == 0)
    {
        project =
            "{\"version\":9,\"configurePresets\":[{\"name\":\"debug\",\"inherits\":[\"first\",\"second\"]},"
            "{\"name\":\"first\",\"binaryDir\":\"first\"},{\"name\":\"second\",\"binaryDir\":\"second\"}]}";
    }
    else if (strcmp(mode, "user") == 0)
    {
        Document(root, "CMakeUserPresets.json",
                 "{\"version\":9,\"buildPresets\":[{\"name\":\"local\",\"inherits\":\"build\"}]}");
    }
    else if (strcmp(mode, "user-only") == 0)
    {
        writeProject = false;
        Document(root, "CMakeUserPresets.json", project);
    }
    else if (strcmp(mode, "reverse") == 0)
    {
        project = "{\"version\":9,\"include\":[\"CMakeUserPresets.json\"]}";
        Document(root, "CMakeUserPresets.json", "{\"version\":9}");
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "macro-source") == 0)
        project = "{\"version\":9,\"include\":[\"${sourceDir}/presets/shared.json\"]}";
    else if (strcmp(mode, "macro-file") == 0)
        project = "{\"version\":9,\"include\":[\"${fileDir}/presets/shared.json\"]}";
    else if (strcmp(mode, "macro-environment") == 0)
    {
        project = "{\"version\":9,\"include\":[\"$penv{UMICOM_PRESET_FILE}\"]}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    else if (strcmp(mode, "macro-format") == 0)
    {
        project = "{\"version\":6,\"include\":[\"${sourceDir}/presets/shared.json\"]}";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    else if (strcmp(mode, "include-format") == 0)
    {
        project = "{\"version\":3,\"include\":[]}";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(mode, "unicode") == 0)
    {
        project = "{\"version\":9,\"include\":[\"caf\\u00e9.json\"]}";
        Document(root, "caf\xc3\xa9.json", shared);
    }
    else if (strcmp(mode, "documents") == 0)
    {
        project = "{\"version\":9,\"include\":[\"chain0.json\"]}";
        for (unsigned i = 0U; i < 64U; ++i)
        {
            char name[64], content[128];
            CHECK(snprintf(name, sizeof(name), "chain%u.json", i) > 0);
            CHECK(snprintf(content, sizeof(content), "{\"version\":9,\"include\":[\"chain%u.json\"]}",
                           i + 1U) > 0);
            Document(root, name, content);
        }
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(mode, "large") == 0)
    {
        /* A vendor array exceeds the old token ceiling but has no preset
         * semantics. The complete document still needs grammar validation. */
        size_t count = 12000U;
        char *json = malloc(count * 2U + 256U);
        CHECK(json != NULL);
        size_t used = (size_t)sprintf(
            json, "{\"version\":9,\"configurePresets\":[{\"name\":\"debug\"}],\"vendor\":{\"data\":[");
        for (size_t i = 0U; i < count; ++i)
        {
            json[used++] = '0';
            json[used++] = i + 1U == count ? ']' : ',';
        }
        strcpy(json + used, "}}");
        Document(root, "CMakePresets.json", json);
        free(json);
        writeProject = false;
    }
    else if (strcmp(mode, "bytes") == 0)
    {
        size_t length = 2U * 1024U * 1024U + 1U;
        char *data = malloc(length + 1U);
        CHECK(data != NULL);
        memset(data, ' ', length);
        memcpy(data, "{\"version\":9}", 13U);
        data[length] = '\0';
        Document(root, "CMakePresets.json", data);
        free(data);
        writeProject = false;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(mode, "cancel") != 0 && strcmp(mode, "basic") != 0 && strcmp(mode, "legacy") != 0)
        return 2;
    if (writeProject)
        Document(root, "CMakePresets.json", project);
    if (writeShared)
        Document(root, "presets/shared.json", shared);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    UmiProjectPresetCatalogue *catalogue = NULL;
    UmiProjectPresetReadReport report;
    if (strcmp(mode, "cancel") == 0)
    {
        umi_cancellation_token_request(cancel);
        CHECK(UmiProjectPresetCatalogueReadExpanded(root, cancel, &catalogue, &report) ==
              UMI_STATUS_CANCELLED);
        CHECK(catalogue == NULL);
        umi_cancellation_token_reset(cancel);
    }
    UmiStatus status = UmiProjectPresetCatalogueReadExpanded(root, cancel, &catalogue, &report);
    CHECK(status == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(catalogue == NULL && report.problem_file[0] != '\0');
    else
    {
        UmiProjectPresetSummary summary;
        CHECK(UmiProjectPresetCatalogueSummary(catalogue, &summary) == UMI_STATUS_OK);
        CHECK(!summary.includes_not_read && report.problem_file[0] == '\0');
        if (strcmp(mode, "repeat") == 0 || strcmp(mode, "macro-source") == 0 ||
            strcmp(mode, "macro-file") == 0 || strcmp(mode, "unicode") == 0)
            CHECK(summary.count == 2U && report.file_count == 2U);
        else if (strcmp(mode, "diamond") == 0)
            CHECK(summary.count == 2U && report.file_count == 4U);
        else if (strcmp(mode, "user") == 0)
        {
            size_t index;
            UmiProjectPresetChoice choice = Choice(catalogue, "local", UMI_PROJECT_PRESET_BUILD, &index);
            CHECK(choice.from_user_file && !choice.hidden && strcmp(choice.configure_preset, "debug") == 0);
            CHECK(summary.project_file && summary.user_file);
        }
        else if (strcmp(mode, "large") == 0)
            CHECK(summary.count == 1U);
        else
        {
            size_t index;
            UmiProjectPresetChoice choice = Choice(catalogue, "debug", UMI_PROJECT_PRESET_CONFIGURE, &index);
            CHECK(!choice.hidden);
            if (strcmp(mode, "override") == 0)
                CHECK(!choice.condition_false && choice.binary_directory[0] == '\0');
            else if (strcmp(mode, "precedence") == 0)
                CHECK(choice.condition_false && strcmp(choice.binary_directory, "second") == 0);
            else if (strcmp(mode, "null") == 0 || strcmp(mode, "false") == 0)
                CHECK(choice.condition_false);
            else if (strcmp(mode, "deferred") == 0)
                CHECK(choice.condition_deferred && !choice.condition_false);
            else if (strcmp(mode, "first-parent") == 0)
                CHECK(strcmp(choice.binary_directory, "first") == 0);
            else
            {
                CHECK(strcmp(choice.binary_directory, "${sourceDir}/build") == 0);
                CHECK(strcmp(choice.display_name, "Debug sources") == 0 && choice.description[0] == '\0');
                UmiProjectPresetOrigin origin;
                CHECK(UmiProjectPresetCatalogueOriginAt(catalogue, index, &origin) == UMI_STATUS_OK);
                CHECK(!origin.included_file && origin.inherited_metadata);
                (void)Choice(catalogue, "base", UMI_PROJECT_PRESET_CONFIGURE, &index);
                CHECK(UmiProjectPresetCatalogueOriginAt(catalogue, index, &origin) == UMI_STATUS_OK &&
                      origin.included_file);
                CHECK(strstr(origin.file_path, "shared.json") != NULL);
            }
            if (choice.condition_false)
            {
                UmiBuildProfile profile, selected;
                umi_build_profile_init(&profile);
                strcpy(profile.source_directory, root);
                CHECK(UmiProjectPresetCatalogueSelect(catalogue, index, &profile, &selected) ==
                      UMI_STATUS_PERMISSION_DENIED);
            }
        }
    }
    UmiProjectPresetCatalogueDestroy(catalogue);
    catalogue = NULL;
    if (strcmp(mode, "legacy") == 0)
    {
        CHECK(UmiProjectPresetCatalogueCreate(project, strlen(project), NULL, 0U, &catalogue) ==
              UMI_STATUS_OK);
        UmiProjectPresetOrigin origin = {0};
        strcpy(origin.file_path, "retained");
        CHECK(UmiProjectPresetCatalogueOriginAt(catalogue, 0U, &origin) == UMI_STATUS_NOT_FOUND);
        CHECK(strcmp(origin.file_path, "retained") == 0);
        UmiProjectPresetCatalogueDestroy(catalogue);
    }
    umi_cancellation_token_destroy(cancel);
    return 0;
}
