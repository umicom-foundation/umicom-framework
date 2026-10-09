/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_library.c
 * PURPOSE: Keep curriculum lookup separate from writable projects and private application settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/filesystem.h"
#include "umicom/teacher/foundations_curriculum.h"
#include "umicom/teacher/learning_library.h"
#include <string.h>

/* Probe only recognised layouts below the selected folder. Moving a checkout
 * requires a new explicit choice; searching ancestor directories could silently
 * select another project's documentation. */
UmiStatus UmiLearningLibrarySelect(const char *selected, char *out, size_t capacity)
{
    static const char *const layouts[] = {"", "learning", "docs/learning",
                                          "framework/docs/learning"};
    char root[UMI_PATH_CAPACITY], candidate[UMI_PATH_CAPACITY], welcome[UMI_PATH_CAPACITY];
    if (selected == NULL || out == NULL || capacity == 0U || !umi_path_is_absolute(selected))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_path_normalise(selected, root, sizeof(root));
    if (status != UMI_STATUS_OK)
        return status;
    for (size_t index = 0U; index < sizeof(layouts) / sizeof(layouts[0]); ++index)
    {
        status = layouts[index][0] == '\0'
                     ? umi_path_copy(candidate, sizeof(candidate), root)
                     : umi_path_join(root, layouts[index], candidate, sizeof(candidate));
        if (status != UMI_STATUS_OK)
            return status;
        status = umi_path_join(candidate, "welcome.html", welcome, sizeof(welcome));
        if (status != UMI_STATUS_OK)
            return status;
        if (umi_fs_is_directory(candidate) && umi_fs_is_file(welcome))
        {
            size_t length = strlen(candidate);
            if (length >= capacity)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            memcpy(out, candidate, length + 1U);
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Existing catalogue paths remain public compatibility data. Resolve their
 * final filename beneath the selected library rather than changing every lesson
 * to an installation-specific absolute path. */
UmiStatus UmiLearningLibraryResource(const char *directory, const char *resource, char *out,
                                     size_t capacity)
{
    static const char prefix[] = "framework/docs/learning/";
    char candidate[UMI_PATH_CAPACITY];
    int recognised = 0;
    if (directory == NULL || resource == NULL || out == NULL || capacity == 0U ||
        !umi_path_is_absolute(directory))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < umi_teacher_foundations_curriculum_count(); ++index)
    {
        const UmiTeacherFoundationsLesson *lesson = umi_teacher_foundations_curriculum_at(index);
        if (lesson != NULL && strcmp(lesson->resource_path, resource) == 0)
        {
            recognised = 1;
            break;
        }
    }
    if (!recognised || strncmp(resource, prefix, sizeof(prefix) - 1U) != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *name = resource + sizeof(prefix) - 1U;
    if (name[0] == '\0' || strchr(name, '/') != NULL || strchr(name, '\\') != NULL ||
        strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_path_join(directory, name, candidate, sizeof(candidate));
    if (status != UMI_STATUS_OK)
        return status;
    if (!umi_fs_is_file(candidate))
        return UMI_STATUS_NOT_FOUND;
    size_t length = strlen(candidate);
    if (length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, candidate, length + 1U);
    return UMI_STATUS_OK;
}
