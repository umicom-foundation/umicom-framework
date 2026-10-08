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

/* The original single-phrase search and priority-only ordering are retained
 * for review. Word matching and relevance ordering below replace them so users
 * can find tools without remembering the exact label or registration order. */
#if 0
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

#endif

/* Search belongs in Framework so every frontend recognises the same words.
 * ASCII folding is explicit: locale settings must not change command ordering.
 * Non-ASCII bytes are compared exactly; this is not Unicode case folding. */
static unsigned char search_fold(unsigned char value)
{
    return value >= (unsigned char)'A' && value <= (unsigned char)'Z'
        ? (unsigned char)(value + (unsigned char)('a' - 'A')) : value;
}

/* Space-separated words may match different discovery fields. The caller
 * supplies a bounded span so matching never needs a temporary word buffer. */
static bool search_space(char value)
{
    return value == ' ' || value == '\t' || value == '\r' || value == '\n';
}

static bool search_prefix(const char *text, const char *word, size_t length)
{
    for (size_t index = 0U; index < length; ++index) {
        if (text[index] == '\0' ||
            search_fold((unsigned char)text[index]) !=
            search_fold((unsigned char)word[index])) return false;
    }
    return true;
}

static bool search_contains(const char *text, const char *word, size_t length)
{
    for (; *text != '\0'; ++text)
        if (search_prefix(text, word, length)) return true;
    return length == 0U;
}

/* Relevance precedes the application's priority for a non-empty query. Exact
 * names win over prefix matches; otherwise all words must be discoverable.
 * Registration order remains the tie-breaker for equal relevance and priority. */
static unsigned int command_match_rank(
    const UmiWsCommandBarItem *item, const UmiWsCommandBarQuery *query)
{
    const char *first = query->text;
    const char *end = first + strlen(first);
    const char *cursor;
    size_t length;
    if (query->scope != UMI_WS_COMMAND_SCOPE_ALL &&
        query->scope != item->scope) return 0U;
    while (search_space(*first)) ++first;
    while (end > first && search_space(end[-1])) --end;
    length = (size_t)(end - first);
    if (length == 0U) return 1U;
    cursor = first;
    while (cursor < end) {
        const char *word = cursor;
        while (cursor < end && !search_space(*cursor)) ++cursor;
        size_t word_length = (size_t)(cursor - word);
        if (!search_contains(item->title, word, word_length) &&
            !search_contains(item->description, word, word_length) &&
            !search_contains(item->keywords, word, word_length) &&
            !search_contains(item->command_id, word, word_length) &&
            !search_contains(item->item_id, word, word_length)) return 0U;
        while (cursor < end && search_space(*cursor)) ++cursor;
    }
    if (strlen(item->title) == length &&
        search_prefix(item->title, first, length)) return 4U;
    if (strlen(item->command_id) == length &&
        search_prefix(item->command_id, first, length)) return 3U;
    if (search_prefix(item->title, first, length)) return 2U;
    return 1U;
}

static bool item_matches_query(
    const UmiWsCommandBarItem *item, const UmiWsCommandBarQuery *query)
{
    return item != NULL && query != NULL && command_match_rank(item, query) != 0U;
}

/* Insert indices rather than copying items. A filtered view retains the
 * catalogue's ownership and does not expose borrowed pointers across refresh. */
static void insert_result(UmiWsCommandBarModel *model, size_t item_index)
{
    size_t position = model->result_count;
    unsigned int rank = command_match_rank(&model->items[item_index], &model->query);
    while (position > 0U) {
        const UmiWsCommandBarItem *previous =
            &model->items[model->result_indices[position - 1U]];
        unsigned int previous_rank = command_match_rank(previous, &model->query);
        if (previous_rank > rank ||
            (previous_rank == rank && previous->priority >= model->items[item_index].priority))
            break;
        model->result_indices[position] = model->result_indices[position - 1U];
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
    /* The direct index lookup is kept for review; the checked lookup below
     * adds protection for malformed public snapshots without changing valid results. */
#if 0
    if (model == NULL || result_index >= model->result_count) return NULL;
    return &model->items[model->result_indices[result_index]];
#endif
    /* Public snapshots may cross an extension boundary. Reject invalid counts
     * before following an index into their fixed-capacity arrays. */
    if (model == NULL || model->count > UMI_WS_MAX_PALETTE_ITEMS ||
        model->result_count > model->count || result_index >= model->result_count ||
        model->result_indices[result_index] >= model->count) return NULL;
    return &model->items[model->result_indices[result_index]];
}

static bool command_results_valid(const UmiWsCommandBarModel *model);

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
    /* Validate before converting a public size_t index to a signed offset. */
    if (!command_results_valid(model)) return UMI_STATUS_INVALID_ARGUMENT;
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

/* Validate the published result map before sharing page indices with a
 * frontend. Checking the entire map prevents a later page from hiding damage. */
static bool command_results_valid(const UmiWsCommandBarModel *model)
{
    if (model == NULL || model->count > UMI_WS_MAX_PALETTE_ITEMS ||
        model->result_count > model->count ||
        (model->result_count > 0U && model->selected_result >= model->result_count))
        return false;
    for (size_t index = 0U; index < model->result_count; ++index)
        if (model->result_indices[index] >= model->count) return false;
    return true;
}

/* Shared selection keeps keyboard, touch paging and automation on the same
 * result. It changes presentation state only and never invokes a command. */
UmiStatus umi_ws_command_bar_model_select_result(
    UmiWsCommandBarModel *model, size_t result_index)
{
    if (!command_results_valid(model)) return UMI_STATUS_INVALID_ARGUMENT;
    if (result_index >= model->result_count) return UMI_STATUS_NOT_FOUND;
    if (result_index != model->selected_result) {
        model->selected_result = result_index;
        ++model->revision;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_command_bar_model_page(
    const UmiWsCommandBarModel *model, size_t page_size, UmiWsCommandBarPage *out_page)
{
    UmiWsCommandBarPage page = {0};
    if (out_page == NULL || page_size == 0U || !command_results_valid(model))
        return UMI_STATUS_INVALID_ARGUMENT;
    page.total_results = model->result_count;
    if (model->result_count > 0U) {
        page.selected_result = model->selected_result;
        page.first_result = (model->selected_result / page_size) * page_size;
        page.result_count = model->result_count - page.first_result;
        if (page.result_count > page_size) page.result_count = page_size;
        page.has_previous = page.first_result != 0U;
        page.has_next = page.result_count < model->result_count - page.first_result;
    }
    *out_page = page;
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
    /* The original in-place parser is preserved for comparison. A local
     * candidate now prevents rejected input from erasing the previous query. */
#if 0
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
#endif
    /* Parse into local storage. Failure must leave the caller's previous
     * query intact, including when input aliases that query's text buffer. */
    UmiWsCommandBarQuery candidate = {0};
    UmiStatus status;
    text = input;
    while (search_space(*text)) ++text;
    switch (*text) {
        case '>': scope = UMI_WS_COMMAND_SCOPE_COMMAND; ++text; break;
        case '@': scope = UMI_WS_COMMAND_SCOPE_SYMBOL; ++text; break;
        case '#': scope = UMI_WS_COMMAND_SCOPE_TEXT; ++text; break;
        case ':': scope = UMI_WS_COMMAND_SCOPE_LINE; ++text; break;
        case '/': scope = UMI_WS_COMMAND_SCOPE_SETTING; ++text; break;
        case '+': scope = UMI_WS_COMMAND_SCOPE_PANEL; ++text; break;
        case '?': scope = UMI_WS_COMMAND_SCOPE_AI; ++text; break;
        default: break;
    }
    while (search_space(*text)) ++text;
    status = umi_ws_copy_text(candidate.text, sizeof(candidate.text), text);
    if (status != UMI_STATUS_OK) return status;
    size_t length = strlen(candidate.text);
    while (length > 0U && search_space(candidate.text[length - 1U]))
        candidate.text[--length] = '\0';
    candidate.scope = scope;
    *out_query = candidate;
    return UMI_STATUS_OK;
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
