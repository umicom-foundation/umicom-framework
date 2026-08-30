/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_ignore.c
 *
 * PURPOSE:
 *   Verify the reusable AI coding runtime ignore contract.
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
#include "umicom/ai_coding_runtime/ignore.h"

int main(void)
{

    UmiAiCodingIgnorePolicy policy;
    umi_ai_coding_ignore_policy_init(&policy);
    assert(umi_ai_coding_ignore_path(&policy, ".git/config", 0));
    assert(umi_ai_coding_ignore_path(&policy, "build/app.exe", 0));
    assert(umi_ai_coding_ignore_path(&policy, "node_modules/pkg/index.js", 0));
    assert(!umi_ai_coding_ignore_path(&policy, "src/main.c", 0));

    return 0;
}
