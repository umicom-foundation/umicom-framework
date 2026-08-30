/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_persistence_budget.c
 *
 * PURPOSE:
 *   Guard the worst-case durable AI developer state against the authoritative
 *   UMI_SESSION_STORE_MAX entry capacity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/ai_developer_experience/approval_persistence.h"
#include "umicom/ai_developer_experience/chat_persistence.h"
#include "umicom/ai_developer_experience/task_persistence.h"

int main(void)
{
    /*
     * Worst-case accounting:
     * - chat session: 6 metadata entries + 8 messages * 6 entries
     * - task: 4 chunked fields worst-case 12 entries + 7 numeric entries
     * - approval: worst-case 29 entries with 8 argument chunks
     * - presentation/preferences/schema reserve: 32 entries
     */
    const size_t chat_entries =
        1U + UMI_AI_DEVELOPER_PERSISTED_CHAT_SESSIONS * 54U;
    const size_t task_entries =
        1U + UMI_AI_DEVELOPER_PERSISTED_TASKS * 19U;
    const size_t approval_entries =
        1U + UMI_AI_DEVELOPER_PERSISTED_APPROVALS * 29U;
    const size_t reserved_entries = 32U;
    const size_t total =
        chat_entries + task_entries + approval_entries + reserved_entries;

    assert(total <= UMI_SESSION_STORE_MAX);
    return 0;
}
