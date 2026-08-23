/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/ignore.c
 *
 * PURPOSE:
 *   Implement default repository context exclusions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/ignore.h"

#include <string.h>

static int path_component_equals(const char *path, const char *name)
{
    const char *cursor = path;
    const size_t name_length = strlen(name);

    while (cursor != NULL && *cursor != '\0') {
        const char *end = strchr(cursor, '/');
        const size_t length =
            end != NULL ? (size_t)(end - cursor) : strlen(cursor);

        if (length == name_length &&
            strncmp(cursor, name, length) == 0) {
            return 1;
        }

        cursor = end != NULL ? end + 1 : NULL;
    }

    return 0;
}

static int has_binary_extension(const char *path)
{
    static const char *const EXTENSIONS[] = {
        ".exe", ".dll", ".so", ".dylib", ".a", ".lib", ".o", ".obj",
        ".pdb", ".class", ".jar", ".zip", ".7z", ".gz", ".tar",
        ".png", ".jpg", ".jpeg", ".gif", ".webp", ".ico", ".pdf",
        ".woff", ".woff2", ".ttf", ".otf", ".bin", ".db", ".sqlite",
        ".sqlite3", ".pyc"
    };
    size_t index;
    const size_t path_length = path != NULL ? strlen(path) : 0U;

    for (index = 0U; index < sizeof(EXTENSIONS) / sizeof(EXTENSIONS[0]);
         ++index) {
        const size_t ext_length = strlen(EXTENSIONS[index]);

        if (path_length >= ext_length &&
            strcmp(path + path_length - ext_length, EXTENSIONS[index]) == 0) {
            return 1;
        }
    }

    return 0;
}

void umi_ai_coding_ignore_policy_init(UmiAiCodingIgnorePolicy *policy)
{
    if (policy == NULL) return;

    policy->ignore_git = 1;
    policy->ignore_build = 1;
    policy->ignore_install = 1;
    policy->ignore_dependencies = 1;
    policy->ignore_hidden_directories = 1;
    policy->ignore_binary_extensions = 1;
}

int umi_ai_coding_ignore_path(
    const UmiAiCodingIgnorePolicy *policy,
    const char *relative_path,
    int directory)
{
    const char *base;
    const char *slash;

    if (policy == NULL || relative_path == NULL ||
        relative_path[0] == '\0') {
        return 1;
    }

    if (policy->ignore_git &&
        path_component_equals(relative_path, ".git")) {
        return 1;
    }

    if (policy->ignore_build &&
        (path_component_equals(relative_path, "build") ||
         path_component_equals(relative_path, "out") ||
         path_component_equals(relative_path, "dist"))) {
        return 1;
    }

    if (policy->ignore_install &&
        path_component_equals(relative_path, "install")) {
        return 1;
    }

    if (policy->ignore_dependencies &&
        (path_component_equals(relative_path, "node_modules") ||
         path_component_equals(relative_path, "vendor") ||
         path_component_equals(relative_path, ".venv") ||
         path_component_equals(relative_path, "venv") ||
         path_component_equals(relative_path, "target"))) {
        return 1;
    }

    slash = strrchr(relative_path, '/');
    base = slash != NULL ? slash + 1 : relative_path;

    if (directory && policy->ignore_hidden_directories &&
        base[0] == '.' &&
        strcmp(base, ".github") != 0) {
        return 1;
    }

    return !directory &&
        policy->ignore_binary_extensions &&
        has_binary_extension(relative_path);
}
