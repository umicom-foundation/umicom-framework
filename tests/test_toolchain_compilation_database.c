/*-----------------------------------------------------------------------------
 * Umicom Framework - Batch 35 compilation database import regression test.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/toolchain/compilation_database.h"

int main(void)
{
    UmiCompilationDatabase *database = NULL;
    UmiCompilationDatabaseSnapshot snapshot;
    UmiCompilationCommandSnapshot command;
    char temporary[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    char build_directory[UMI_PATH_CAPACITY];
    char conventional_path[UMI_PATH_CAPACITY];
    char discovered_path[UMI_PATH_CAPACITY];
    if (umi_fs_temp_directory(temporary, sizeof(temporary)) != UMI_STATUS_OK ||
        umi_path_join(temporary, "umicom-b35-compile-commands.json", path,
                      sizeof(path)) != UMI_STATUS_OK) return 1;
    if (umi_fs_write_text(path,
        "[\n"
        " {\"directory\":\"/src/build\",\"command\":\"clang -c /src/a.c\",\"file\":\"/src/a.c\",\"output\":\"a.o\"},\n"
        " {\"directory\":\"/src/build\",\"arguments\":[\"gcc\",\"-c\",\"/src/b.c\"],\"file\":\"/src/b.c\"}\n"
        "]\n") != UMI_STATUS_OK) return 2;
    if (umi_compilation_database_create(&database) != UMI_STATUS_OK ||
        umi_compilation_database_load(database, path) != UMI_STATUS_OK) return 3;
    if (umi_compilation_database_snapshot(database, &snapshot) != UMI_STATUS_OK ||
        snapshot.command_count != 2U || snapshot.clang_command_count != 1U ||
        snapshot.gcc_command_count != 1U) return 4;
    if (umi_compilation_database_find_file(database, "/src/b.c", &command) !=
            UMI_STATUS_OK ||
        command.compiler_vendor != UMI_COMPILER_VENDOR_GCC) return 5;
    if (umi_path_join(temporary, "build", build_directory,
                      sizeof(build_directory)) != UMI_STATUS_OK ||
        umi_fs_make_directories(build_directory) != UMI_STATUS_OK ||
        umi_path_join(build_directory, "compile_commands.json",
                      conventional_path, sizeof(conventional_path)) !=
            UMI_STATUS_OK ||
        umi_fs_write_text(conventional_path, "[]\n") != UMI_STATUS_OK ||
        umi_compilation_database_discover(temporary, NULL, 0U,
            discovered_path, sizeof(discovered_path)) != UMI_STATUS_OK ||
        !umi_path_equal(discovered_path, conventional_path) ||
        umi_compilation_database_load(database, discovered_path) !=
            UMI_STATUS_OK ||
        umi_compilation_database_snapshot(database, &snapshot) !=
            UMI_STATUS_OK || snapshot.command_count != 0U) return 6;
    umi_compilation_database_destroy(database);
    if (umi_fs_remove_tree(path) != UMI_STATUS_OK ||
        umi_fs_remove_tree(build_directory) != UMI_STATUS_OK) return 7;
    return EXIT_SUCCESS;
}
