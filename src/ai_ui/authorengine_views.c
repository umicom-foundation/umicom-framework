/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_ui/authorengine_views.c
 *
 * PURPOSE:
 *   Project AuthorEngine Integration v2 state into toolkit-neutral view models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_ui/authorengine_views.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "umicom/ui/command_view.h"

/*
 * Each property below is plain data.  A GTK, Qt, Wt or web adapter may render
 * the same result without the reusable AI layer knowing which toolkit won.
 */
static UmiStatus set_string(UmiUiViewModel *view,
                            const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(
        &value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_integer(UmiUiViewModel *view,
                             const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_boolean(UmiUiViewModel *view,
                             const char *key,
                             int enabled)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_boolean(&value, enabled);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_action(UmiUiViewModel *view,
                            size_t index,
                            const char *action_id,
                            const char *label,
                            const char *tooltip)
{
    UmiUiCommandViewAction action;
    (void)memset(&action, 0, sizeof(action));
    (void)snprintf(action.action_id, sizeof(action.action_id), "%s", action_id);
    (void)snprintf(action.label, sizeof(action.label), "%s", label);
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s", tooltip);
    action.enabled = 1;
    return umi_ui_command_view_set_action(view, index, &action);
}

static UmiStatus base_view(const char *view_id,
                           const char *kind,
                           const char *title,
                           const char *summary,
                           UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    status = umi_ui_view_model_create(
        view_id, "umicom.authorengine-ui", UMI_UI_ROLE_PANE, out_view);
    if (status == UMI_STATUS_OK) {
        status = set_string(*out_view, "umicom.view-kind", kind);
    }
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    if (status == UMI_STATUS_OK) {
        status = set_string(*out_view, "summary", summary);
    }
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

UmiStatus umi_ai_ui_authorengine_overview_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view)
{
    UmiAiAuthorEngineServiceSnapshot snapshot;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "authorengine-overview", "AI Workspace",
        "AuthorEngine orchestration, governed context and conversation status.",
        out_view);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_authorengine_service_snapshot(service, &snapshot);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "authorengine.executable", snapshot.executable);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "authorengine.workspace", snapshot.workspace);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "authorengine.active-session", snapshot.active_session_id);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "authorengine.providers", (int64_t)snapshot.providers);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "authorengine.runtimes", (int64_t)snapshot.runtimes);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "authorengine.healthy-runtimes",
        (int64_t)snapshot.healthy_runtimes);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "authorengine.context-sources",
        (int64_t)snapshot.context_sources);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "authorengine.sessions", (int64_t)snapshot.sessions);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "authorengine.context-limit",
        (int64_t)snapshot.context_limit);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "authorengine.reserved-output-tokens",
        (int64_t)snapshot.reserved_output_tokens);
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "authorengine.remote-allowed", snapshot.remote_allowed);
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "authorengine.persistence-allowed",
        snapshot.persistence_allowed);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.ai.refresh-health", "Refresh Health",
        "Probe configured AI provider and AuthorEngine runtime boundaries");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.ai.new-session", "New Session",
        "Create a governed AuthorEngine conversation session");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.ai.save-session", "Save Session",
        "Persist the active session when privacy policy permits");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.pane.ai-runtimes", "Runtimes",
        "Open the provider and model runtime catalogue");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 4U, "studio.action.pane.ai-context", "Context",
        "Inspect sources, token budgets and sharing decisions");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 5U, "studio.action.pane.ai-sessions", "Sessions",
        "Inspect conversation session state");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 6U, "studio.action.pane.ai-privacy", "Privacy",
        "Inspect local, remote and persistence controls");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 7U, "studio.action.pane.ai-coding", "Coding Assistant",
        "Open repository-aware coding tasks and reviewed patch workflows");
    return status;
}

UmiStatus umi_ai_ui_chat_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view)
{
    UmiAiAuthorEngineServiceSnapshot snapshot;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "ai-chat", "Assistant Chat",
        "Ask questions, continue conversations and delegate approved tasks.",
        out_view);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_authorengine_service_snapshot(service, &snapshot);
    }
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-chat.active-session", snapshot.active_session_id);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-chat.session-count", (int64_t)snapshot.sessions);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-chat.provider-count", (int64_t)snapshot.providers);
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "ai-chat.remote-allowed", snapshot.remote_allowed);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.ai.send-message", "Send",
        "Send the composed message through the selected governed runtime");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.ai.new-session", "New Chat",
        "Start a separate conversation session");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.pane.ai-model-comparison",
        "Compare Models",
        "Ask several approved models and inspect their answers side by side");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.pane.ai-coding", "Agent Task",
        "Open repository-aware tasks and reviewed patch workflows");
    return status;
}

UmiStatus umi_ai_ui_runtime_catalogue_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view)
{
    UmiAiRuntimeCatalogue *catalogue;
    size_t count;
    size_t index;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "ai-runtimes", "AI Runtime Catalogue",
        "Local and remote models available through governed provider boundaries.",
        out_view);
    if (status != UMI_STATUS_OK) return status;
    catalogue = umi_ai_authorengine_service_catalogue(service);
    count = umi_ai_runtime_catalogue_count(catalogue);
    if (count > UMI_AI_UI_VISIBLE_ROWS) count = UMI_AI_UI_VISIBLE_ROWS;
    status = set_integer(*out_view, "ai-runtimes.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiAiRuntimeDescriptor descriptor;
        char key[96];
        char text[512];
        status = umi_ai_runtime_catalogue_at(catalogue, index, &descriptor);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "ai-runtimes.row.%zu", index);
        (void)snprintf(
            text, sizeof(text), "%.128s | %.80s / %.80s | %.24s | %" PRIu32
            " context | %.96s",
            descriptor.display_name, descriptor.provider_id,
            descriptor.model_id,
            umi_ai_runtime_transport_text(descriptor.transport),
            descriptor.context_tokens,
            descriptor.health.available ? "healthy" : descriptor.health.message);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.ai.refresh-health", "Refresh Health",
        "Probe provider and AuthorEngine runtime health");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.pane.ai-workspace", "AI Workspace",
        "Return to the AuthorEngine overview");
    return status;
}

UmiStatus umi_ai_ui_context_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view)
{
    UmiAiAuthorEngineServiceSnapshot snapshot;
    UmiAiContextBroker *broker;
    size_t count;
    size_t index;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "ai-context", "AI Context Broker",
        "Project and workspace sources governed by explicit token and privacy budgets.",
        out_view);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ai_authorengine_service_snapshot(service, &snapshot);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-context.limit", (int64_t)snapshot.context_limit);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-context.reserved-output",
        (int64_t)snapshot.reserved_output_tokens);
    broker = umi_ai_authorengine_service_context(service);
    count = umi_ai_context_broker_count(broker);
    if (count > UMI_AI_UI_VISIBLE_ROWS) count = UMI_AI_UI_VISIBLE_ROWS;
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-context.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiAiContextSource source;
        char key[96];
        char text[512];
        status = umi_ai_context_broker_at(broker, index, &source);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "ai-context.row.%zu", index);
        (void)snprintf(
            text, sizeof(text), "%.300s | %.32s | %" PRIu32
            " tokens | %.24s | %.16s",
            source.label, umi_ai_context_source_kind_text(source.kind),
            source.estimated_tokens,
            umi_ai_data_classification_text(source.classification),
            source.enabled ? (source.mandatory ? "mandatory" : "enabled")
                           : "disabled");
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.pane.ai-workspace", "AI Workspace",
        "Return to the AuthorEngine overview");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.pane.ai-privacy", "Privacy",
        "Inspect rules applied to these context sources");
    return status;
}

UmiStatus umi_ai_ui_sessions_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view)
{
    UmiAiConversationArchive *archive;
    size_t count;
    size_t index;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "ai-sessions", "AI Conversation Sessions",
        "Bounded conversation state with explicit persistence and approval controls.",
        out_view);
    if (status != UMI_STATUS_OK) return status;
    archive = umi_ai_authorengine_service_conversations(service);
    count = umi_ai_conversation_archive_count(archive);
    if (count > UMI_AI_UI_VISIBLE_ROWS) count = UMI_AI_UI_VISIBLE_ROWS;
    status = set_integer(*out_view, "ai-sessions.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiAiConversationRecord record;
        char key[96];
        char text[512];
        status = umi_ai_conversation_archive_at(archive, index, &record);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "ai-sessions.row.%zu", index);
        (void)snprintf(
            text, sizeof(text), "%.128s | %.80s | %.80s / %.80s | %zu messages | %.24s",
            record.title, record.session.session_id, record.session.provider_id,
            record.session.model_id, record.conversation.message_count,
            umi_ai_data_classification_text(record.classification));
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.ai.new-session", "New Session",
        "Create a governed AuthorEngine conversation session");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.ai.save-session", "Save Session",
        "Persist the active session when policy permits");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.pane.ai-workspace", "AI Workspace",
        "Return to the AuthorEngine overview");
    return status;
}

UmiStatus umi_ai_ui_privacy_view_create(
    const char *view_id,
    UmiAiAuthorEngineService *service,
    UmiUiViewModel **out_view)
{
    const UmiAiPrivacyPolicy *privacy;
    UmiAiAuthorEngineServiceSnapshot snapshot;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "ai-privacy", "AI Privacy and Approval Policy",
        "Local, remote, sensitive-data and persistence decisions remain application controlled.",
        out_view);
    if (status != UMI_STATUS_OK) return status;
    privacy = umi_ai_authorengine_service_privacy_policy(service);
    status = umi_ai_authorengine_service_snapshot(service, &snapshot);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-privacy.local-maximum",
        umi_ai_data_classification_text(privacy->maximum_local_classification));
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-privacy.remote-maximum",
        umi_ai_data_classification_text(privacy->maximum_remote_classification));
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "ai-privacy.remote-allowed", snapshot.remote_allowed);
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "ai-privacy.persist-sessions", privacy->persist_sessions);
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "ai-privacy.persist-prompt-text", privacy->persist_prompt_text);
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "ai-privacy.require-sensitive-approval",
        privacy->require_sensitive_approval);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.pane.ai-context", "Context",
        "Inspect sources governed by this privacy policy");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.pane.ai-workspace", "AI Workspace",
        "Return to the AuthorEngine overview");
    return status;
}
