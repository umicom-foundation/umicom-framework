/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_path.c
 *
 * PURPOSE:
 *   Verify the reusable AI coding runtime path contract.
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
#include "umicom/ai_coding_runtime/path.h"

int main(void)
{

    char path[256];
    assert(umi_ai_coding_runtime_path_normalize_relative(
        "src\\module\\file.c", path, sizeof(path)) == UMI_STATUS_OK);
    assert(strcmp(path, "src/module/file.c") == 0);
    assert(umi_ai_coding_runtime_path_normalize_relative(
        "../secret.txt", path, sizeof(path)) == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_ai_coding_runtime_path_normalize_relative(
        "C:/secret.txt", path, sizeof(path)) == UMI_STATUS_PERMISSION_DENIED);

    return 0;
}
