/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/detection.c
 *
 * PURPOSE:
 *   Detect project build/language shape from canonical workspace file-index
 *   records. Detection is evidence only; it does not rewrite project files.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/detection.h"

#include <stdio.h>
#include <string.h>

static int contains_text(const char *text, const char *needle)
{
    return text != NULL && needle != NULL && strstr(text, needle) != NULL;
}

static int language_index(
    UmiDeveloperProjectDetectionReport *report,
    const char *language_id)
{
    size_t index;

    for (index = 0U; index < report->language_count; ++index) {
        if (strcmp(report->language_ids[index], language_id) == 0) {
            return (int)index;
        }
    }

    return -1;
}

static UmiStatus add_language(
    UmiDeveloperProjectDetectionReport *report,
    const char *language_id)
{
    size_t length;

    if (language_index(report, language_id) >= 0) return UMI_STATUS_OK;

    if (report->language_count >= UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    length = strlen(language_id);
    if (length >= sizeof(report->language_ids[0])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(
        report->language_ids[report->language_count],
        language_id,
        length + 1U);
    report->language_count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_project_detect(
    const UmiFileIndex *file_index,
    const UmiDeveloperProjectLanguageRegistry *languages,
    UmiDeveloperProjectDetectionReport *out_report)
{
    UmiFileIndexStats stats;
    size_t language_hits[UMI_DEVELOPER_PROJECT_LANGUAGE_CAPACITY] = {0U};
    size_t index;
    size_t best_hits = 0U;

    if (file_index == NULL || languages == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_report, 0, sizeof(*out_report));
    stats = umi_file_index_stats(file_index);
    out_report->source_revision = stats.revision;

    for (index = 0U; index < stats.files; ++index) {
        UmiFileIndexEntry entry;
        const UmiDeveloperProjectLanguagePack *pack;

        if (umi_file_index_at(file_index, index, &entry) != UMI_STATUS_OK) {
            continue;
        }

        if (strcmp(entry.name, "CMakeLists.txt") == 0) {
            out_report->build_system = UMI_DEVELOPER_PROJECT_BUILD_CMAKE;
        } else if (strcmp(entry.name, "build.zig") == 0 &&
                   out_report->build_system == UMI_DEVELOPER_PROJECT_BUILD_NONE) {
            out_report->build_system = UMI_DEVELOPER_PROJECT_BUILD_ZIG;
        } else if (strcmp(entry.name, "Cargo.toml") == 0 &&
                   out_report->build_system == UMI_DEVELOPER_PROJECT_BUILD_NONE) {
            out_report->build_system = UMI_DEVELOPER_PROJECT_BUILD_CARGO;
        }

        if (strcmp(entry.name, "CMakePresets.json") == 0) {
            out_report->has_cmake_presets = 1;
        }
        if (strcmp(entry.name, ".git") == 0 ||
            contains_text(entry.relative_path, ".git/")) {
            out_report->has_git_repository = 1;
        }
        if (contains_text(entry.relative_path, "test") ||
            contains_text(entry.relative_path, "tests/")) {
            out_report->has_tests = 1;
        }

        pack = umi_developer_project_language_registry_for_extension(
            languages, entry.extension);
        if (pack != NULL) {
            const int existing = language_index(out_report, pack->language_id);
            UmiStatus status;

            if (existing < 0) {
                status = add_language(out_report, pack->language_id);
                if (status != UMI_STATUS_OK) return status;
                language_hits[out_report->language_count - 1U] = 1U;
            } else {
                language_hits[(size_t)existing] += 1U;
            }

            out_report->source_file_count += 1U;
        }
    }

    /*
     * Prefer executable/native languages as the project identity. Build/config
     * files (CMake/JSON/YAML) remain part of the language matrix but must not
     * become the "primary language" merely because a small project has several
     * configuration files.
     */
    for (index = 0U; index < out_report->language_count; ++index) {
        const UmiDeveloperProjectLanguagePack *pack =
            umi_developer_project_language_registry_find(
                languages,
                out_report->language_ids[index]);

        if (pack != NULL &&
            (pack->native_language ||
             pack->compiled_language ||
             umi_developer_project_language_pack_has_capability(
                 pack,
                 UMI_DEVELOPER_PROJECT_LANGUAGE_RUN)) &&
            language_hits[index] > best_hits) {
            best_hits = language_hits[index];
            (void)snprintf(
                out_report->primary_language_id,
                sizeof(out_report->primary_language_id),
                "%s",
                out_report->language_ids[index]);
        }
    }

    if (out_report->primary_language_id[0] == '\0' &&
        out_report->language_count > 0U) {
        size_t fallback_hits = 0U;

        for (index = 0U; index < out_report->language_count; ++index) {
            if (language_hits[index] > fallback_hits) {
                fallback_hits = language_hits[index];
                (void)snprintf(
                    out_report->primary_language_id,
                    sizeof(out_report->primary_language_id),
                    "%s",
                    out_report->language_ids[index]);
            }
        }
    }

    if (out_report->build_system == UMI_DEVELOPER_PROJECT_BUILD_NONE &&
        out_report->language_count > 0U) {
        if (strcmp(out_report->primary_language_id,
                   "developer.language.bash") == 0 ||
            strcmp(out_report->primary_language_id,
                   "developer.language.cpython") == 0) {
            out_report->build_system = UMI_DEVELOPER_PROJECT_BUILD_SCRIPT;
        }
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_developer_project_detection_to_model(
    const UmiDeveloperProjectDetectionReport *report,
    const char *project_id,
    const char *display_name,
    const char *root,
    UmiDeveloperProjectModel *out_model)
{
    size_t index;

    if (report == NULL || project_id == NULL ||
        display_name == NULL || root == NULL ||
        out_model == NULL || report->primary_language_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_developer_project_model_init(out_model, project_id, display_name);
    (void)snprintf(out_model->root, sizeof(out_model->root), "%s", root);
    (void)snprintf(
        out_model->primary_language_id,
        sizeof(out_model->primary_language_id),
        "%s",
        report->primary_language_id);
    out_model->build_system = report->build_system;
    out_model->generated = 0;

    for (index = 0U; index < report->language_count; ++index) {
        if (strcmp(report->language_ids[index],
                   report->primary_language_id) != 0) {
            UmiStatus status = umi_developer_project_model_add_language(
                out_model, report->language_ids[index]);
            if (status != UMI_STATUS_OK) return status;
        }
    }

    return UMI_STATUS_OK;
}
