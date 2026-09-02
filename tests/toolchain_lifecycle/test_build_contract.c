/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_build_contract.c
 *
 * PURPOSE:
 *   Protect stable build-action values and default lifecycle initialisation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/toolchain/build.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiBuildRequest request;
    UmiBuildReport report;

    assert(UMI_BUILD_CONFIGURE == 1);
    assert(UMI_BUILD_COMPILE == 2);
    assert(UMI_BUILD_TEST == 3);
    assert(UMI_BUILD_RUN == 4);
    assert(UMI_BUILD_MAKE == 5);
    assert(UMI_BUILD_CLEAN == 6);
    assert(UMI_BUILD_INSTALL == 7);
    assert(UMI_BUILD_PACKAGE == 8);
    assert(UMI_BUILD_DELIVER == 9);
    assert(UMI_BUILD_COMMAND == 10);

    umi_build_request_init(&request);
    assert(request.source_root == NULL);
    assert(request.build_directory == NULL);
    assert(request.timeout_ms == 0U);
    assert(request.cancellation == NULL);
    assert(request.window_mode == UMI_PROCESS_WINDOW_HIDDEN);

    umi_build_report_init(&report);
    assert(report.configure_exit_code == -1);
    assert(report.build_exit_code == -1);
    assert(report.test_exit_code == -1);
    assert(report.run_exit_code == -1);
    assert(report.install_exit_code == -1);
    assert(report.package_exit_code == -1);
    assert(report.last_status == UMI_STATUS_OK);

    assert(umi_build_action_text(UMI_BUILD_INSTALL) != NULL);
    assert(umi_build_action_text(UMI_BUILD_PACKAGE) != NULL);
    assert(umi_build_action_text(UMI_BUILD_DELIVER) != NULL);
    return 0;
}
