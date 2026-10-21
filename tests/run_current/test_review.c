/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_current/test_review.c
 * PURPOSE: Check actionable launch reviews without changing a profile or starting a program.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiBuildProfile *profile = malloc(sizeof *profile);
    UmiBuildProfile *before = malloc(sizeof *before);
    CHECK(profile != NULL && before != NULL);
    FixtureProfile(profile);
    /* The fixture is deliberately text, not an executable. An accepted review
     * confirms a file exists; only the separate launch service can run it. */
    const char *mode = argv[1];
    const char *expected_text = "No process was started";
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "empty") == 0) {
        profile->run_program[0] = '\0';
        expected = UMI_STATUS_INVALID_STATE;
        expected_text = "Select Program to run";
    } else if (strcmp(mode, "missing") == 0) {
        strcpy(profile->run_program, "./missing-program");
        expected = UMI_STATUS_NOT_FOUND;
        expected_text = "selected program file";
    } else if (strcmp(mode, "directory") == 0) {
        strcpy(profile->run_program, "./");
        expected = UMI_STATUS_NOT_FOUND;
        expected_text = "selected program file";
    } else if (strcmp(mode, "working-folder") == 0) {
        strcpy(profile->run_working_directory, "missing-folder");
        expected = UMI_STATUS_NOT_FOUND;
        expected_text = "Run working folder";
    } else if (strcmp(mode, "source-folder") == 0) {
        char missing[UMI_BUILD_PATH_CAPACITY];
        CHECK(umi_path_join(profile->source_directory, "missing-source-folder", missing,
                            sizeof missing) == UMI_STATUS_OK);
        strcpy(profile->source_directory, missing);
        expected = UMI_STATUS_NOT_FOUND;
        expected_text = "project source folder";
    } else if (strcmp(mode, "bare") == 0) {
        strcpy(profile->run_program, "selected-program");
        expected = UMI_STATUS_INVALID_ARGUMENT;
        expected_text = "explicit program path";
    } else if (strcmp(mode, "environment") == 0) {
        strcpy(profile->run_environment, "INVALID-NAME=private-fixture-value");
        expected = UMI_STATUS_INVALID_ARGUMENT;
        expected_text = "NAME=VALUE";
    } else if (strcmp(mode, "arguments") == 0) {
        strcpy(profile->run_arguments, "'private-fixture-value");
        expected = UMI_STATUS_INVALID_ARGUMENT;
        expected_text = "check quotes";
    } else {
        CHECK(strcmp(mode, "ready") == 0 || strcmp(mode, "buffer") == 0);
    }
    *before = *profile;
    char reason[512];
    memset(reason, 'x', sizeof reason);
    CHECK(UmiBuildRunCurrentReview(profile, reason, sizeof reason) == expected);
    CHECK(memchr(reason, '\0', sizeof reason) != NULL);
    CHECK(strstr(reason, expected_text) != NULL);
    CHECK(strstr(reason, "private-fixture-value") == NULL);
    CHECK(umi_build_profile_equal(profile, before));
    CHECK(UmiBuildRunCurrentReview(profile, NULL, 0U) == expected);
    CHECK(umi_build_profile_equal(profile, before));
    /* The review never edits the selected file, even when its contents cannot
     * be executed. Profile arguments and environment values stay out of feedback. */
    char *file_text = NULL;
    CHECK(umi_fs_read_text("selected-program", &file_text, NULL) == UMI_STATUS_OK);
    CHECK(strcmp(file_text, "controlled executor fixture\n") == 0);
    umi_fs_free_text(file_text);
    if (strcmp(mode, "buffer") == 0) {
        char small[2] = {'x', 'y'};
        CHECK(UmiBuildRunCurrentReview(profile, small, 1U) == UMI_STATUS_OK);
        CHECK(small[0] == '\0' && small[1] == 'y');
        CHECK(UmiBuildRunCurrentReview(profile, small, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildRunCurrentReview(profile, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildRunCurrentReview(NULL, reason, sizeof reason) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(reason[0] != '\0');
        profile->run_program[0] = '\0';
        CHECK(UmiBuildRunCurrentReview(profile, small, 1U) == UMI_STATUS_INVALID_STATE);
        CHECK(small[0] == '\0' && small[1] == 'y');
    }
    free(before);
    free(profile);
    return 0;
}
