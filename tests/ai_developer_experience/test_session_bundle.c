/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_session_bundle.c
 *
 * PURPOSE:
 *   Contract coverage for AI Developer Experience session bundle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/session_bundle.h"

int main(void)
{
    UmiSessionStore *store = NULL;
    UmiAiCodingToolChatRegistry chats;
    UmiAiCodingToolChatRegistry restored_chats;
    UmiAiDeveloperTaskRegistry *tasks = NULL;
    UmiAiDeveloperTaskRegistry *restored_tasks = NULL;
    UmiAiDeveloperApprovalQueue *approvals = NULL;
    UmiAiDeveloperApprovalQueue *restored_approvals = NULL;
    UmiAiDeveloperPresentationState presentation;
    UmiAiDeveloperPresentationState restored_presentation;
    UmiAiDeveloperPreferences preferences;
    UmiAiDeveloperPreferences restored_preferences;
    UmiAiDeveloperRestoreReport report;
    UmiAiCodingToolChatSession *chat = NULL;
    UmiAiDeveloperTaskEntry task = {0};

    assert(umi_session_store_create(&store) == UMI_STATUS_OK);
    umi_ai_coding_tool_chat_registry_init(&chats);
    assert(umi_ai_coding_tool_chat_registry_open(
        &chats, "chat.1", "provider", "model", &chat) == UMI_STATUS_OK);
    assert(umi_ai_coding_tool_chat_add(
        chat, UMI_AI_ROLE_USER, "user", "hello") == UMI_STATUS_OK);

    assert(umi_ai_developer_task_registry_create(&tasks) == UMI_STATUS_OK);
    assert(umi_ai_developer_task_registry_create(&restored_tasks) ==
           UMI_STATUS_OK);
    (void)strcpy(task.task_id, "task.1");
    (void)strcpy(task.title, "Task");
    task.state = UMI_AI_DEVELOPER_TASK_RUNNING;
    assert(umi_ai_developer_task_registry_upsert(
        tasks, &task) == UMI_STATUS_OK);

    assert(umi_ai_developer_approval_queue_create(&approvals) ==
           UMI_STATUS_OK);
    assert(umi_ai_developer_approval_queue_create(&restored_approvals) ==
           UMI_STATUS_OK);

    umi_ai_developer_presentation_state_init(&presentation);
    assert(umi_ai_developer_presentation_set_chat(
        &presentation, "chat.1") == UMI_STATUS_OK);
    umi_ai_developer_preferences_init(&preferences);

    assert(umi_ai_developer_session_bundle_save(
        store, "ai.dev", &chats, tasks, approvals,
        &presentation, &preferences) == UMI_STATUS_OK);

    assert(umi_ai_developer_session_bundle_restore(
        store, "ai.dev", &restored_chats, restored_tasks,
        restored_approvals, &restored_presentation,
        &restored_preferences, &report) == UMI_STATUS_OK);

    assert(report.schema_version == UMI_AI_DEVELOPER_STATE_SCHEMA_VERSION);
    assert(report.chat_sessions == 1U);
    assert(report.tasks == 1U);
    assert(strcmp(restored_presentation.active_chat_id, "chat.1") == 0);

    umi_ai_developer_approval_queue_destroy(restored_approvals);
    umi_ai_developer_approval_queue_destroy(approvals);
    umi_ai_developer_task_registry_destroy(restored_tasks);
    umi_ai_developer_task_registry_destroy(tasks);
    umi_session_store_destroy(store);
    return 0;
}

