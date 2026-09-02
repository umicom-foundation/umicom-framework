/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_task_executor_contract.c
 *
 * PURPOSE:
 *   Verify semantic Framework build tasks map to native lifecycle operations,
 *   including safe command execution without invoking a shell.
 *
 * REGRESSION COVERAGE:
 *   COMMAND is an implemented lifecycle task and must continue mapping to the
 *   native UMI_BUILD_COMMAND action. DEPLOY and COMPOSITE remain explicit
 *   extension points until their dedicated executors are introduced.
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

#include "umicom/toolchain/task_executor.h"

/*
 * Exercise expect action and return a clear result when the behaviour no longer matches
 * its contract.
 */
static void expect_action(UmiBuildTaskKind kind,
                          UmiBuildAction expected)
{
    UmiBuildTaskSnapshot task;
    UmiBuildAction action = (UmiBuildAction)0;

    (void)memset(&task, 0, sizeof(task));
    task.kind = kind;

    assert(umi_toolchain_task_executor_action(
        &task, &action) == UMI_STATUS_OK);
    assert(action == expected);
}

/*
 * Exercise expect not implemented and return a clear result when the behaviour no longer
 * matches its contract.
 */
static void expect_not_implemented(UmiBuildTaskKind kind)
{
    UmiBuildTaskSnapshot task;
    UmiBuildAction action = (UmiBuildAction)0;

    (void)memset(&task, 0, sizeof(task));
    task.kind = kind;

    assert(umi_toolchain_task_executor_action(
        &task, &action) == UMI_STATUS_NOT_IMPLEMENTED);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    expect_action(UMI_BUILD_TASK_COMMAND, UMI_BUILD_COMMAND);
    expect_action(UMI_BUILD_TASK_CONFIGURE, UMI_BUILD_CONFIGURE);
    expect_action(UMI_BUILD_TASK_BUILD, UMI_BUILD_COMPILE);
    expect_action(UMI_BUILD_TASK_TEST, UMI_BUILD_TEST);
    expect_action(UMI_BUILD_TASK_RUN, UMI_BUILD_RUN);
    expect_action(UMI_BUILD_TASK_CLEAN, UMI_BUILD_CLEAN);
    expect_action(UMI_BUILD_TASK_INSTALL, UMI_BUILD_INSTALL);
    expect_action(UMI_BUILD_TASK_PACKAGE, UMI_BUILD_PACKAGE);

    /*
     * Do not regress COMMAND back to an unsupported placeholder. The executor
     * already implements it through the Framework's argument parser/process
     * path; only genuinely unimplemented task families belong in this block.
     */
    expect_not_implemented(UMI_BUILD_TASK_DEPLOY);
    expect_not_implemented(UMI_BUILD_TASK_COMPOSITE);

    return 0;
}
