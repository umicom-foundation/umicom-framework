/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/configure_definitions/test_profile_initialization.c
 * PURPOSE: Ensure failed or aliased profile initialization cannot erase accepted project settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/build/profile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)
int main(int argc, char **argv)
{
    int failed = 0;
    UmiBuildProfile *profile = malloc(sizeof *profile), *before = malloc(sizeof *before);
    CHECK(argc == 2 && profile != NULL && before != NULL);
    umi_build_profile_init(profile);
    strcpy(profile->profile_id, "accepted"); strcpy(profile->source_directory, "project");
    strcpy(profile->build_directory, "output"); strcpy(profile->compiler, "preserved-compiler");
    *before = *profile;
    const char *mode = argv[1];
    if (strcmp(mode, "alias") == 0) {
        CHECK(umi_build_profile_set(profile, profile->profile_id, profile->source_directory,
            profile->build_directory) == UMI_STATUS_OK);
        CHECK(strcmp(profile->profile_id, "accepted") == 0 && strcmp(profile->source_directory, "project") == 0 &&
            strcmp(profile->build_directory, "output") == 0);
        CHECK(profile->compiler[0] == '\0' && profile->parallel_jobs == 1U);
    } else {
        char path[UMI_BUILD_PATH_CAPACITY + 1U]; memset(path, 'x', sizeof path - 1U); path[sizeof path - 1U] = '\0';
        const char *id = "new", *source = "new-source", *build = "new-build";
        UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(mode, "null_id") == 0) id = NULL;
        else if (strcmp(mode, "null_source") == 0) source = NULL;
        else if (strcmp(mode, "null_build") == 0) build = NULL;
        else {
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
            if (strcmp(mode, "long_id") == 0) id = path;
            else if (strcmp(mode, "long_source") == 0) source = path;
            else { CHECK(strcmp(mode, "long_build") == 0); build = path; }
        }
        CHECK(umi_build_profile_set(profile, id, source, build) == expected);
        CHECK(memcmp(profile, before, sizeof *profile) == 0);
    }
cleanup:
    free(before); free(profile); return failed;
}
