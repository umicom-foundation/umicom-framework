/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_path.c
 *
 * PURPOSE:
 *   Verify deterministic path normalisation, joining, relative paths, names,
 *   extensions and containment independently from filesystem access.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/path.h"

#include <assert.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char path[UMI_PATH_CAPACITY];
    char value[UMI_PATH_CAPACITY];

#ifdef _WIN32
    assert(umi_path_normalise("C:/work/./studio/../framework",
                              path,
                              sizeof(path)) == UMI_STATUS_OK);
    assert(strcmp(path, "C:\\work\\framework") == 0);
    assert(umi_path_relative("C:\\work\\studio",
                             "C:\\work\\framework\\include",
                             value,
                             sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(value, "..\\framework\\include") == 0);
#else
    assert(umi_path_normalise("/work/./studio/../framework",
                              path,
                              sizeof(path)) == UMI_STATUS_OK);
    assert(strcmp(path, "/work/framework") == 0);
    assert(umi_path_relative("/work/studio",
                             "/work/framework/include",
                             value,
                             sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(value, "../framework/include") == 0);
#endif

    assert(umi_path_join(path, "include/umicom", value, sizeof(value)) ==
           UMI_STATUS_OK);
    assert(umi_path_basename("source/module/file.name.c",
                             value,
                             sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(value, "file.name.c") == 0);
    assert(umi_path_extension("source/module/file.name.c",
                              value,
                              sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(value, "c") == 0);
    assert(umi_path_stem("source/module/file.name.c",
                         value,
                         sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(value, "file.name") == 0);
#ifdef _WIN32
    assert(umi_path_is_within("C:\\work", "C:\\work\\studio"));
    assert(!umi_path_is_within("C:\\work", "C:\\worker"));
#else
    assert(umi_path_is_within("/work", "/work/studio"));
    assert(!umi_path_is_within("/work", "/worker"));
#endif
    return 0;
}
