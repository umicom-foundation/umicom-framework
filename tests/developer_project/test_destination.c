/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_destination.c
 * PURPOSE: Check explicit project folder selection without creating or changing files.
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
int main(void)
{
#ifdef _WIN32
    const char *parent = "C:/Projects";
#else
    const char *parent = "/projects";
#endif
    char destination[UMI_PATH_CAPACITY], expected[UMI_PATH_CAPACITY];
    CHECK(umi_path_join(parent, "My Notes", expected, sizeof expected) == UMI_STATUS_OK);
    CHECK(UmiDeveloperProjectChooseDestination(parent, "My Notes", destination,
                                               sizeof destination) == UMI_STATUS_OK);
    CHECK(strcmp(destination, expected) == 0);
    /* A new-project name must stay a single portable component on every host.
     * Refusing it here does not rename, remove or reserve anything on disk. */
    const char *invalid[] = {"",     ".",   "..",       "../Other", "sub/folder", "sub\\folder",
                             ".git", "NUL", "COM1.txt", "ends.",    "ends ",      "has:stream",
                             "a?b",  "a\nb"};
    for (size_t i = 0U; i < sizeof invalid / sizeof invalid[0]; ++i)
    {
        strcpy(destination, "keep");
        CHECK(UmiDeveloperProjectChooseDestination(parent, invalid[i], destination,
                                                   sizeof destination) != UMI_STATUS_OK);
        CHECK(strcmp(destination, "keep") == 0);
    }
    strcpy(destination, "keep");
    CHECK(UmiDeveloperProjectChooseDestination("relative", "Notes", destination,
                                               sizeof destination) != UMI_STATUS_OK);
    CHECK(strcmp(destination, "keep") == 0);
    CHECK(UmiDeveloperProjectChooseDestination(parent, "Notes", destination, 3U) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(strcmp(destination, "keep") == 0);
    CHECK(UmiDeveloperProjectChooseDestination(NULL, "Notes", destination, sizeof destination) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDeveloperProjectChooseDestination(parent, NULL, destination, sizeof destination) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDeveloperProjectChooseDestination(parent, "Notes", NULL, sizeof destination) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDeveloperProjectChooseDestination(parent, "caf\xc3\xa9", destination,
                                               sizeof destination) == UMI_STATUS_OK);
    return 0;
}
