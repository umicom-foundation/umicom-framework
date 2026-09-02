/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/ignore.c
 *
 * PURPOSE:
 *   Implement default repository context exclusions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/ignore.h"

#include <string.h>

/*
 * Provide the path component equals operation used by this module and its client
 * applications.
 */
static int path_component_equals(const char *path, const char *name)
{
    const char *cursor = path;
    const size_t name_length = strlen(name);

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor != NULL && *cursor != '\0') {
        const char *end = strchr(cursor, '/');
        const size_t length =
            end != NULL ? (size_t)(end - cursor) : strlen(cursor);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length == name_length &&
            strncmp(cursor, name, length) == 0) {
            return 1;
        }

        cursor = end != NULL ? end + 1 : NULL;
    }

    return 0;
}

/*
 * Provide the has binary extension operation used by this module and its client
 * applications.
 */
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

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(EXTENSIONS) / sizeof(EXTENSIONS[0]);
         ++index) {
        const size_t ext_length = strlen(EXTENSIONS[index]);

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (path_length >= ext_length &&
            strcmp(path + path_length - ext_length, EXTENSIONS[index]) == 0) {
            return 1;
        }
    }

    return 0;
}

/*
 * Initialise ai coding ignore policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_ignore_policy_init(UmiAiCodingIgnorePolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;

    policy->ignore_git = 1;
    policy->ignore_build = 1;
    policy->ignore_install = 1;
    policy->ignore_dependencies = 1;
    policy->ignore_hidden_directories = 1;
    policy->ignore_binary_extensions = 1;
}

/*
 * Provide the ai coding ignore path operation used by this module and its client
 * applications.
 */
int umi_ai_coding_ignore_path(
    const UmiAiCodingIgnorePolicy *policy,
    const char *relative_path,
    int directory)
{
    const char *base;
    const char *slash;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || relative_path == NULL ||
        relative_path[0] == '\0') {
        return 1;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->ignore_git &&
        path_component_equals(relative_path, ".git")) {
        return 1;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->ignore_build &&
        (path_component_equals(relative_path, "build") ||
         path_component_equals(relative_path, "out") ||
         path_component_equals(relative_path, "dist"))) {
        return 1;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->ignore_install &&
        path_component_equals(relative_path, "install")) {
        return 1;
    }

    /* Apply this branch only when its contract condition is satisfied. */
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

    /* Apply this branch only when its contract condition is satisfied. */
    if (directory && policy->ignore_hidden_directories &&
        base[0] == '.' &&
        strcmp(base, ".github") != 0) {
        return 1;
    }

    return !directory &&
        policy->ignore_binary_extensions &&
        has_binary_extension(relative_path);
}
