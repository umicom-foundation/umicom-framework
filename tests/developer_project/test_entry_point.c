/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_entry_point.c
 * PURPOSE: Check entry-point resolution without current-directory or template-layout assumptions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/new_project.h"
#include "umicom/platform/path.h"
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
#ifdef _WIN32
#define ROOT "C:/Lesson Projects/notes"
#else
#define ROOT "/tmp/Lesson Projects/notes"
#endif
int main(void)
{
    UmiDeveloperProjectModel project;
    umi_developer_project_model_init(&project, "notes", "Notes project");
    strcpy(project.primary_language_id, "c23");
    strcpy(project.root, ROOT);
    char out[UMI_PATH_CAPACITY], expected[UMI_PATH_CAPACITY];
    CHECK(UmiDeveloperProjectEntryPath(&project, out, sizeof out) == UMI_STATUS_OK);
    CHECK(umi_path_join(ROOT, "src/main.c", expected, sizeof expected) == UMI_STATUS_OK);
    CHECK(umi_path_equal(out, expected));
    strcpy(project.entry_point, "main.c");
    CHECK(UmiDeveloperProjectEntryPath(&project, out, sizeof out) == UMI_STATUS_OK);
    CHECK(umi_path_join(ROOT, "main.c", expected, sizeof expected) == UMI_STATUS_OK);
    CHECK(umi_path_equal(out, expected));
    strcpy(project.entry_point, "src/../main.c");
    CHECK(UmiDeveloperProjectEntryPath(&project, out, sizeof out) == UMI_STATUS_OK);
    CHECK(umi_path_equal(out, expected));
    strcpy(out, "unchanged");
    strcpy(project.entry_point, "../../outside.c");
    CHECK(UmiDeveloperProjectEntryPath(&project, out, sizeof out) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(strcmp(out, "unchanged") == 0);
    strcpy(project.entry_point, "/absolute.c");
    CHECK(UmiDeveloperProjectEntryPath(&project, out, sizeof out) == UMI_STATUS_INVALID_ARGUMENT);
    strcpy(project.entry_point, "C:relative.c");
    CHECK(UmiDeveloperProjectEntryPath(&project, out, sizeof out) == UMI_STATUS_INVALID_ARGUMENT);
    strcpy(project.entry_point, "main.c");
    CHECK(UmiDeveloperProjectEntryPath(&project, out, 2U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(strcmp(out, "unchanged") == 0);
    memset(project.entry_point, 'x', sizeof project.entry_point);
    CHECK(UmiDeveloperProjectEntryPath(&project, out, sizeof out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(strcmp(out, "unchanged") == 0);
    CHECK(UmiDeveloperProjectEntryPath(NULL, out, sizeof out) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
