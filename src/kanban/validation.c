/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/validation.c
 * PURPOSE: Validate complete planning state before publishing reviewed changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "board_internal.h"
#include <string.h>

bool UmiKanbanText(const char *text, size_t capacity, bool optional)
{
    if (text == NULL || capacity == 0U || (!optional && text[0] == '\0')) return false;
    for (size_t i = 0U; i < capacity; ++i) {
        if (text[i] == '\0') return true;
    }
    return false;
}
bool UmiKanbanId(const char *text, bool optional)
{
    if (!UmiKanbanText(text, UMI_KANBAN_ID_SIZE, optional)) return false;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        unsigned char ch = (unsigned char)text[i];
        if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
              (ch >= '0' && ch <= '9') || ch == '.' || ch == '_' || ch == '-' || ch == ':'))
            return false;
    }
    return true;
}
size_t UmiKanbanFind(const UmiKanbanBoard *board, const char *id)
{
    for (size_t i = 0U; i < board->count; ++i) {
        if (strcmp(board->rows[i].card.id, id) == 0) return i;
    }
    return UMI_KANBAN_MISSING;
}
size_t UmiKanbanColumnFind(const UmiKanbanConfig *config, const char *id)
{
    for (size_t i = 0U; i < config->column_count; ++i) {
        if (strcmp(config->columns[i].id, id) == 0) return i;
    }
    return UMI_KANBAN_MISSING;
}
UmiStatus UmiKanbanProblemSet(UmiKanbanProblem *problem, UmiKanbanProblemKind kind,
    UmiStatus status, const char *card, const char *column)
{
    if (problem != NULL) {
        problem->kind = kind;
        if (card != NULL) memcpy(problem->card_id, card, strlen(card) + 1U);
        if (column != NULL) memcpy(problem->column_id, column, strlen(column) + 1U);
    }
    return status;
}
/* The extended validator below accepts the appended Blocked stage while retaining every existing configuration check. Retained for engineering review. */
#if 0
UmiStatus UmiKanbanConfigCheck(const UmiKanbanConfig *config)
{
    if (config == NULL || !UmiKanbanId(config->id, false) ||
        !UmiKanbanText(config->title, sizeof(config->title), false) ||
        config->card_capacity == 0U || config->card_capacity > UMI_KANBAN_MAX_CARDS ||
        config->column_count == 0U || config->column_count > UMI_KANBAN_MAX_COLUMNS)
        return UMI_STATUS_INVALID_ARGUMENT;
    uint32_t mask = (UINT32_C(1) << config->column_count) - UINT32_C(1);
    for (size_t i = 0U; i < config->column_count; ++i) {
        const UmiKanbanColumn *column = &config->columns[i];
        if (!UmiKanbanId(column->id, false) ||
            !UmiKanbanText(column->title, sizeof(column->title), false) ||
            column->stage < UMI_KANBAN_TODO || column->stage > UMI_KANBAN_DONE ||
            column->wip_limit > config->card_capacity || (config->transitions[i] & ~mask) != 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j) {
            if (strcmp(column->id, config->columns[j].id) == 0) return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    return UMI_STATUS_OK;
}
#endif

UmiStatus UmiKanbanConfigCheck(const UmiKanbanConfig *config)
{
    if (config == NULL || !UmiKanbanId(config->id, false) ||
        !UmiKanbanText(config->title, sizeof(config->title), false) ||
        config->card_capacity == 0U || config->card_capacity > UMI_KANBAN_MAX_CARDS ||
        config->column_count == 0U || config->column_count > UMI_KANBAN_MAX_COLUMNS)
        return UMI_STATUS_INVALID_ARGUMENT;
    uint32_t mask = (UINT32_C(1) << config->column_count) - UINT32_C(1);
    for (size_t i = 0U; i < config->column_count; ++i) {
        const UmiKanbanColumn *column = &config->columns[i];
        if (!UmiKanbanId(column->id, false) ||
            !UmiKanbanText(column->title, sizeof(column->title), false) ||
            column->stage < UMI_KANBAN_TODO || column->stage > UMI_KANBAN_BLOCKED ||
            column->wip_limit > config->card_capacity || (config->transitions[i] & ~mask) != 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j) {
            if (strcmp(column->id, config->columns[j].id) == 0) return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiKanbanCardCheck(const UmiKanbanCard *card)
{
    if (card == NULL || !UmiKanbanId(card->id, false) ||
        !UmiKanbanText(card->title, sizeof(card->title), false) ||
        !UmiKanbanText(card->description, sizeof(card->description), true) ||
        !UmiKanbanId(card->assignee, true) ||
        !UmiKanbanText(card->blocked_reason, sizeof(card->blocked_reason), true) ||
        card->priority < UMI_KANBAN_LOW || card->priority > UMI_KANBAN_URGENT ||
        card->label_count > UMI_KANBAN_MAX_LABELS || card->check_count > UMI_KANBAN_MAX_CHECKS ||
        card->dependency_count > UMI_KANBAN_MAX_DEPENDENCIES) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < card->label_count; ++i) {
        if (!UmiKanbanText(card->labels[i], sizeof(card->labels[i]), false)) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j) {
            if (strcmp(card->labels[i], card->labels[j]) == 0) return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    for (size_t i = 0U; i < card->check_count; ++i) {
        if (!UmiKanbanText(card->checks[i].text, sizeof(card->checks[i].text), false) ||
            (card->checks[i].complete != 0 && card->checks[i].complete != 1))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (size_t i = 0U; i < card->dependency_count; ++i) {
        if (!UmiKanbanId(card->dependencies[i], false)) return UMI_STATUS_INVALID_ARGUMENT;
        if (strcmp(card->id, card->dependencies[i]) == 0) return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j) {
            if (strcmp(card->dependencies[i], card->dependencies[j]) == 0) return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    return UMI_STATUS_OK;
}
size_t UmiKanbanUnfinishedDependencies(const UmiKanbanBoard *board, const UmiKanbanCard *card)
{
    size_t count = 0U;
    for (size_t i = 0U; i < card->dependency_count; ++i) {
        size_t found = UmiKanbanFind(board, card->dependencies[i]);
        if (found == UMI_KANBAN_MISSING || board->rows[found].archived ||
            board->config.columns[board->rows[found].column].stage != UMI_KANBAN_DONE) ++count;
    }
    return count;
}

/* Iterative depth-first traversal bounds both work and stack use. Large boards
 * must not turn a long dependency chain into recursive native stack frames. */
static UmiStatus ValidateCycles(const UmiKanbanBoard *board, UmiKanbanProblem *problem)
{
    unsigned char colors[UMI_KANBAN_MAX_CARDS] = {0};
    size_t stack[UMI_KANBAN_MAX_CARDS], next[UMI_KANBAN_MAX_CARDS];
    for (size_t root = 0U; root < board->count; ++root) {
        if (colors[root] != 0U) continue;
        size_t depth = 1U;
        stack[0] = root; next[0] = 0U; colors[root] = 1U;
        while (depth != 0U) {
            size_t top = depth - 1U, index = stack[top];
            const UmiKanbanCard *card = &board->rows[index].card;
            if (next[top] == card->dependency_count) {
                colors[index] = 2U; --depth; continue;
            }
            size_t child = UmiKanbanFind(board, card->dependencies[next[top]++]);
            if (child == UMI_KANBAN_MISSING) return UMI_STATUS_INTERNAL_ERROR;
            if (colors[child] == 1U)
                return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_CYCLE,
                    UMI_STATUS_INVALID_STATE, card->id, NULL);
            if (colors[child] == 0U) {
                if (depth == UMI_KANBAN_MAX_CARDS) return UMI_STATUS_INTERNAL_ERROR;
                stack[depth] = child; next[depth] = 0U; colors[child] = 1U; ++depth;
            }
        }
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiKanbanValidate(const UmiKanbanBoard *board, UmiKanbanProblem *problem)
{
    UmiStatus status = UmiKanbanConfigCheck(&board->config);
    if (status != UMI_STATUS_OK || board->count > board->config.card_capacity)
        return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_ARGUMENT,
            UMI_STATUS_INVALID_ARGUMENT, NULL, NULL);
    /* Directory validation also protects archive imports before publication. */
    if (UmiKanbanDevelopersCheck(board) != UMI_STATUS_OK)
        return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_ARGUMENT,
            UMI_STATUS_INVALID_ARGUMENT, NULL, NULL);
    size_t counts[UMI_KANBAN_MAX_COLUMNS] = {0};
    for (size_t i = 0U; i < board->count; ++i) {
        const UmiKanbanRow *row = &board->rows[i];
        status = UmiKanbanCardCheck(&row->card);
        if (status != UMI_STATUS_OK || row->column >= board->config.column_count ||
            (row->archived != 0 && row->archived != 1))
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_ARGUMENT,
                UMI_STATUS_INVALID_ARGUMENT, NULL, NULL);
        for (size_t j = 0U; j < i; ++j) {
            if (strcmp(row->card.id, board->rows[j].card.id) == 0)
                return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_DUPLICATE,
                    UMI_STATUS_ALREADY_EXISTS, row->card.id, NULL);
        }
    }
    /* Validate every bounded field before following a dependency to any row.
     * Portable imports may place a malformed row after a valid reference. */
    for (size_t i = 0U; i < board->count; ++i) {
        const UmiKanbanRow *row = &board->rows[i];
        const UmiKanbanColumn *column = &board->config.columns[row->column];
        if (!row->archived) ++counts[row->column];
        for (size_t j = 0U; j < row->card.dependency_count; ++j) {
            size_t dependency = UmiKanbanFind(board, row->card.dependencies[j]);
            if (dependency == UMI_KANBAN_MISSING || (!row->archived && board->rows[dependency].archived))
                return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_DEPENDENCY,
                    UMI_STATUS_INVALID_STATE, row->card.id, column->id);
        }
        if (!row->archived && column->stage == UMI_KANBAN_DONE) {
            if (row->card.blocked_reason[0] != '\0')
                return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_BLOCKED,
                    UMI_STATUS_INVALID_STATE, row->card.id, column->id);
            for (size_t j = 0U; j < row->card.check_count; ++j) {
                if (!row->card.checks[j].complete)
                    return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_CHECKLIST,
                        UMI_STATUS_INVALID_STATE, row->card.id, column->id);
            }
            if (UmiKanbanUnfinishedDependencies(board, &row->card) != 0U)
                return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_DEPENDENCY,
                    UMI_STATUS_INVALID_STATE, row->card.id, column->id);
        }
    }
    for (size_t i = 0U; i < board->config.column_count; ++i) {
        size_t limit = board->config.columns[i].wip_limit;
        if (limit != 0U && counts[i] > limit)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_WIP,
                UMI_STATUS_CAPACITY_EXCEEDED, NULL, board->config.columns[i].id);
    }
    return ValidateCycles(board, problem);
}
