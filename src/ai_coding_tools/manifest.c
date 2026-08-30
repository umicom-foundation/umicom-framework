/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/manifest.c
 *
 * PURPOSE:
 *   Define concise built-in tool argument/result contracts for provider prompts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/manifest.h"

#include <string.h>

static const UmiAiCodingToolManifestEntry ENTRIES[] = {
    { .tool_id = "agent.apply", .arguments_schema = "{}", .result_summary = "Applies the approved governed patch." },
    { .tool_id = "agent.approve", .arguments_schema = "{\"approvedBy\":\"identity\"}", .result_summary = "Approves pending governed patch." },
    { .tool_id = "agent.cancel", .arguments_schema = "{}", .result_summary = "Requests coding-task cancellation." },
    { .tool_id = "agent.continue-repair", .arguments_schema = "{}", .result_summary = "Continues bounded repair iteration." },
    { .tool_id = "agent.reject", .arguments_schema = "{}", .result_summary = "Rejects pending patch." },
    { .tool_id = "agent.snapshot", .arguments_schema = "{}", .result_summary = "Returns coding-agent state." },
    { .tool_id = "debug.continue", .arguments_schema = "{\"threadId\":0,\"timeoutMs\":1000}", .result_summary = "Continues the debuggee." },
    { .tool_id = "debug.pause", .arguments_schema = "{\"threadId\":0,\"timeoutMs\":1000}", .result_summary = "Pauses the debuggee." },
    { .tool_id = "debug.snapshot", .arguments_schema = "{}", .result_summary = "Returns DAP runtime/session/inspection state." },
    { .tool_id = "debug.stack", .arguments_schema = "{\"threadId\":0,\"timeoutMs\":1000}", .result_summary = "Refreshes stack for active or specified thread." },
    { .tool_id = "debug.step-into", .arguments_schema = "{\"threadId\":0,\"timeoutMs\":1000}", .result_summary = "Steps into." },
    { .tool_id = "debug.step-out", .arguments_schema = "{\"threadId\":0,\"timeoutMs\":1000}", .result_summary = "Steps out." },
    { .tool_id = "debug.step-over", .arguments_schema = "{\"threadId\":0,\"timeoutMs\":1000}", .result_summary = "Steps over." },
    { .tool_id = "debug.threads", .arguments_schema = "{\"timeoutMs\":1000}", .result_summary = "Refreshes and reports thread state." },
    { .tool_id = "developer.build", .arguments_schema = "{\"program\":\"tool\",\"arguments\":\"args\",\"workingDirectory\":\"optional\",\"timeoutMs\":180000}", .result_summary = "Executes a build operation." },
    { .tool_id = "developer.configure", .arguments_schema = "{\"program\":\"tool\",\"arguments\":\"args\",\"workingDirectory\":\"optional\",\"timeoutMs\":120000}", .result_summary = "Executes a configure operation." },
    { .tool_id = "developer.package", .arguments_schema = "{\"program\":\"tool\",\"arguments\":\"args\",\"workingDirectory\":\"optional\",\"timeoutMs\":300000}", .result_summary = "Executes a package operation." },
    { .tool_id = "developer.run", .arguments_schema = "{\"program\":\"tool\",\"arguments\":\"quoted args\",\"workingDirectory\":\"optional\",\"timeoutMs\":120000}", .result_summary = "Executes through DeveloperExecutor." },
    { .tool_id = "developer.test", .arguments_schema = "{\"program\":\"tool\",\"arguments\":\"args\",\"workingDirectory\":\"optional\",\"timeoutMs\":300000}", .result_summary = "Executes a test operation." },
    { .tool_id = "language.diagnostics", .arguments_schema = "{\"documentId\":\"optional\",\"limit\":100}", .result_summary = "Returns provider-neutral diagnostics." },
    { .tool_id = "language.symbol-find", .arguments_schema = "{\"id\":\"symbol-id\"}", .result_summary = "Returns one symbol by stable identifier." },
    { .tool_id = "language.symbols", .arguments_schema = "{\"query\":\"optional name substring\",\"documentId\":\"optional\",\"limit\":100}", .result_summary = "Returns provider-neutral language symbols." },
    { .tool_id = "source-control.commit", .arguments_schema = "{\"message\":\"commit message\"}", .result_summary = "Creates a commit and returns its ID." },
    { .tool_id = "source-control.diff", .arguments_schema = "{\"path\":\"relative/path\",\"staged\":false}", .result_summary = "Returns working-tree or staged diff for one path." },
    { .tool_id = "source-control.fetch", .arguments_schema = "{}", .result_summary = "Fetches from configured remote." },
    { .tool_id = "source-control.pull", .arguments_schema = "{}", .result_summary = "Pulls from configured remote." },
    { .tool_id = "source-control.push", .arguments_schema = "{}", .result_summary = "Pushes to configured remote." },
    { .tool_id = "source-control.stage", .arguments_schema = "{\"path\":\"relative/path\"}", .result_summary = "Stages one path." },
    { .tool_id = "source-control.stage-all", .arguments_schema = "{}", .result_summary = "Stages all current changes." },
    { .tool_id = "source-control.status", .arguments_schema = "{}", .result_summary = "Returns branch, change and staged counts." },
    { .tool_id = "source-control.unstage", .arguments_schema = "{\"path\":\"relative/path\"}", .result_summary = "Unstages one path." },
    { .tool_id = "tools.catalogue", .arguments_schema = "{}", .result_summary = "Lists tool risk and approval metadata." },
    { .tool_id = "tools.policy", .arguments_schema = "{}", .result_summary = "Returns active tool policy." },
    { .tool_id = "workspace.checkpoint-create", .arguments_schema = "{\"id\":\"checkpoint-id\",\"label\":\"label\",\"paths\":[\"a.c\",\"b.h\"]}", .result_summary = "Captures complete-file rollback state." },
    { .tool_id = "workspace.checkpoint-list", .arguments_schema = "{}", .result_summary = "Lists checkpoint metadata without source content." },
    { .tool_id = "workspace.checkpoint-restore", .arguments_schema = "{\"id\":\"checkpoint-id\"}", .result_summary = "Restores the named checkpoint." },
    { .tool_id = "workspace.exists", .arguments_schema = "{\"path\":\"relative/path\"}", .result_summary = "Returns whether the path exists." },
    { .tool_id = "workspace.list", .arguments_schema = "{\"contains\":\"optional substring\",\"limit\":100}", .result_summary = "Returns bounded repository file metadata." },
    { .tool_id = "workspace.read", .arguments_schema = "{\"path\":\"relative/path\"}", .result_summary = "Returns path, byte count and full bounded text content." },
    { .tool_id = "workspace.search", .arguments_schema = "{\"query\":\"literal text\",\"limit\":50}", .result_summary = "Returns bounded path/line/text matches." }
};

size_t umi_ai_coding_tool_manifest_count(void)
{
    return sizeof(ENTRIES) / sizeof(ENTRIES[0]);
}

UmiStatus umi_ai_coding_tool_manifest_at(
    size_t index,
    UmiAiCodingToolManifestEntry *out_entry)
{
    if (out_entry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= umi_ai_coding_tool_manifest_count()) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_entry = ENTRIES[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_tool_manifest_find(
    const char *tool_id,
    UmiAiCodingToolManifestEntry *out_entry)
{
    size_t index;

    if (tool_id == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < umi_ai_coding_tool_manifest_count(); ++index) {
        if (strcmp(ENTRIES[index].tool_id, tool_id) == 0) {
            *out_entry = ENTRIES[index];
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}
