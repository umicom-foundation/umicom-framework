/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compilation_database/test_limits.c
 * PURPOSE: Exercise bounded compiler database ownership at real document and argument limits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiCompilationDatabase *database = NULL;
    if (strcmp(mode, "rows") == 0 || strcmp(mode, "too-many-rows") == 0)
    {
        size_t count =
            UMI_COMPILATION_DATABASE_ENTRY_LIMIT + (strcmp(mode, "too-many-rows") == 0 ? 1U : 0U);
        const char row[] = DATABASE_ROW;
        size_t capacity = count * (sizeof row + 1U) + 3U;
        char *text = malloc(capacity);
        CHECK(text != NULL);
        size_t used = 0U;
        text[used++] = '[';
        for (size_t i = 0U; i < count; ++i)
        {
            if (i != 0U)
                text[used++] = ',';
            memcpy(text + used, row, sizeof row - 1U);
            used += sizeof row - 1U;
        }
        text[used++] = ']';
        text[used] = '\0';
        UmiStatus status = UmiCompilationDatabaseCreate(text, used, NULL, &database);
        free(text);
        if (count > UMI_COMPILATION_DATABASE_ENTRY_LIMIT)
            CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && database == NULL);
        else
        {
            CHECK(status == UMI_STATUS_OK && UmiCompilationDatabaseCount(database) == count);
            UmiCompilationCommand row_out;
            CHECK(UmiCompilationDatabaseAt(database, count - 1U, &row_out) == UMI_STATUS_OK);
            CHECK(umi_path_equal(row_out.source_file, DATABASE_ROOT "/src/main.c"));
        }
    }
    else if (strcmp(mode, "text") == 0 || strcmp(mode, "too-long-text") == 0)
    {
        size_t length =
            UMI_COMPILATION_DATABASE_TEXT_LIMIT + (strcmp(mode, "too-long-text") == 0 ? 1U : 0U);
        const char prefix[] =
            "[{\"directory\":\"" DATABASE_DIRECTORY "\",\"file\":\"main.c\",\"command\":\"";
        char *text = malloc(sizeof prefix + length + 4U);
        CHECK(text != NULL);
        memcpy(text, prefix, sizeof prefix - 1U);
        memset(text + sizeof prefix - 1U, 'x', length);
        strcpy(text + sizeof prefix - 1U + length, "\"}]");
        UmiStatus status = ParseDatabase(text, &database);
        free(text);
        CHECK(status == (length == UMI_COMPILATION_DATABASE_TEXT_LIMIT
                             ? UMI_STATUS_OK
                             : UMI_STATUS_CAPACITY_EXCEEDED));
        if (status == UMI_STATUS_OK)
        {
            char *command = malloc(length + 1U);
            CHECK(command != NULL);
            CHECK(UmiCompilationDatabaseCommand(database, 0U, command, length + 1U) ==
                  UMI_STATUS_OK);
            CHECK(strlen(command) == length);
            free(command);
        }
    }
    else if (strcmp(mode, "arguments") == 0 || strcmp(mode, "too-many-arguments") == 0)
    {
        size_t count = UMI_COMPILATION_DATABASE_ARGUMENT_LIMIT +
                       (strcmp(mode, "too-many-arguments") == 0 ? 1U : 0U);
        char *text = malloc(256U + 5U * count);
        CHECK(text != NULL);
        strcpy(text, "[{\"directory\":\"" DATABASE_DIRECTORY
                     "\",\"file\":\"main.c\",\"arguments\":[\"cc\"");
        size_t used = strlen(text);
        for (size_t i = 1U; i < count; ++i)
        {
            memcpy(text + used, ",\"x\"", 4U);
            used += 4U;
        }
        strcpy(text + used, "]}]");
        UmiStatus status = ParseDatabase(text, &database);
        free(text);
        CHECK(status == (count == UMI_COMPILATION_DATABASE_ARGUMENT_LIMIT
                             ? UMI_STATUS_OK
                             : UMI_STATUS_CAPACITY_EXCEEDED));
        if (status == UMI_STATUS_OK)
        {
            char value[4];
            CHECK(UmiCompilationDatabaseArgument(database, 0U, count - 1U, value, sizeof value) ==
                  UMI_STATUS_OK);
            CHECK(strcmp(value, "x") == 0);
        }
    }
    else
        return 2;
    UmiCompilationDatabaseDestroy(database);
    return EXIT_SUCCESS;
}
