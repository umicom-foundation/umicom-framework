/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_task_executor_contract.c
 *
 * PURPOSE:
 *   Verify semantic Framework build tasks map to native lifecycle operations
 *   without invoking a shell or external tool.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/toolchain/task_executor.h"

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

int main(void)
{
    UmiBuildTaskSnapshot task;
    UmiBuildAction action = (UmiBuildAction)0;

    expect_action(UMI_BUILD_TASK_CONFIGURE, UMI_BUILD_CONFIGURE);
    expect_action(UMI_BUILD_TASK_BUILD, UMI_BUILD_COMPILE);
    expect_action(UMI_BUILD_TASK_TEST, UMI_BUILD_TEST);
    expect_action(UMI_BUILD_TASK_RUN, UMI_BUILD_RUN);
    expect_action(UMI_BUILD_TASK_CLEAN, UMI_BUILD_CLEAN);
    expect_action(UMI_BUILD_TASK_INSTALL, UMI_BUILD_INSTALL);
    expect_action(UMI_BUILD_TASK_PACKAGE, UMI_BUILD_PACKAGE);

    (void)memset(&task, 0, sizeof(task));

    task.kind = UMI_BUILD_TASK_COMMAND;
    assert(umi_toolchain_task_executor_action(
        &task, &action) == UMI_STATUS_NOT_IMPLEMENTED);

    task.kind = UMI_BUILD_TASK_DEPLOY;
    assert(umi_toolchain_task_executor_action(
        &task, &action) == UMI_STATUS_NOT_IMPLEMENTED);

    task.kind = UMI_BUILD_TASK_COMPOSITE;
    assert(umi_toolchain_task_executor_action(
        &task, &action) == UMI_STATUS_NOT_IMPLEMENTED);

    return 0;
}
