/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_automatic_plan.c
 *
 * PURPOSE:
 *   Verify shared automatic workflow admission without executing build tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/build/ctest_provider.h"
#include "umicom/platform/filesystem.h"

static void Ordinary(const UmiBuildAutomaticPlan *plan, bool configure, bool testing)
{
    size_t index = 0U;
    CHECK(!plan->uses_driver);
    CHECK(plan->phase_count == 1U + (configure ? 1U : 0U) + (testing ? 1U : 0U));
    if (configure) CHECK(plan->phases[index++] == UMI_BUILD_PHASE_CONFIGURE);
    CHECK(plan->phases[index++] == UMI_BUILD_PHASE_BUILD);
    if (testing) CHECK(plan->phases[index++] == UMI_BUILD_PHASE_TEST);
}

int main(void)
{
    char root[UMI_PATH_CAPACITY], build[UMI_PATH_CAPACITY];
    char folder[UMI_PATH_CAPACITY], driver[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(build, root, "configured");
    FixturePath(folder, build, "umicom");
    FixturePath(driver, folder, "automatic-tests-Debug.cmake");
    UmiBuildProfile *profile = calloc(1U, sizeof(*profile));
    CHECK(profile != NULL);
    CHECK(umi_build_profile_set(profile, "admission", root, build) == UMI_STATUS_OK);
    strcpy(profile->configuration, "Debug");
    profile->build_testing = true;
    UmiBuildAutomaticPlan cli = {0}, studio = {0}, later = {0};
    CHECK(UmiBuildAutomaticPlanPrepare(profile, false, &cli) == UMI_STATUS_OK);
    CHECK(UmiBuildAutomaticPlanPrepare(profile, true, &studio) == UMI_STATUS_OK);
    Ordinary(&cli, false, true);
    Ordinary(&studio, true, true);
    CHECK(!umi_fs_is_directory(build));

    /* A fixture creates the file that Configure or Build can produce. Admission
     * must leave existing plans intact; only a later request may use the file. */
    CHECK(umi_fs_make_directories(folder) == UMI_STATUS_OK);
    CHECK(umi_fs_write_text(driver, "# Discovery fixture; never execute.\n") == UMI_STATUS_OK);
    Ordinary(&cli, false, true);
    Ordinary(&studio, true, true);
    CHECK(UmiBuildAutomaticPlanPrepare(profile, false, &later) == UMI_STATUS_OK);
    CHECK(later.uses_driver && later.phase_count == 1U && later.phases[0] == UMI_BUILD_PHASE_TEST);
    CHECK(UmiBuildAutomaticPlanPrepare(profile, true, &later) == UMI_STATUS_OK);
    CHECK(later.uses_driver && later.phase_count == 1U && later.phases[0] == UMI_BUILD_PHASE_TEST);

    /* Explicit presets retain their own build/test semantics even if an
     * unrelated generated driver exists in this profile's build directory. */
    char *presets[] = {profile->preset, profile->configure_preset,
        profile->build_preset, profile->test_preset};
    for (size_t i = 0U; i < sizeof(presets) / sizeof(presets[0]); ++i) {
        strcpy(presets[i], "selected");
        CHECK(UmiBuildAutomaticPlanPrepare(profile, false, &later) == UMI_STATUS_OK);
        Ordinary(&later, false, true);
        CHECK(UmiBuildAutomaticPlanPrepare(profile, true, &later) == UMI_STATUS_OK);
        Ordinary(&later, true, true);
        presets[i][0] = '\0';
    }
    profile->build_testing = false;
    CHECK(UmiBuildAutomaticPlanPrepare(profile, false, &later) == UMI_STATUS_OK);
    Ordinary(&later, false, false);
    CHECK(UmiBuildAutomaticPlanPrepare(profile, true, &later) == UMI_STATUS_OK);
    Ordinary(&later, true, false);

    /* A rejected request cannot replace a caller's previously admitted plan. */
    unsigned char saved[sizeof(later)];
    memcpy(saved, &later, sizeof(saved));
    profile->build_testing = true;
    strcpy(profile->configuration, "Debug/escape");
    CHECK(UmiBuildAutomaticPlanPrepare(profile, false, &later) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(saved, &later, sizeof(saved)) == 0);
    CHECK(UmiBuildAutomaticPlanPrepare(NULL, false, &later) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(saved, &later, sizeof(saved)) == 0);
    CHECK(UmiBuildAutomaticPlanPrepare(profile, false, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    free(profile);
    return 0;
}
