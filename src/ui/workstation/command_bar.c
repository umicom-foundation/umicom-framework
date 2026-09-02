/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/command_bar.c
 *
 * PURPOSE:
 *   Implement the portable command/search model shared by application shells.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/command_bar.h"

#include <ctype.h>
#include <string.h>

/* Scope values are kept in one range so malformed data is rejected before it
 * can enter a catalogue or be passed to a frontend renderer. */
static bool command_scope_valid(UmiWsCommandScope scope)
{
    return scope >= UMI_WS_COMMAND_SCOPE_ALL &&
           scope <= UMI_WS_COMMAND_SCOPE_AI;
}

/* Compare ASCII command metadata without making search depend on the machine's
 * locale. User-visible Unicode text remains intact in the returned item. */
static bool text_contains_ignore_case(const char *text, const char *query)
{
    size_t text_index;
    size_t query_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || query[0] == '\0') return true;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || text[0] == '\0') return false;

    /* Visit each bounded item once so every record receives the same rule. */
    for (text_index = 0U; text[text_index] != '\0'; ++text_index) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (query_index = 0U;
             query[query_index] != '\0' &&
             text[text_index + query_index] != '\0';
             ++query_index) {
            const unsigned char left =
                (unsigned char)text[text_index + query_index];
            const unsigned char right = (unsigned char)query[query_index];
            /* Apply this branch only when its contract condition is satisfied. */
            if (tolower((int)left) != tolower((int)right)) break;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (query[query_index] == '\0') return true;
    }
    return false;
}

/* An action matches when its scope is allowed and the words appear in any
 * human-readable discovery field. Stable identifiers are included so an
 * experienced user may also search by the exact command name. */
static bool item_matches_query(
    const UmiWsCommandBarItem *item,
    const UmiWsCommandBarQuery *query)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || query == NULL) return false;
    /* Apply this branch only when its contract condition is satisfied. */
    if (query->scope != UMI_WS_COMMAND_SCOPE_ALL &&
        item->scope != query->scope) {
        return false;
    }
    return text_contains_ignore_case(item->title, query->text) ||
           text_contains_ignore_case(item->description, query->text) ||
           text_contains_ignore_case(item->keywords, query->text) ||
           text_contains_ignore_case(item->command_id, query->text) ||
           text_contains_ignore_case(item->item_id, query->text);
}

/* Insert one result before lower-priority entries. Equal priorities keep their
 * registration order, making keyboard navigation deterministic. */
static void insert_result(
    UmiWsCommandBarModel *model,
    size_t item_index)
{
    size_t position = model->result_count;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (position > 0U &&
           model->items[model->result_indices[position - 1U]].priority <
               model->items[item_index].priority) {
        model->result_indices[position] =
            model->result_indices[position - 1U];
        --position;
    }
    model->result_indices[position] = item_index;
    ++model->result_count;
}

/* Rebuild derived search state after a query or enablement change. Disabled
 * actions remain visible so the interface can explain that they are currently
 * unavailable, while activation policy can still prevent execution. */
static void rebuild_results(UmiWsCommandBarModel *model)
{
    size_t index;

    model->result_count = 0U;
    model->selected_result = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (item_matches_query(&model->items[index], &model->query)) {
            insert_result(model, index);
        }
    }
}

/*
 * Initialise ws command bar model from caller-provided values so later operations receive
 * a known state.
 */
void umi_ws_command_bar_model_init(UmiWsCommandBarModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    *model = (UmiWsCommandBarModel){0};
    model->presentation = UMI_WS_COMMAND_BAR_PRESENTATION_EXPANDED;
    model->revision = 1U;
}

/* Add ws command bar model only after its inputs and available capacity have been checked. */
UmiStatus umi_ws_command_bar_model_add(
    UmiWsCommandBarModel *model,
    const char *item_id,
    const char *title,
    const char *description,
    const char *command_id,
    const char *keywords,
    UmiWsCommandScope scope,
    uint32_t priority)
{
    UmiWsCommandBarItem candidate = {0};
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_ws_id_valid(item_id) || title == NULL ||
        title[0] == '\0' || !umi_ws_id_valid(command_id) ||
        !command_scope_valid(scope)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_WS_MAX_PALETTE_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].item_id, item_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    /* Copy into a local value first. This makes the public operation
     * transactional: capacity errors do not change model count or results. */
    status = umi_ws_copy_text(
        candidate.item_id, sizeof(candidate.item_id), item_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ws_copy_text(
            candidate.title, sizeof(candidate.title), title);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ws_copy_text(
            candidate.description,
            sizeof(candidate.description),
            description != NULL ? description : "");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ws_copy_text(
            candidate.command_id, sizeof(candidate.command_id), command_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ws_copy_text(
            candidate.keywords,
            sizeof(candidate.keywords),
            keywords != NULL ? keywords : "");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    candidate.scope = scope;
    candidate.priority = priority;
    candidate.enabled = true;
    model->items[model->count] = candidate;
    ++model->count;
    rebuild_results(model);
    ++model->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws command bar model set query operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_command_bar_model_set_query(
    UmiWsCommandBarModel *model,
    const char *input)
{
    UmiWsCommandBarQuery query;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ws_command_bar_parse(input, &query);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    model->query = query;
    rebuild_results(model);
    ++model->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws command bar model set enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_ws_command_bar_model_set_enabled(
    UmiWsCommandBarModel *model,
    const char *item_id,
    bool enabled)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_ws_id_valid(item_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].item_id, item_id) == 0) {
            model->items[index].enabled = enabled;
            rebuild_results(model);
            ++model->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find ws command bar model result while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiWsCommandBarItem *umi_ws_command_bar_model_result_at(
    const UmiWsCommandBarModel *model,
    size_t result_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || result_index >= model->result_count) return NULL;
    return &model->items[model->result_indices[result_index]];
}

/*
 * Provide the ws command bar model move selection operation used by this module and its
 * client applications.
 */
UmiStatus umi_ws_command_bar_model_move_selection(
    UmiWsCommandBarModel *model,
    int32_t offset)
{
    int64_t next;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (model->result_count == 0U) return UMI_STATUS_NOT_FOUND;
    next = (int64_t)model->selected_result + (int64_t)offset;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next < 0) next = 0;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((uint64_t)next >= (uint64_t)model->result_count) {
        next = (int64_t)(model->result_count - 1U);
    }
    model->selected_result = (size_t)next;
    ++model->revision;
    return UMI_STATUS_OK;
}

/*
 * Find ws command bar model while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWsCommandBarItem *umi_ws_command_bar_model_selected(
    const UmiWsCommandBarModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || model->result_count == 0U) return NULL;
    return umi_ws_command_bar_model_result_at(model, model->selected_result);
}

/*
 * Provide the ws command bar presentation for width operation used by this module and its
 * client applications.
 */
UmiWsCommandBarPresentation umi_ws_command_bar_presentation_for_width(
    int32_t available_width)
{
    /* A compact search field remains usable on ordinary laptop widths. The
     * button form protects the central work area on very small windows. */
    if (available_width < 140) {
        return UMI_WS_COMMAND_BAR_PRESENTATION_BUTTON;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (available_width < 300) {
        return UMI_WS_COMMAND_BAR_PRESENTATION_COMPACT;
    }
    return UMI_WS_COMMAND_BAR_PRESENTATION_EXPANDED;
}

/*
 * Provide the ws command bar model set available width operation used by this module and
 * its client applications.
 */
UmiStatus umi_ws_command_bar_model_set_available_width(
    UmiWsCommandBarModel *model,
    int32_t available_width)
{
    UmiWsCommandBarPresentation presentation;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || available_width < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    presentation = umi_ws_command_bar_presentation_for_width(available_width);
    /* Apply this branch only when its contract condition is satisfied. */
    if (presentation != model->presentation) {
        model->presentation = presentation;
        ++model->revision;
    }
    return UMI_STATUS_OK;
}

/*
 * Read ws command bar into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_ws_command_bar_parse(
    const char *input,
    UmiWsCommandBarQuery *out_query)
{
    const char *text;
    UmiWsCommandScope scope = UMI_WS_COMMAND_SCOPE_ALL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || out_query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    text = input;
    /* Prefixes make a broad command bar predictable without requiring a
     * separate search window for every kind of application object. */
    switch (input[0]) {
        case '>': scope = UMI_WS_COMMAND_SCOPE_COMMAND; ++text; break;
        case '@': scope = UMI_WS_COMMAND_SCOPE_SYMBOL; ++text; break;
        case '#': scope = UMI_WS_COMMAND_SCOPE_TEXT; ++text; break;
        case ':': scope = UMI_WS_COMMAND_SCOPE_LINE; ++text; break;
        case '/': scope = UMI_WS_COMMAND_SCOPE_SETTING; ++text; break;
        case '+': scope = UMI_WS_COMMAND_SCOPE_PANEL; ++text; break;
        case '?': scope = UMI_WS_COMMAND_SCOPE_AI; ++text; break;
        default: break;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*text == ' ') ++text;
    *out_query = (UmiWsCommandBarQuery){0};
    out_query->scope = scope;
    return umi_ws_copy_text(out_query->text, sizeof(out_query->text), text);
}

/*
 * Provide the ws command bar scope prefix operation used by this module and its client
 * applications.
 */
char umi_ws_command_bar_scope_prefix(UmiWsCommandScope scope)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (scope) {
        case UMI_WS_COMMAND_SCOPE_COMMAND: return '>';
        case UMI_WS_COMMAND_SCOPE_SYMBOL: return '@';
        case UMI_WS_COMMAND_SCOPE_TEXT: return '#';
        case UMI_WS_COMMAND_SCOPE_LINE: return ':';
        case UMI_WS_COMMAND_SCOPE_SETTING: return '/';
        case UMI_WS_COMMAND_SCOPE_PANEL: return '+';
        case UMI_WS_COMMAND_SCOPE_AI: return '?';
        default: return '\0';
    }
}
