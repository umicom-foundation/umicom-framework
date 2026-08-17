/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/completion_command.c
 *
 * PURPOSE:
 *   Implement the canonical command catalogue for completion lists, inline
 *   ghost text, partial acceptance and governed AI suggestion policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/completion_command.h"

#include <string.h>

#define COMMAND(kind_value, command_id, command_label, keybinding,             \
                needs_session, needs_candidates, needs_inline, mutates,        \
                confirm)                                                       \
    {                                                                          \
        (uint32_t)sizeof(UmiEditorCompletionCommandDescriptor),                \
        UMI_EDITOR_COMPLETION_COMMAND_API_VERSION,                             \
        kind_value, command_id, command_label, keybinding,                     \
        needs_session, needs_candidates, needs_inline, mutates, confirm        \
    }

static const UmiEditorCompletionCommandDescriptor COMMANDS[] = {
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_TRIGGER,
            "editor.completion.trigger", "Trigger Completion", "Ctrl+Space",
            0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_TRIGGER_SUGGEST,
            "editor.completion.trigger-suggest", "Trigger Suggestion", "",
            0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_HIDE,
            "editor.completion.hide", "Hide Completion", "Escape",
            1, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_SELECT_NEXT,
            "editor.completion.select-next", "Select Next Completion", "Down",
            1, 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_SELECT_PREVIOUS,
            "editor.completion.select-previous", "Select Previous Completion",
            "Up", 1, 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_SELECT_NEXT_PAGE,
            "editor.completion.select-next-page", "Select Next Completion Page",
            "PageDown", 1, 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_SELECT_PREVIOUS_PAGE,
            "editor.completion.select-previous-page",
            "Select Previous Completion Page", "PageUp", 1, 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_ACCEPT,
            "editor.completion.accept", "Accept Completion", "Tab",
            1, 1, 0, 1, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_ACCEPT_ENTER,
            "editor.completion.accept-enter", "Accept Completion with Enter",
            "Enter", 1, 1, 0, 1, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_RESOLVE_DETAILS,
            "editor.completion.resolve-details", "Resolve Completion Details",
            "", 1, 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_TOGGLE_DETAILS,
            "editor.completion.toggle-details", "Toggle Completion Details",
            "Ctrl+Space", 1, 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_FILTER_KIND,
            "editor.completion.filter-kind", "Filter Completion Kind", "",
            1, 1, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_PROVIDER_DIAGNOSTICS,
            "editor.completion.provider-diagnostics",
            "Open Completion Provider Diagnostics", "", 0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_INLINE_TRIGGER,
            "editor.inline-suggestion.trigger", "Trigger Inline Suggestion",
            "Alt+\\", 0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_INLINE_NEXT,
            "editor.inline-suggestion.next", "Show Next Inline Suggestion",
            "Alt+]", 0, 0, 1, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_INLINE_PREVIOUS,
            "editor.inline-suggestion.previous",
            "Show Previous Inline Suggestion", "Alt+[", 0, 0, 1, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_INLINE_ACCEPT,
            "editor.inline-suggestion.accept", "Accept Inline Suggestion", "Tab",
            0, 0, 1, 1, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_INLINE_ACCEPT_WORD,
            "editor.inline-suggestion.accept-word",
            "Accept Next Inline Suggestion Word", "Ctrl+Right",
            0, 0, 1, 1, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_INLINE_ACCEPT_LINE,
            "editor.inline-suggestion.accept-line",
            "Accept Next Inline Suggestion Line", "Ctrl+End",
            0, 0, 1, 1, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_INLINE_REJECT,
            "editor.inline-suggestion.reject", "Reject Inline Suggestion",
            "Escape", 0, 0, 1, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_INLINE_TOGGLE,
            "editor.inline-suggestion.toggle", "Toggle Inline Suggestions", "",
            0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_INLINE_PAUSE,
            "editor.inline-suggestion.pause", "Pause Inline Suggestions", "",
            0, 0, 0, 0, 0),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_AI_ENABLE,
            "editor.inline-suggestion.ai-enable",
            "Enable AI Inline Suggestions", "", 0, 0, 0, 0, 1),
    COMMAND(UMI_EDITOR_COMPLETION_COMMAND_AI_DISABLE,
            "editor.inline-suggestion.ai-disable",
            "Disable AI Inline Suggestions", "", 0, 0, 0, 0, 0)
};

#undef COMMAND

size_t umi_editor_completion_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiEditorCompletionCommandDescriptor *
umi_editor_completion_command_at(size_t position)
{
    return position < umi_editor_completion_command_count()
        ? &COMMANDS[position]
        : NULL;
}

const UmiEditorCompletionCommandDescriptor *
umi_editor_completion_command_find(const char *command_id)
{
    size_t position;

    if (command_id == NULL || command_id[0] == '\0') return NULL;
    for (position = 0U; position < umi_editor_completion_command_count();
         ++position) {
        if (strcmp(COMMANDS[position].id, command_id) == 0) {
            return &COMMANDS[position];
        }
    }
    return NULL;
}

const UmiEditorCompletionCommandDescriptor *
umi_editor_completion_command_for_kind(UmiEditorCompletionCommandKind kind)
{
    size_t position;

    for (position = 0U; position < umi_editor_completion_command_count();
         ++position) {
        if (COMMANDS[position].kind == kind) return &COMMANDS[position];
    }
    return NULL;
}
