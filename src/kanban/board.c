/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/board.c
 * PURPOSE: Own portable planning state independently of application widgets and storage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "board_internal.h"
#include <stdlib.h>
#include <string.h>

UmiStatus umi_kanban_board_create(const UmiKanbanConfig *config, UmiKanbanBoard **out_board)
{
    if (out_board == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiKanbanConfigCheck(config);
    if (status != UMI_STATUS_OK) return status;
    UmiKanbanBoard *board = calloc(1U, sizeof(*board));
    if (board == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    board->rows = calloc(config->card_capacity, sizeof(*board->rows));
    if (board->rows == NULL) { free(board); return UMI_STATUS_OUT_OF_MEMORY; }
    board->config = *config;
    board->revision = 1U;
    board->next_sequence = 1U;
    *out_board = board;
    return UMI_STATUS_OK;
}
UmiStatus umi_kanban_board_create_development(const char *id, const char *title,
    UmiKanbanBoard **out_board)
{
    if (!UmiKanbanId(id, false) || !UmiKanbanText(title, 256U, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanConfig config = {0};
    memcpy(config.id, id, strlen(id) + 1U);
    memcpy(config.title, title, strlen(title) + 1U);
    config.card_capacity = UMI_KANBAN_MAX_CARDS;
    config.column_count = 4U;
    const char *ids[] = {"todo", "doing", "testing", "done"};
    const char *titles[] = {"To do", "Doing", "Testing", "Done"};
    for (size_t i = 0U; i < 4U; ++i) {
        memcpy(config.columns[i].id, ids[i], strlen(ids[i]) + 1U);
        memcpy(config.columns[i].title, titles[i], strlen(titles[i]) + 1U);
        config.columns[i].stage = (UmiKanbanStage)i;
    }
    config.transitions[0] = UINT32_C(1) << 1U;
    config.transitions[1] = (UINT32_C(1) << 0U) | (UINT32_C(1) << 2U);
    config.transitions[2] = (UINT32_C(1) << 1U) | (UINT32_C(1) << 3U);
    config.transitions[3] = UINT32_C(1) << 0U;
    return umi_kanban_board_create(&config, out_board);
}
void umi_kanban_board_destroy(UmiKanbanBoard *board)
{
    if (board != NULL) { free(board->rows); free(board); }
}
uint64_t umi_kanban_board_revision(const UmiKanbanBoard *board)
{ return board != NULL ? board->revision : 0U; }
size_t umi_kanban_board_count(const UmiKanbanBoard *board)
{ return board != NULL ? board->count : 0U; }
UmiStatus umi_kanban_board_config(const UmiKanbanBoard *board, UmiKanbanConfig *out_config)
{
    if (board == NULL || out_config == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_config = board->config;
    return UMI_STATUS_OK;
}
void UmiKanbanCopyRow(const UmiKanbanBoard *board, size_t index, UmiKanbanCardSnapshot *out)
{
    memset(out, 0, sizeof(*out));
    out->card = board->rows[index].card;
    const char *column = board->config.columns[board->rows[index].column].id;
    memcpy(out->column_id, column, strlen(column) + 1U);
    out->archived = board->rows[index].archived;
    out->board_revision = board->revision;
}
UmiStatus umi_kanban_board_find(const UmiKanbanBoard *board, const char *id,
    UmiKanbanCardSnapshot *out_card)
{
    if (board == NULL || out_card == NULL || !UmiKanbanId(id, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiKanbanFind(board, id);
    if (index == UMI_KANBAN_MISSING) return UMI_STATUS_NOT_FOUND;
    UmiKanbanCopyRow(board, index, out_card);
    return UMI_STATUS_OK;
}
size_t umi_kanban_board_activity_count(const UmiKanbanBoard *board)
{ return board != NULL ? board->activity_count : 0U; }
UmiStatus umi_kanban_board_activity_at(const UmiKanbanBoard *board,
    uint64_t expected_revision, size_t index, UmiKanbanActivity *out_activity)
{
    if (board == NULL || out_activity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (expected_revision != board->revision) return UMI_STATUS_INVALID_STATE;
    if (index >= board->activity_count) return UMI_STATUS_NOT_FOUND;
    *out_activity = board->activity[(board->activity_start + index) % UMI_KANBAN_ACTIVITY_CAPACITY];
    return UMI_STATUS_OK;
}
