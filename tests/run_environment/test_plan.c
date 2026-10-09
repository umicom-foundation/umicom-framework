/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_environment/test_plan.c
 * PURPOSE: Check portable variable syntax, ownership, bounds and explicit PATH precedence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/platform/process_search_path.h"
static const char *Find(const UmiEnvironmentVariable *values, size_t count, const char *name)
{
    for (size_t index = 0U; index < count; ++index)
        if (strcmp(values[index].name, name) == 0)
            return values[index].value;
    return NULL;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    if (strcmp(mode, "invalid") == 0)
    {
        const char *invalid[] = {"NAME",
                                 "=value",
                                 "1NAME=value",
                                 "A-B=value",
                                 "\"A B\"=value",
                                 "A='unterminated",
                                 "A=\"line\nvalue\"",
                                 "A=\xc0\xaf",
                                 "A=\xed\xa0\x80",
                                 "A=\xf4\x90\x80\x80",
                                 "A=\xf0\x9f"};
        for (size_t index = 0U; index < sizeof invalid / sizeof invalid[0]; ++index)
        {
            UmiProcessEnvironmentPlan *plan = NULL;
            CHECK(UmiProcessEnvironmentPlanCreate(invalid[index], NULL, &plan) != UMI_STATUS_OK);
            CHECK(plan == NULL && UmiProcessEnvironmentValidate(invalid[index]) != UMI_STATUS_OK);
        }
    }
    else if (strcmp(mode, "duplicates") == 0)
    {
        CHECK(UmiProcessEnvironmentValidate("MODE=one MODE=two") == UMI_STATUS_ALREADY_EXISTS);
        CHECK(UmiProcessEnvironmentValidate("MODE=one mode=two") == UMI_STATUS_ALREADY_EXISTS);
        CHECK(UmiProcessEnvironmentValidate("PATH=one Path=two") == UMI_STATUS_ALREADY_EXISTS);
    }
    else if (strcmp(mode, "bounds") == 0)
    {
        char large[UMI_PROCESS_ENVIRONMENT_TEXT_CAPACITY + 1U];
        memset(large, 'a', sizeof large);
        large[sizeof large - 1U] = '\0';
        CHECK(UmiProcessEnvironmentValidate(large) == UMI_STATUS_CAPACITY_EXCEEDED);
        strcpy(large, "A=");
        memset(large + 2U, 'x', UMI_PROCESS_ENVIRONMENT_TEXT_CAPACITY - 3U);
        large[UMI_PROCESS_ENVIRONMENT_TEXT_CAPACITY - 1U] = '\0';
        CHECK(UmiProcessEnvironmentValidate(large) == UMI_STATUS_OK);
        char name[80];
        memset(name, 'A', 63U);
        strcpy(name + 63U, "=value");
        CHECK(UmiProcessEnvironmentValidate(name) == UMI_STATUS_OK);
        name[63] = 'A';
        strcpy(name + 64U, "=value");
        CHECK(UmiProcessEnvironmentValidate(name) == UMI_STATUS_CAPACITY_EXCEEDED);
    }
    else if (strcmp(mode, "count") == 0)
    {
        char text[1024] = "";
        size_t used = 0U;
        for (unsigned index = 0U; index < 17U; ++index)
        {
            int length = snprintf(text + used, sizeof text - used, "NAME_%u=value ", index);
            CHECK(length > 0 && (size_t)length < sizeof text - used);
            used += (size_t)length;
            CHECK(UmiProcessEnvironmentValidate(text) ==
                  (index < 16U ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED));
        }
    }
    else
    {
        UmiProcessEnvironmentPlan *plan = NULL;
        const UmiEnvironmentVariable *values = NULL;
        size_t count = 0U;
        if (strcmp(mode, "empty") == 0)
        {
            CHECK(UmiProcessEnvironmentPlanCreate(NULL, NULL, &plan) == UMI_STATUS_OK);
            CHECK(UmiProcessEnvironmentPlanRead(plan, &values, &count) == UMI_STATUS_OK &&
                  count == 0U);
        }
        else if (strcmp(mode, "values") == 0)
        {
            char text[] = "MODE='one two' EMPTY= LITERAL='$HOME;$(never)' UNICODE=caf\xc3\xa9 "
                          "FACE=\xf0\x9f\x98\x80";
            CHECK(UmiProcessEnvironmentPlanCreate(text, NULL, &plan) == UMI_STATUS_OK);
            memset(text, 'x', sizeof text);
            CHECK(UmiProcessEnvironmentPlanRead(plan, &values, &count) == UMI_STATUS_OK &&
                  count == 5U);
            CHECK(strcmp(Find(values, count, "MODE"), "one two") == 0);
            CHECK(strcmp(Find(values, count, "EMPTY"), "") == 0);
            CHECK(strcmp(Find(values, count, "LITERAL"), "$HOME;$(never)") == 0);
            CHECK(strcmp(Find(values, count, "UNICODE"), "caf\xc3\xa9") == 0);
        }
        else if (strcmp(mode, "path") == 0)
        {
            CHECK(UmiProcessEnvironmentPlanCreate("Path=selected", PROJECT_ROOT, &plan) ==
                  UMI_STATUS_OK);
            CHECK(UmiProcessEnvironmentPlanRead(plan, &values, &count) == UMI_STATUS_OK &&
                  count == 1U);
            char *expected = NULL;
            CHECK(UmiProcessSearchPathJoin(PROJECT_ROOT, "selected", &expected) == UMI_STATUS_OK);
            CHECK(strcmp(Find(values, count, "PATH"), expected) == 0);
            UmiProcessSearchPathFree(expected);
        }
        else if (strcmp(mode, "inherited") == 0)
        {
            char *before = NULL, *after = NULL, *expected = NULL;
            CHECK(UmiProcessSearchPathRead(&before) == UMI_STATUS_OK);
            CHECK(UmiProcessEnvironmentPlanCreate("LOCAL_MODE=fixture", PROJECT_ROOT, &plan) ==
                  UMI_STATUS_OK);
            CHECK(UmiProcessEnvironmentPlanRead(plan, &values, &count) == UMI_STATUS_OK &&
                  count == 2U);
            CHECK(UmiProcessSearchPathRead(&after) == UMI_STATUS_OK);
            CHECK(UmiProcessSearchPathJoin(PROJECT_ROOT, before, &expected) == UMI_STATUS_OK);
            CHECK(strcmp(before, after) == 0 && strcmp(Find(values, count, "PATH"), expected) == 0);
            UmiProcessSearchPathFree(before);
            UmiProcessSearchPathFree(after);
            UmiProcessSearchPathFree(expected);
        }
        else
            CHECK(0);
        UmiProcessEnvironmentPlanDestroy(plan);
    }
    CHECK(UmiProcessEnvironmentPlanCreate("", NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
