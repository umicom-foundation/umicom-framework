/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/command.c
 *
 * PURPOSE:
 *   Publish the reusable AI developer command catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/command.h"

#include <string.h>

#include "umicom/ai_developer_experience/action_ids.h"

static const UmiAiDeveloperCommandDescriptor COMMANDS[] = {
    {"ai.developer.open-overview", "AI Developer Overview",
     "Open the AI Developer overview.", UMI_AI_DEVELOPER_PANE_OVERVIEW, 0, 0, 0},
    {"ai.developer.new-chat", "New AI Coding Chat",
     "Open a new repository-aware coding conversation.", UMI_AI_DEVELOPER_PANE_CHAT, 0, 0, 1},
    {"ai.developer.open-tasks", "AI Tasks",
     "Open AI coding tasks.", UMI_AI_DEVELOPER_PANE_TASKS, 0, 0, 0},
    {"ai.developer.open-approvals", "AI Approvals",
     "Open pending sensitive-operation approvals.", UMI_AI_DEVELOPER_PANE_APPROVALS, 0, 0, 0},
    {"ai.developer.open-review", "AI Patch Review",
     "Open the current governed patch review.", UMI_AI_DEVELOPER_PANE_PATCH_REVIEW, 1, 0, 0},
    {"ai.developer.open-diff", "AI Diff",
     "Open line-level patch diff.", UMI_AI_DEVELOPER_PANE_DIFF, 1, 0, 0},
    {"ai.developer.open-tools", "AI Tool Activity",
     "Open controlled developer tool activity.", UMI_AI_DEVELOPER_PANE_TOOL_ACTIVITY, 0, 0, 0},
    {"ai.developer.open-validation", "AI Validation",
     "Open build/test validation evidence.", UMI_AI_DEVELOPER_PANE_VALIDATION, 0, 0, 0},
    {"ai.developer.open-context", "AI Context",
     "Open repository context.", UMI_AI_DEVELOPER_PANE_CONTEXT, 0, 0, 0},
    {"ai.developer.open-history", "AI History",
     "Open unified AI developer history.", UMI_AI_DEVELOPER_PANE_HISTORY, 0, 0, 0},
    {"ai.developer.open-policy", "AI Tool Policy",
     "Open model tool permissions and risk gates.", UMI_AI_DEVELOPER_PANE_POLICY, 0, 0, 0},
    {"ai.developer.open-checkpoints", "AI Checkpoints",
     "Open workspace rollback checkpoints.", UMI_AI_DEVELOPER_PANE_CHECKPOINTS, 0, 0, 0},
    {"ai.developer.approval-approve", "Approve AI Operation",
     "Approve the selected sensitive operation.", UMI_AI_DEVELOPER_PANE_APPROVALS, 0, 1, 1},
    {"ai.developer.approval-reject", "Reject AI Operation",
     "Reject the selected sensitive operation.", UMI_AI_DEVELOPER_PANE_APPROVALS, 0, 1, 1},
    {"ai.developer.patch-apply", "Apply AI Patch",
     "Apply the approved governed patch.", UMI_AI_DEVELOPER_PANE_PATCH_REVIEW, 1, 0, 1},
    {"ai.developer.patch-reject", "Reject AI Patch",
     "Reject the governed patch.", UMI_AI_DEVELOPER_PANE_PATCH_REVIEW, 1, 0, 1},
    {"ai.developer.patch-select-file", "Select Patch File",
     "Select one changed file for review.", UMI_AI_DEVELOPER_PANE_PATCH_REVIEW, 1, 0, 1},
    {"ai.developer.patch-mark-reviewed", "Mark Patch File Reviewed",
     "Mark the selected patch file reviewed.", UMI_AI_DEVELOPER_PANE_PATCH_REVIEW, 1, 0, 1},
    {"ai.developer.patch-mark-unreviewed", "Mark Patch File Unreviewed",
     "Clear reviewed state for the selected patch file.", UMI_AI_DEVELOPER_PANE_PATCH_REVIEW, 1, 0, 1},
    {"ai.developer.diff-next", "Next AI Diff Change",
     "Jump to the next changed line.", UMI_AI_DEVELOPER_PANE_DIFF, 1, 0, 1},
    {"ai.developer.diff-previous", "Previous AI Diff Change",
     "Jump to the previous changed line.", UMI_AI_DEVELOPER_PANE_DIFF, 1, 0, 1},
    {"ai.developer.session-save", "Save AI Developer Session",
     "Persist AI developer state through Session Store.", UMI_AI_DEVELOPER_PANE_OVERVIEW, 0, 0, 1},
    {"ai.developer.session-restore", "Restore AI Developer Session",
     "Restore AI developer state through Session Store.", UMI_AI_DEVELOPER_PANE_OVERVIEW, 0, 0, 1}
};

size_t umi_ai_developer_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiAiDeveloperCommandDescriptor *
umi_ai_developer_command_at(size_t index)
{
    return index < umi_ai_developer_command_count()
        ? &COMMANDS[index]
        : NULL;
}

const UmiAiDeveloperCommandDescriptor *
umi_ai_developer_command_find(const char *command_id)
{
    size_t index;

    if (command_id == NULL) return NULL;

    for (index = 0U; index < umi_ai_developer_command_count(); ++index) {
        if (strcmp(COMMANDS[index].command_id, command_id) == 0) {
            return &COMMANDS[index];
        }
    }

    return NULL;
}
