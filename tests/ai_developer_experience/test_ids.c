/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_ids.c
 *
 * PURPOSE:
 *   Contract coverage for AI Developer Experience ids.
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
#include "umicom/ai_developer_experience/action_ids.h"
#include "umicom/ai_developer_experience/view_ids.h"

int main(void)
{
    assert(strcmp(
        umi_ai_developer_view_id_chat(),
        "umicom.ai-developer.chat") == 0);
    assert(strcmp(
        umi_ai_developer_action_open_review(),
        "ai.developer.open-review") == 0);
    assert(strcmp(
        umi_ai_developer_action_patch_mark_reviewed(),
        "ai.developer.patch-mark-reviewed") == 0);
    return 0;
}

