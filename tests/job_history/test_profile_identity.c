/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/job_history/test_profile_identity.c
 * PURPOSE: Check build settings isolation, framing, root resolution and bounded failures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/job_history.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(x))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiBuildProfile *profile = malloc(sizeof(*profile)), *changed = malloc(sizeof(*changed));
    CHECK(profile != NULL && changed != NULL);
    umi_build_profile_init(profile);
    UmiJobIdentity first, second, sentinel;
    CHECK(UmiBuildProfileJobIdentity(profile, &first) == UMI_STATUS_OK);
    CHECK(UmiJobIdentityIsRecorded(&first) && first.inputs[0] == '\0');
    if (strcmp(argv[1], "settings") == 0)
    {
        /* Every setting that can redirect a tool, select a target or change
         * execution must invalidate the old settings comparison. */
        const size_t fields[] = {offsetof(UmiBuildProfile, profile_id),
                                 offsetof(UmiBuildProfile, build_directory),
                                 offsetof(UmiBuildProfile, generator),
                                 offsetof(UmiBuildProfile, compiler),
                                 offsetof(UmiBuildProfile, configuration),
                                 offsetof(UmiBuildProfile, preset),
                                 offsetof(UmiBuildProfile, build_target),
                                 offsetof(UmiBuildProfile, run_program),
                                 offsetof(UmiBuildProfile, run_argument),
                                 offsetof(UmiBuildProfile, install_directory),
                                 offsetof(UmiBuildProfile, run_arguments),
                                 offsetof(UmiBuildProfile, configure_preset),
                                 offsetof(UmiBuildProfile, build_preset),
                                 offsetof(UmiBuildProfile, test_preset),
                                 offsetof(UmiBuildProfile, run_working_directory)};
        for (size_t i = 0U; i < sizeof(fields) / sizeof(fields[0]); ++i)
        {
            *changed = *profile;
            strcpy((char *)changed + fields[i], "other");
            CHECK(UmiBuildProfileJobIdentity(changed, &second) == UMI_STATUS_OK);
            CHECK(UmiJobIdentityCompare(&first, &second) ==
                  UMI_JOB_IDENTITY_DIFFERENT_CONFIGURATION);
        }
        for (unsigned i = 0U; i < 4U; ++i)
        {
            *changed = *profile;
            if (i == 0U)
                changed->parallel_jobs = 2U;
            else if (i == 1U)
                changed->timeout_ms = 42U;
            else if (i == 2U)
                changed->build_testing = 0;
            else
                changed->strict_warnings = 0;
            CHECK(UmiBuildProfileJobIdentity(changed, &second) == UMI_STATUS_OK);
            CHECK(UmiJobIdentityCompare(&first, &second) ==
                  UMI_JOB_IDENTITY_DIFFERENT_CONFIGURATION);
        }
    }
    else if (strcmp(argv[1], "framing") == 0)
    {
        strcpy(profile->profile_id, "ab");
        strcpy(profile->generator, "c");
        *changed = *profile;
        strcpy(changed->profile_id, "a");
        strcpy(changed->generator, "bc");
        CHECK(UmiBuildProfileJobIdentity(profile, &first) == UMI_STATUS_OK);
        CHECK(UmiBuildProfileJobIdentity(changed, &second) == UMI_STATUS_OK);
        CHECK(UmiJobIdentityCompare(&first, &second) == UMI_JOB_IDENTITY_DIFFERENT_CONFIGURATION);
    }
    else if (strcmp(argv[1], "padding") == 0)
    {
        *changed = *profile;
        memset(changed->profile_id + strlen(changed->profile_id) + 1U, 0x5a,
               sizeof(changed->profile_id) - strlen(changed->profile_id) - 1U);
        CHECK(UmiBuildProfileJobIdentity(changed, &second) == UMI_STATUS_OK);
        CHECK(strcmp(first.configuration, second.configuration) == 0);
    }
    else if (strcmp(argv[1], "root") == 0)
    {
        char absolute[UMI_BUILD_PATH_CAPACITY];
        CHECK(UmiBuildProfileSourceDirectory(profile, absolute, sizeof(absolute)) == UMI_STATUS_OK);
        CHECK(umi_path_is_absolute(absolute));
        strcpy(profile->source_directory, absolute);
        CHECK(UmiBuildProfileJobIdentity(profile, &second) == UMI_STATUS_OK);
        CHECK(strcmp(first.subject, second.subject) == 0 &&
              strcmp(first.configuration, second.configuration) == 0);
        CHECK(umi_path_join(absolute, "other-project", profile->source_directory,
                            sizeof(profile->source_directory)) == UMI_STATUS_OK);
        CHECK(UmiBuildProfileJobIdentity(profile, &second) == UMI_STATUS_OK);
        CHECK(UmiJobIdentityCompare(&first, &second) == UMI_JOB_IDENTITY_DIFFERENT_SUBJECT);
    }
    else if (strcmp(argv[1], "invalid") == 0)
    {
        memset(&sentinel, 0x5a, sizeof(sentinel));
        memcpy(&second, &sentinel, sizeof(second));
        memset(profile->source_directory, 'x', sizeof(profile->source_directory));
        CHECK(UmiBuildProfileJobIdentity(profile, &second) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&second, &sentinel, sizeof(second)) == 0);
        umi_build_profile_init(profile);
        char small[2] = "x";
        CHECK(UmiBuildProfileSourceDirectory(profile, small, sizeof(small)) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(small, "x") == 0);
    }
    else if (strcmp(argv[1], "source-unknown") == 0)
    {
        CHECK(UmiBuildProfileJobIdentity(profile, &second) == UMI_STATUS_OK);
        CHECK(UmiJobIdentityCompare(&first, &second) == UMI_JOB_IDENTITY_INPUTS_UNRECORDED);
    }
    else
        return 2;
    free(changed);
    free(profile);
    return 0;
}
