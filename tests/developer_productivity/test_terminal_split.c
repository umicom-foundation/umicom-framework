/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_terminal_split.c
 *
 * PURPOSE:
 *   Verify portable terminal split placement.
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
#include "umicom/developer_productivity/terminal_split.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperTerminalSplitModel model;
    UmiDeveloperTerminalSplit split;

    umi_developer_terminal_split_model_init(&model);
    (void)memset(&split, 0, sizeof(split));
    (void)strcpy(split.split_id, "split.1");
    (void)strcpy(split.first_session_id, "one");
    (void)strcpy(split.second_session_id, "two");
    split.orientation = UMI_DEVELOPER_TERMINAL_SPLIT_VERTICAL;
    split.ratio = 0.5;

    assert(umi_developer_terminal_split_model_add(
        &model, &split) == UMI_STATUS_OK);
    assert(model.count == 1U);
    return 0;
}
