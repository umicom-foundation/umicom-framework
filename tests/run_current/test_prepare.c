/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_current/test_prepare.c
 * PURPOSE: Check explicit launch resolution and failure without mutating caller-owned settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiBuildProfile *input = malloc(sizeof *input), *out = malloc(sizeof *out);
    CHECK(input != NULL && out != NULL);
    FixtureProfile(input);
    *out = *input;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(argv[1], "bare") == 0)
    {
        strcpy(input->run_program, "selected-program");
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(argv[1], "missing") == 0)
    {
        strcpy(input->run_program, "./missing-program");
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(argv[1], "directory") == 0)
    {
        strcpy(input->run_program, "./");
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(argv[1], "working-folder") == 0)
    {
        strcpy(input->run_working_directory, "missing-folder");
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(argv[1], "empty") == 0)
    {
        input->run_program[0] = '\0';
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (strcmp(argv[1], "environment") == 0)
    {
        strcpy(input->run_environment, "BAD-NAME=value");
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else
        CHECK(strcmp(argv[1], "relative") == 0 || strcmp(argv[1], "alias") == 0);
    UmiBuildProfile *before = malloc(sizeof *before);
    CHECK(before != NULL);
    *before = *out;
    if (strcmp(argv[1], "alias") == 0)
    {
        CHECK(UmiBuildRunCurrentPrepare(input, input) == UMI_STATUS_OK);
        *out = *input;
    }
    else
        CHECK(UmiBuildRunCurrentPrepare(input, out) == expected);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(umi_path_is_absolute(out->run_program));
        CHECK(umi_path_equal(out->run_working_directory, out->source_directory));
        CHECK(strcmp(out->run_arguments, "'argument with spaces'") == 0);
        CHECK(strcmp(out->run_environment, "UMICOM_RUN_VALUE=selected") == 0);
        CHECK(umi_fs_is_file(out->run_program));
    }
    else
        CHECK(umi_build_profile_equal(out, before));
    free(before);
    free(out);
    free(input);
    return 0;
}
