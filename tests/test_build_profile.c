/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_profile.c
 *
 * PURPOSE:
 *   Verify build-profile defaults, validation and equality.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/build/profile.h"
#include "umicom/platform/path.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* These roots need not exist: resolving a profile must neither create folders
 * nor depend on the process directory or an installed compiler. */
static void TestBuildDirectory(void)
{
#ifdef _WIN32
    const char *root = "C:/Projects/Notes";
    const char *expected = "C:/Projects/Notes/build/debug";
    const char *outside = "D:/Builds/Notes";
    const char *parent = "C:/Projects/shared-build";
#else
    const char *root = "/projects/Notes";
    const char *expected = "/projects/Notes/build/debug";
    const char *outside = "/builds/Notes";
    const char *parent = "/projects/shared-build";
#endif
    UmiBuildProfile profile;
    umi_build_profile_init(&profile);
    strcpy(profile.source_directory, root);
    strcpy(profile.build_directory, "build/./debug");
    UmiBuildProfile original = profile;
    char output[UMI_BUILD_PATH_CAPACITY] = "retained";
    assert(UmiBuildProfileBuildDirectory(&profile, root, output, sizeof output) == UMI_STATUS_OK);
    assert(umi_path_equal(output, expected) && umi_path_is_absolute(output));
    assert(umi_build_profile_equal(&profile, &original));
    strcpy(profile.build_directory, outside);
    assert(UmiBuildProfileBuildDirectory(&profile, root, output, sizeof output) == UMI_STATUS_OK);
    assert(umi_path_equal(output, outside));
    strcpy(profile.build_directory, "../shared-build");
    assert(UmiBuildProfileBuildDirectory(&profile, root, output, sizeof output) == UMI_STATUS_OK);
    assert(umi_path_equal(output, parent));
    strcpy(output, "retained");
    assert(UmiBuildProfileBuildDirectory(&profile, root, output, 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(strcmp(output, "retained") == 0);
    assert(UmiBuildProfileBuildDirectory(&profile, "relative-root", output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
    assert(strcmp(output, "retained") == 0);
    assert(UmiBuildProfileBuildDirectory(NULL, root, output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiBuildProfileBuildDirectory(&profile, root, NULL, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiBuildProfileBuildDirectory(&profile, root, output, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    profile.build_directory[0] = '\0';
    assert(UmiBuildProfileBuildDirectory(&profile, root, output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
    memset(profile.build_directory, 'x', sizeof profile.build_directory);
    assert(UmiBuildProfileBuildDirectory(&profile, root, output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
    assert(strcmp(output, "retained") == 0);
#ifdef _WIN32
    strcpy(profile.build_directory, "C:build");
    assert(UmiBuildProfileBuildDirectory(&profile, root, output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
    strcpy(profile.build_directory, "/build");
    assert(UmiBuildProfileBuildDirectory(&profile, root, output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
    assert(strcmp(output, "retained") == 0);
#endif
}

/* This check is intentionally independent of filesystem existence and trust. */
static void TestProjectOwnership(void)
{
#ifdef _WIN32
    const char *root = "C:/Projects/Notes";
    const char *equivalent = "C:/Projects/./Notes";
    const char *other = "C:/Projects/Other";
#else
    const char *root = "/projects/Notes";
    const char *equivalent = "/projects/./Notes";
    const char *other = "/projects/Other";
#endif
    UmiBuildProfile profile;
    umi_build_profile_init(&profile);
    strcpy(profile.source_directory, root);
    UmiBuildProfile before = profile;
    assert(UmiBuildProfileCheckProject(&profile, root) == UMI_STATUS_OK);
    assert(UmiBuildProfileCheckProject(&profile, equivalent) == UMI_STATUS_OK);
    assert(UmiBuildProfileCheckProject(&profile, other) == UMI_STATUS_INVALID_STATE);
    assert(umi_build_profile_equal(&profile, &before));
    assert(UmiBuildProfileCheckProject(&profile, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiBuildProfileCheckProject(&profile, "relative") == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiBuildProfileCheckProject(NULL, root) == UMI_STATUS_INVALID_ARGUMENT);
    strcpy(profile.source_directory, ".");
    assert(UmiBuildProfileCheckProject(&profile, root) == UMI_STATUS_INVALID_STATE);
    memset(profile.source_directory, 'x', sizeof profile.source_directory);
    assert(UmiBuildProfileCheckProject(&profile, root) == UMI_STATUS_INVALID_ARGUMENT);
}

int main(void)
{
    UmiBuildProfile a;
    UmiBuildProfile b;
    char message[256];
    umi_build_profile_init(&a);
    assert(umi_build_profile_set(&a, "debug", ".", "build/debug") == UMI_STATUS_OK);
    assert(umi_build_profile_validate(&a, message, sizeof(message)) == UMI_STATUS_OK);
    b = a;
    assert(umi_build_profile_equal(&a, &b));
    b.parallel_jobs += 1U;
    assert(!umi_build_profile_equal(&a, &b));
    /* Stored profiles normalise paths. Equivalent source spellings must not
     * invalidate a settings round trip, while a different project still must. */
    b = a;
    strcpy(a.source_directory, "project/source");
    strcpy(b.source_directory, "project/./source");
    assert(umi_build_profile_equal(&a, &b));
#ifdef _WIN32
    strcpy(b.source_directory, "project\\source");
    assert(umi_build_profile_equal(&a, &b));
#endif
    strcpy(b.source_directory, "project/another-source");
    assert(!umi_build_profile_equal(&a, &b));
    TestBuildDirectory();
    TestProjectOwnership();
    return 0;
}
