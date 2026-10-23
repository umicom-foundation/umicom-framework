/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/developers.c
 * PURPOSE: Keep developer labels independent of UI lifetime and apply them through board transactions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "board_internal.h"
#include <string.h>
static size_t DeveloperIndex(const UmiKanbanBoard *board, const char *id)
{
    for (size_t i = 0U; i < board->developer_count; ++i)
        if (strcmp(board->developers[i].id, id) == 0) return i;
    return UMI_KANBAN_MISSING;
}
UmiStatus UmiKanbanDevelopersCheck(const UmiKanbanBoard *board)
{
    if (board->developer_count > UMI_KANBAN_MAX_DEVELOPERS) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < board->developer_count; ++i) {
        const UmiKanbanDeveloper *person = &board->developers[i];
        if (!UmiKanbanId(person->id, false) || !UmiKanbanText(person->name, sizeof(person->name), false))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(board->developers[j].id, person->id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiKanbanDeveloperPut(UmiKanbanBoard *board, const char *id, const char *name)
{
    if (!UmiKanbanId(id, false) || !UmiKanbanText(name, sizeof(board->developers[0].name), false))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = DeveloperIndex(board, id);
    if (index == UMI_KANBAN_MISSING) {
        if (board->developer_count == UMI_KANBAN_MAX_DEVELOPERS) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = board->developer_count++;
    }
    UmiKanbanDeveloper person = {0};
    memcpy(person.id, id, strlen(id) + 1U);
    memcpy(person.name, name, strlen(name) + 1U);
    board->developers[index] = person;
    return UMI_STATUS_OK;
}
size_t umi_kanban_board_developer_count(const UmiKanbanBoard *board)
{ return board == NULL ? 0U : board->developer_count; }
UmiStatus umi_kanban_board_developer_at(const UmiKanbanBoard *board,
    uint64_t expected_revision, size_t index, UmiKanbanDeveloper *out_developer)
{
    if (board == NULL || out_developer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (expected_revision != board->revision) return UMI_STATUS_INVALID_STATE;
    if (index >= board->developer_count) return UMI_STATUS_NOT_FOUND;
    *out_developer = board->developers[index];
    return UMI_STATUS_OK;
}
UmiStatus umi_kanban_board_developer_find(const UmiKanbanBoard *board,
    uint64_t expected_revision, const char *id, UmiKanbanDeveloper *out_developer)
{
    if (board == NULL || out_developer == NULL || !UmiKanbanId(id, false)) return UMI_STATUS_INVALID_ARGUMENT;
    if (expected_revision != board->revision) return UMI_STATUS_INVALID_STATE;
    size_t index = DeveloperIndex(board, id);
    if (index == UMI_KANBAN_MISSING) return UMI_STATUS_NOT_FOUND;
    *out_developer = board->developers[index];
    return UMI_STATUS_OK;
}
