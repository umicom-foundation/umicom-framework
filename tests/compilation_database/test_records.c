/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compilation_database/test_records.c
 * PURPOSE: Verify compiler-command variants, decoded arguments and unchanged outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiCompilationDatabase *database = NULL;
    if (strcmp(mode, "empty") == 0)
    {
        CHECK(ParseDatabase("[]", &database) == UMI_STATUS_OK);
        CHECK(UmiCompilationDatabaseCount(database) == 0U);
    }
    else if (strcmp(mode, "command") == 0)
    {
        CHECK(ParseDatabase("[{\"directory\":\"" DATABASE_DIRECTORY "\",\"file\":\"main.c\","
                            "\"command\":\"cc -DNAME=\\\"two words\\\" main.c > forbidden.txt\"}]",
                            &database) == UMI_STATUS_OK);
        UmiCompilationCommand row;
        char text[256] = "unchanged";
        CHECK(UmiCompilationDatabaseAt(database, 0U, &row) == UMI_STATUS_OK);
        CHECK(row.has_command && !row.has_arguments && row.argument_count == 0U);
        CHECK(UmiCompilationDatabaseArgument(database, 0U, 0U, text, sizeof text) ==
              UMI_STATUS_NOT_FOUND);
        CHECK(strcmp(text, "unchanged") == 0);
        CHECK(UmiCompilationDatabaseCommand(database, 0U, text, sizeof text) == UMI_STATUS_OK);
        CHECK(strcmp(text, "cc -DNAME=\"two words\" main.c > forbidden.txt") == 0);
    }
    else if (strcmp(mode, "variants") == 0)
    {
        CHECK(ParseDatabase("[" DATABASE_ROW "," DATABASE_ROW "]", &database) == UMI_STATUS_OK);
        size_t first = 99U, second = 99U, missing = 99U;
        CHECK(UmiCompilationDatabaseCount(database) == 2U);
        CHECK(UmiCompilationDatabaseFind(database, DATABASE_ROOT "/src/./main.c", 0U, &first) ==
              UMI_STATUS_OK);
        CHECK(first == 0U);
        CHECK(UmiCompilationDatabaseFind(database, DATABASE_ROOT "/src/main.c", first + 1U,
                                         &second) == UMI_STATUS_OK);
        CHECK(second == 1U);
        CHECK(UmiCompilationDatabaseFind(database, DATABASE_ROOT "/src/main.c", second + 1U,
                                         &missing) == UMI_STATUS_NOT_FOUND);
        CHECK(missing == 99U);
        CHECK(UmiCompilationDatabaseFind(database, "main.c", 0U, &missing) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "unicode") == 0)
    {
        CHECK(ParseDatabase("[{\"directory\":\"" DATABASE_ROOT "/caf\\u00e9\","
                            "\"file\":\"\\u6587.c\",\"output\":\"obj/\\u6587.o\","
                            "\"arguments\":[\"cc\",\"\\u6587.c\"],\"command\":\"cc \\u6587.c\"}]",
                            &database) == UMI_STATUS_OK);
        UmiCompilationCommand row;
        char text[128];
        CHECK(UmiCompilationDatabaseAt(database, 0U, &row) == UMI_STATUS_OK);
        CHECK(umi_path_equal(row.source_file, DATABASE_ROOT "/caf\xc3\xa9/\xe6\x96\x87.c"));
        CHECK(umi_path_equal(row.output_file, DATABASE_ROOT "/caf\xc3\xa9/obj/\xe6\x96\x87.o"));
        CHECK(row.has_arguments && row.has_command);
        CHECK(UmiCompilationDatabaseArgument(database, 0U, 1U, text, sizeof text) == UMI_STATUS_OK);
        CHECK(strcmp(text, "\xe6\x96\x87.c") == 0);
    }
    else if (strcmp(mode, "arguments") == 0 || strcmp(mode, "output-preserved") == 0)
    {
        CHECK(ParseDatabase("[" DATABASE_ROW "]", &database) == UMI_STATUS_OK);
        UmiCompilationCommand row = {0};
        unsigned char previous[sizeof row];
        char value[128] = "unchanged";
        CHECK(UmiCompilationDatabaseAt(database, 0U, &row) == UMI_STATUS_OK);
        CHECK(row.argument_count == 5U && row.has_arguments && !row.has_command);
        CHECK(umi_path_equal(row.directory, DATABASE_DIRECTORY));
        CHECK(umi_path_equal(row.source_file, DATABASE_ROOT "/src/main.c"));
        CHECK(row.output_file[0] == '\0');
        CHECK(UmiCompilationDatabaseArgument(database, 0U, 2U, value, sizeof value) ==
              UMI_STATUS_OK);
        CHECK(strcmp(value, "-DNAME=a b") == 0);
        CHECK(UmiCompilationDatabaseArgument(database, 0U, 3U, value, sizeof value) ==
              UMI_STATUS_OK);
        CHECK(value[0] == '\0');
        strcpy(value, "unchanged");
        CHECK(UmiCompilationDatabaseArgument(database, 0U, 2U, value, 2U) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(value, "unchanged") == 0);
        CHECK(UmiCompilationDatabaseCommand(database, 0U, value, sizeof value) ==
              UMI_STATUS_NOT_FOUND);
        CHECK(strcmp(value, "unchanged") == 0);
        memcpy(previous, &row, sizeof row);
        CHECK(UmiCompilationDatabaseAt(database, 1U, &row) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&row, previous, sizeof row) == 0);
    }
    else
        return 2;
    UmiCompilationDatabaseDestroy(database);
    return EXIT_SUCCESS;
}
