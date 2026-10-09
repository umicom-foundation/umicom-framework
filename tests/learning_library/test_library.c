/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_library/test_library.c
 * PURPOSE: Verify explicit lesson locations and reject accidental workspace-relative resolution.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/teacher/foundations_curriculum.h"
#include "umicom/teacher/learning_library.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    char selected[UMI_PATH_CAPACITY], directory[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY];
    CHECK(umi_path_copy(selected, sizeof(selected), argv[1]) == UMI_STATUS_OK);
    if (strcmp(argv[2], "docs") == 0)
        CHECK(umi_path_join(argv[1], "docs", selected, sizeof(selected)) == UMI_STATUS_OK);
    else if (strcmp(argv[2], "learning") == 0)
        CHECK(umi_path_join(argv[1], "docs/learning", selected, sizeof(selected)) == UMI_STATUS_OK);
    else if (strcmp(argv[2], "invalid") == 0)
    {
        strcpy(directory, "unchanged");
        CHECK(UmiLearningLibrarySelect("framework", directory, sizeof(directory)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(directory, "unchanged") == 0);
        CHECK(UmiLearningLibrarySelect(NULL, directory, sizeof(directory)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningLibrarySelect(argv[1], directory, 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(directory, "unchanged") == 0);
        CHECK(UmiLearningLibrarySelect(argv[1], directory, sizeof(directory)) == UMI_STATUS_OK);
        strcpy(file, "unchanged");
        CHECK(UmiLearningLibraryResource(directory, "../welcome.html", file, sizeof(file)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningLibraryResource(directory, "https://example.test/lesson", file,
                                         sizeof(file)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningLibraryResource(directory, "framework/docs/learning/welcome.html", file,
                                         1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(file, "unchanged") == 0);
        return 0;
    }
    else if (strcmp(argv[2], "framework") != 0)
        return 2;
    CHECK(UmiLearningLibrarySelect(selected, directory, sizeof(directory)) == UMI_STATUS_OK);
    CHECK(umi_path_is_absolute(directory));
    /* Every advertised lesson must actually ship in the selected source library.
     * Adding a catalogue entry without its readable resource breaks this check. */
    for (size_t i = 0U; i < umi_teacher_foundations_curriculum_count(); ++i)
    {
        const UmiTeacherFoundationsLesson *lesson = umi_teacher_foundations_curriculum_at(i);
        CHECK(lesson != NULL);
        CHECK(UmiLearningLibraryResource(directory, lesson->resource_path, file, sizeof(file)) ==
              UMI_STATUS_OK);
        CHECK(umi_path_is_absolute(file));
    }
    return 0;
}
