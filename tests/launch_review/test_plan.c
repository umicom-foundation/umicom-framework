/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/launch_review/test_plan.c
 * PURPOSE: Verify owned launch descriptions, path adoption and preserved lookup behaviour.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/launch_plan.h"
#include "umicom/build/cmake_provider.h"
#include <stdio.h>
#include <string.h>
#define CHECK(test)                                                                                          \
    do                                                                                                       \
    {                                                                                                        \
        if (!(test))                                                                                         \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test);                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
#ifdef _WIN32
#define ROOT "C:/umicom-launch/project"
#define OTHER "C:/umicom-launch/programs/notes.exe"
#else
#define ROOT "/umicom-launch/project"
#define OTHER "/umicom-launch/programs/notes"
#endif
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiBuildProfile profile, selected, before;
    UmiBuildLaunchPlan plan, saved;
    umi_build_profile_init(&profile);
    strcpy(profile.source_directory, ROOT);
    strcpy(profile.run_program, "bin/notes");
    memset(&plan, 0x5a, sizeof(plan));
    memcpy(&saved, &plan, sizeof(saved));
    if (strcmp(mode, "relative") == 0 || strcmp(mode, "owned") == 0 || strcmp(mode, "provider") == 0)
    {
        strcpy(profile.run_arguments, "--file \"notes for review.txt\" \"\" 'caf\xc3\xa9' '\"quoted\"'");
        strcpy(profile.run_working_directory, "sample data");
        CHECK(UmiBuildLaunchPlanCreate(&profile, ROOT, &plan) == UMI_STATUS_OK);
        CHECK(strcmp(plan.run_program, ROOT "/bin/notes") == 0 &&
              strcmp(plan.debug_program, plan.run_program) == 0);
        CHECK(strcmp(plan.working_directory, ROOT "/sample data") == 0 && !plan.run_uses_program_lookup);
        CHECK(plan.argument_count == 5U && strcmp(plan.arguments[0], "--file") == 0);
        CHECK(strcmp(plan.arguments[1], "notes for review.txt") == 0 && plan.arguments[2][0] == '\0');
        CHECK(strcmp(plan.arguments[3], "caf\xc3\xa9") == 0 && strcmp(plan.arguments[4], "\"quoted\"") == 0);
        if (strcmp(mode, "owned") == 0)
        {
            saved = plan;
            memset(&plan, 0, sizeof(plan));
            memset(&profile, 0, sizeof(profile));
            CHECK(strcmp(saved.arguments[1], "notes for review.txt") == 0 &&
                  strcmp(saved.debug_program, ROOT "/bin/notes") == 0);
        }
        if (strcmp(mode, "provider") == 0)
        {
            UmiBuildProvider provider = umi_build_cmake_provider();
            UmiBuildCommand command;
            strcpy(profile.run_program, plan.run_program);
            CHECK(umi_build_provider_create_command(&provider, &profile, UMI_BUILD_PHASE_RUN, &command) ==
                  UMI_STATUS_OK);
            CHECK(strcmp(command.program, plan.run_program) == 0 &&
                  strcmp(command.working_directory, plan.working_directory) == 0);
            CHECK(command.argument_count == plan.argument_count);
            for (size_t i = 0U; i < plan.argument_count; ++i)
                CHECK(strcmp(command.arguments[i], plan.arguments[i]) == 0);
        }
    }
    else if (strcmp(mode, "bare") == 0)
    {
        strcpy(profile.run_program, "notes");
        CHECK(UmiBuildLaunchPlanCreate(&profile, ROOT, &plan) == UMI_STATUS_OK);
        CHECK(plan.run_uses_program_lookup && strcmp(plan.run_program, "notes") == 0);
        CHECK(strcmp(plan.debug_program, ROOT "/notes") == 0 && strcmp(plan.working_directory, ROOT) == 0);
    }
    else if (strcmp(mode, "dot-relative") == 0)
    {
        strcpy(profile.run_program, "./notes");
        CHECK(UmiBuildLaunchPlanCreate(&profile, ROOT, &plan) == UMI_STATUS_OK);
        CHECK(!plan.run_uses_program_lookup && strcmp(plan.run_program, ROOT "/notes") == 0);
    }
    else if (strcmp(mode, "absolute") == 0)
    {
        strcpy(profile.run_program, OTHER);
        strcpy(profile.run_working_directory, ROOT "/data");
        strcpy(profile.run_argument, "--two words");
        CHECK(UmiBuildLaunchPlanCreate(&profile, ROOT, &plan) == UMI_STATUS_OK);
        CHECK(!plan.run_uses_program_lookup && strcmp(plan.run_program, OTHER) == 0 &&
              strcmp(plan.debug_program, OTHER) == 0);
        CHECK(plan.argument_count == 1U && strcmp(plan.arguments[0], "--two words") == 0);
    }
    else if (strcmp(mode, "missing") == 0 || strcmp(mode, "conflict") == 0 ||
             strcmp(mode, "unterminated") == 0 || strcmp(mode, "bad-quotes") == 0 ||
             strcmp(mode, "relative-root") == 0 || strcmp(mode, "null") == 0 ||
             strcmp(mode, "drive-relative") == 0 || strcmp(mode, "rooted-program") == 0)
    {
        UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
        const char *root = ROOT;
        if (strcmp(mode, "missing") == 0)
        {
            profile.run_program[0] = '\0';
            expected = UMI_STATUS_INVALID_STATE;
        }
        if (strcmp(mode, "conflict") == 0)
        {
            strcpy(profile.run_argument, "literal");
            strcpy(profile.run_arguments, "--flag");
        }
        if (strcmp(mode, "unterminated") == 0)
            memset(profile.run_program, 'p', sizeof(profile.run_program));
        if (strcmp(mode, "bad-quotes") == 0)
        {
            strcpy(profile.run_arguments, "\"unfinished");
            expected = UMI_STATUS_PARSE_ERROR;
        }
        if (strcmp(mode, "relative-root") == 0)
            root = "relative/root";
        if (strcmp(mode, "null") == 0)
        {
            CHECK(UmiBuildLaunchPlanCreate(NULL, ROOT, &plan) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiBuildLaunchPlanCreate(&profile, ROOT, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiBuildLaunchSelectPath(&profile, UMI_BUILD_LAUNCH_PROGRAM, OTHER, NULL) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            root = NULL;
        }
        if (strcmp(mode, "drive-relative") == 0 || strcmp(mode, "rooted-program") == 0)
        {
#ifdef _WIN32
            strcpy(profile.run_program, strcmp(mode, "drive-relative") == 0 ? "C:notes.exe" : "\\notes.exe");
#else
            return 77;
#endif
        }
        CHECK(UmiBuildLaunchPlanCreate(&profile, root, &plan) == expected);
        CHECK(memcmp(&plan, &saved, sizeof(plan)) == 0);
    }
    else if (strcmp(mode, "select-program") == 0 || strcmp(mode, "select-folder") == 0 ||
             strcmp(mode, "select-inplace") == 0)
    {
        UmiBuildLaunchPathKind kind =
            strcmp(mode, "select-folder") == 0 ? UMI_BUILD_LAUNCH_DIRECTORY : UMI_BUILD_LAUNCH_PROGRAM;
        before = profile;
        strcpy(kind == UMI_BUILD_LAUNCH_PROGRAM ? before.run_program : before.run_working_directory, OTHER);
        CHECK(UmiBuildLaunchSelectPath(&profile, kind, OTHER, &selected) == UMI_STATUS_OK);
        CHECK(umi_build_profile_equal(&selected, &before));
        if (strcmp(mode, "select-inplace") == 0)
        {
            CHECK(UmiBuildLaunchSelectPath(&profile, kind, OTHER, &profile) == UMI_STATUS_OK);
            CHECK(umi_build_profile_equal(&profile, &before));
        }
    }
    else if (strcmp(mode, "select-invalid") == 0)
    {
        selected = profile;
        memcpy(&before, &selected, sizeof(before));
        CHECK(UmiBuildLaunchSelectPath(&profile, UMI_BUILD_LAUNCH_PROGRAM, "relative", &selected) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildLaunchSelectPath(&profile, (UmiBuildLaunchPathKind)99, OTHER, &selected) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildLaunchSelectPath(&profile, UMI_BUILD_LAUNCH_PROGRAM, ROOT "/bad\nname", &selected) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&before, &selected, sizeof(before)) == 0);
    }
    else if (strcmp(mode, "control-path") == 0)
    {
        strcpy(profile.run_program, "bin/notes\nother");
        CHECK(UmiBuildLaunchPlanCreate(&profile, ROOT, &plan) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&plan, &saved, sizeof(plan)) == 0);
    }
    else if (strcmp(mode, "capacity") == 0)
    {
        char path[UMI_BUILD_PATH_CAPACITY + 1U];
        memset(path, 'p', sizeof(path) - 1U);
        path[sizeof(path) - 1U] = '\0';
        memcpy(path, ROOT "/", strlen(ROOT "/"));
        selected = profile;
        memcpy(&before, &selected, sizeof(before));
        CHECK(UmiBuildLaunchSelectPath(&profile, UMI_BUILD_LAUNCH_PROGRAM, path, &selected) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&before, &selected, sizeof(before)) == 0);
    }
    else
        CHECK(0);
    return 0;
}
