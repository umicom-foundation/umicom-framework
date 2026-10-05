/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/installed_files/fixture.h
 * PURPOSE: Create owned install-manifest fixtures without running an installer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INSTALLED_FILES_FIXTURE_H
#define UMICOM_INSTALLED_FILES_FIXTURE_H
#include "umicom/developer_project/installed_files.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/stat.h>
#endif
typedef struct InstalledFixture
{
    UmiBuildProfile profile;
    char manifest[UMI_PATH_CAPACITY], program[UMI_PATH_CAPACITY], data[UMI_PATH_CAPACITY];
} InstalledFixture;
static UmiStatus InstalledFixtureCreate(const char *root, InstalledFixture *fixture)
{
    memset(fixture, 0, sizeof(*fixture));
    umi_build_profile_init(&fixture->profile);
    strcpy(fixture->profile.source_directory, root);
    strcpy(fixture->profile.build_directory, "build");
    strcpy(fixture->profile.install_directory, "installed");
    strcpy(fixture->profile.run_program, "previous-program");
    char build[UMI_PATH_CAPACITY], prefix[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_join(root, "build", build, sizeof(build));
    if (status == UMI_STATUS_OK)
        status = umi_path_join(root, "installed", prefix, sizeof(prefix));
    if (status == UMI_STATUS_OK)
        status = umi_fs_make_directories(build);
    if (status == UMI_STATUS_OK)
        status = umi_fs_make_directories(prefix);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(build, "install_manifest.txt", fixture->manifest, sizeof(fixture->manifest));
    if (status == UMI_STATUS_OK)
        status = umi_path_join(prefix, "sample caf\xc3\xa9.exe", fixture->program, sizeof(fixture->program));
    if (status == UMI_STATUS_OK)
        status = umi_path_join(prefix, "help.txt", fixture->data, sizeof(fixture->data));
    /* These are inert bytes. No fixture program is ever executed. */
    if (status == UMI_STATUS_OK)
        status = umi_fs_write_text(fixture->program, "inert executable fixture");
#ifndef _WIN32
    if (status == UMI_STATUS_OK && chmod(fixture->program, 0700) != 0)
        status = UMI_STATUS_IO_ERROR;
#endif
    if (status == UMI_STATUS_OK)
        status = umi_fs_write_text(fixture->data, "Help");
    if (status != UMI_STATUS_OK)
        return status;
    char manifest[UMI_PATH_CAPACITY * 2U + 4U];
    int length = snprintf(manifest, sizeof(manifest), "%s\n%s\n", fixture->program, fixture->data);
    if (length < 0 || (size_t)length >= sizeof(manifest))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        status = umi_fs_write_text(fixture->manifest, manifest);
    return status;
}
#endif
