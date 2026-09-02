/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/commit_message.c
 *
 * PURPOSE:
 *   Classify staged repository paths and build a deterministic conventional
 *   commit message. No source content is uploaded or passed to an AI service.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/commit_message.h"

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"

/* Compare ASCII path text without depending on the user's current locale. */
static int text_equal_ignore_case(
    const char *left,
    size_t left_length,
    const char *right)
{
    size_t index;
    size_t right_length = strlen(right);

    if (left_length != right_length) return 0;
    for (index = 0U; index < left_length; ++index) {
        if (tolower((unsigned char)left[index]) !=
            tolower((unsigned char)right[index])) {
            return 0;
        }
    }
    return 1;
}

/* Recognise a fixed prefix while accepting either path separator. */
static int path_starts_with(
    const char *path,
    size_t path_length,
    const char *prefix)
{
    size_t index;
    size_t prefix_length = strlen(prefix);

    if (path_length < prefix_length) return 0;
    for (index = 0U; index < prefix_length; ++index) {
        char path_character = path[index] == '\\' ? '/' : path[index];
        char prefix_character = prefix[index] == '\\' ? '/' : prefix[index];
        if (tolower((unsigned char)path_character) !=
            tolower((unsigned char)prefix_character)) {
            return 0;
        }
    }
    return 1;
}

/* Find the filename inside a bounded path line without writing into Git's
 * captured output buffer. */
static const char *path_filename(const char *path, size_t path_length)
{
    size_t index = path_length;

    while (index > 0U) {
        if (path[index - 1U] == '/' || path[index - 1U] == '\\') {
            return path + index;
        }
        --index;
    }
    return path;
}

/* Match a filename extension without requiring a temporary NUL-terminated
 * string for every staged record. */
static int path_has_extension(
    const char *path,
    size_t path_length,
    const char *extension)
{
    size_t extension_length = strlen(extension);

    if (path_length < extension_length) return 0;
    return text_equal_ignore_case(
        path + path_length - extension_length,
        extension_length,
        extension);
}

/* Documentation-only commits receive a docs prefix. General HTML and text
 * files are not assumed to be documentation because applications may use them
 * as runtime assets. */
static int path_is_documentation(const char *path, size_t path_length)
{
    const char *filename = path_filename(path, path_length);
    size_t filename_length = (size_t)((path + path_length) - filename);

    return path_starts_with(path, path_length, "docs/") ||
           text_equal_ignore_case(filename, filename_length, "README") ||
           text_equal_ignore_case(filename, filename_length, "README.md") ||
           text_equal_ignore_case(filename, filename_length, "LICENSE") ||
           path_has_extension(path, path_length, ".md") ||
           path_has_extension(path, path_length, ".rst") ||
           path_has_extension(path, path_length, ".adoc");
}

/* Test-only changes are identified from the conventional test directory and
 * filename shapes already used across Umicom repositories. */
static int path_is_test(const char *path, size_t path_length)
{
    const char *filename = path_filename(path, path_length);
    size_t filename_length = (size_t)((path + path_length) - filename);
    size_t index;

    if (path_starts_with(path, path_length, "tests/") ||
        path_starts_with(filename, filename_length, "test_")) {
        return 1;
    }
    for (index = 0U; index + 6U <= path_length; ++index) {
        if ((index == 0U || path[index - 1U] == '/' ||
             path[index - 1U] == '\\') &&
            path_starts_with(path + index, path_length - index, "tests/")) {
            return 1;
        }
    }
    return 0;
}

/* Build-only commits cover build-system definitions and automation metadata,
 * but not ordinary source files that happen to accompany those definitions. */
static int path_is_build(const char *path, size_t path_length)
{
    const char *filename = path_filename(path, path_length);
    size_t filename_length = (size_t)((path + path_length) - filename);

    return path_starts_with(path, path_length, "cmake/") ||
           path_starts_with(path, path_length, ".github/") ||
           text_equal_ignore_case(
               filename, filename_length, "CMakeLists.txt") ||
           path_has_extension(path, path_length, ".cmake");
}

/* Convert the repository directory name into a safe lowercase message scope.
 * Unsupported punctuation becomes a separator rather than reaching Git. */
static void make_scope(
    const char *repository_root,
    char *out_scope,
    size_t capacity)
{
    char current_directory[UMI_PATH_CAPACITY];
    char absolute_root[UMI_PATH_CAPACITY];
    char directory_name[UMI_PATH_CAPACITY];
    const char *source = "repository";
    size_t read_index;
    size_t write_index = 0U;
    int previous_separator = 0;

    if (umi_fs_current_directory(current_directory,
                                 sizeof(current_directory)) == UMI_STATUS_OK &&
        umi_path_absolute(repository_root, current_directory,
                          absolute_root, sizeof(absolute_root)) == UMI_STATUS_OK &&
        umi_path_basename(absolute_root, directory_name,
                          sizeof(directory_name)) == UMI_STATUS_OK &&
        directory_name[0] != '\0') {
        source = directory_name;
    }

    for (read_index = 0U; source[read_index] != '\0' &&
         write_index + 1U < capacity; ++read_index) {
        unsigned char character = (unsigned char)source[read_index];
        if (isalnum(character)) {
            out_scope[write_index++] = (char)tolower(character);
            previous_separator = 0;
        } else if (!previous_separator && write_index > 0U) {
            out_scope[write_index++] = '-';
            previous_separator = 1;
        }
    }
    if (write_index > 0U && out_scope[write_index - 1U] == '-') {
        --write_index;
    }
    out_scope[write_index] = '\0';
    if (write_index == 0U) {
        (void)snprintf(out_scope, capacity, "%s", "repository");
    }
}

/* Classify every staged path. A mixed change deliberately falls back to feat
 * because claiming that it only changes docs, tests or build files would be
 * misleading. */
UmiStatus umi_repository_commit_message_generate(
    const char *repository_root,
    const char *staged_paths,
    UmiRepositoryCommitMessage *out_message)
{
    const char *cursor;
    int documentation_only = 1;
    int tests_only = 1;
    int build_only = 1;
    const char *type;
    const char *summary;
    int written;

    if (repository_root == NULL || repository_root[0] == '\0' ||
        staged_paths == NULL || out_message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_message, 0, sizeof(*out_message));

    cursor = staged_paths;
    while (*cursor != '\0') {
        const char *line = cursor;
        size_t line_length;

        while (*cursor != '\0' && *cursor != '\n') ++cursor;
        line_length = (size_t)(cursor - line);
        if (line_length > 0U && line[line_length - 1U] == '\r') {
            --line_length;
        }
        if (line_length > 0U) {
            if (out_message->changed_path_count == SIZE_MAX) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            ++out_message->changed_path_count;
            documentation_only = documentation_only &&
                path_is_documentation(line, line_length);
            tests_only = tests_only && path_is_test(line, line_length);
            build_only = build_only && path_is_build(line, line_length);
        }
        if (*cursor == '\n') ++cursor;
    }
    if (out_message->changed_path_count == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }

    make_scope(repository_root, out_message->scope,
               sizeof(out_message->scope));
    if (documentation_only) {
        out_message->kind = UMI_REPOSITORY_COMMIT_MESSAGE_DOCUMENTATION;
        type = "docs";
        summary = "update documentation";
    } else if (tests_only) {
        out_message->kind = UMI_REPOSITORY_COMMIT_MESSAGE_TEST;
        type = "test";
        summary = "strengthen test coverage";
    } else if (build_only) {
        out_message->kind = UMI_REPOSITORY_COMMIT_MESSAGE_BUILD;
        type = "build";
        summary = "update build configuration";
    } else {
        out_message->kind = UMI_REPOSITORY_COMMIT_MESSAGE_FEATURE;
        type = "feat";
        summary = "update implementation";
    }

    written = snprintf(out_message->message, sizeof(out_message->message),
                       "%s(%s): %s", type, out_message->scope, summary);
    if (written < 0 || (size_t)written >= sizeof(out_message->message)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
