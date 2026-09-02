/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_build_validation.c
 *
 * PURPOSE:
 *   Verify lifecycle requests fail early when required paths or executables are
 *   missing, without spawning an external process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/toolchain/build.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiBuildRequest request;
    char message[256];

    umi_build_request_init(&request);
    request.source_root = "C:/work/project";
    request.preset = "windows-ucrt64-debug";

    assert(umi_build_request_validate(
        UMI_BUILD_CONFIGURE,
        &request,
        message,
        sizeof(message)) == UMI_STATUS_OK);

    assert(umi_build_request_validate(
        UMI_BUILD_INSTALL,
        &request,
        message,
        sizeof(message)) == UMI_STATUS_INVALID_ARGUMENT);

    request.build_directory = "build/windows-ucrt64-debug";
    request.install_prefix = "install/windows-ucrt64-debug";

    assert(umi_build_request_validate(
        UMI_BUILD_INSTALL,
        &request,
        message,
        sizeof(message)) == UMI_STATUS_OK);

    assert(umi_build_request_validate(
        UMI_BUILD_PACKAGE,
        &request,
        message,
        sizeof(message)) == UMI_STATUS_OK);

    assert(umi_build_request_validate(
        UMI_BUILD_DELIVER,
        &request,
        message,
        sizeof(message)) == UMI_STATUS_OK);

    assert(umi_build_request_validate(
        UMI_BUILD_RUN,
        &request,
        message,
        sizeof(message)) == UMI_STATUS_INVALID_ARGUMENT);

    request.executable = "bin/application.exe";
    assert(umi_build_request_validate(
        UMI_BUILD_RUN,
        &request,
        message,
        sizeof(message)) == UMI_STATUS_OK);

    assert(strlen(message) > 0U);
    return 0;
}
