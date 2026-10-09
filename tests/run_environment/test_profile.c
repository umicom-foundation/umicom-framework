/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_environment/test_profile.c
 * PURPOSE: Bind queued job identity and profile equality to reviewed launch variables.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/build/job_history.h"
int main(void)
{
    UmiBuildProfile original, changed;
    umi_build_profile_init(&original);
    strcpy(original.source_directory, PROJECT_ROOT);
    changed = original;
    UmiJobIdentity before, after;
    CHECK(UmiBuildProfileJobIdentity(&original, &before) == UMI_STATUS_OK);
    strcpy(changed.run_environment, "APP_MODE=one");
    CHECK(!umi_build_profile_equal(&original, &changed));
    CHECK(UmiBuildProfileJobIdentity(&changed, &after) == UMI_STATUS_OK);
    CHECK(strcmp(before.subject, after.subject) == 0);
    CHECK(strcmp(before.configuration, after.configuration) != 0);
    changed.run_environment[0] = '\0';
    CHECK(UmiBuildProfileJobIdentity(&changed, &after) == UMI_STATUS_OK);
    CHECK(strcmp(before.configuration, after.configuration) == 0);
    memset(changed.run_environment, 'x', sizeof changed.run_environment);
    CHECK(umi_build_profile_validate(&changed, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!umi_build_profile_equal(&changed, &changed));
    return 0;
}
