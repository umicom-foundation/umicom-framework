/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_directory_compare.c
 *
 * PURPOSE:
 *   Verify portable directory comparison aggregation.
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
#include "umicom/developer_productivity/directory_compare.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperDirectoryCompareModel model;
    UmiDeveloperDirectoryCompareItem item;

    umi_developer_directory_compare_init(&model);
    (void)memset(&item, 0, sizeof(item));
    (void)strcpy(item.relative_path, "src/main.c");
    item.state = UMI_DEVELOPER_DIRECTORY_DIFFERENT;

    assert(umi_developer_directory_compare_add(
        &model, &item) == UMI_STATUS_OK);
    assert(model.different_count == 1U);
    return 0;
}
