/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/board_internal.h
 * PURPOSE: Keep board ownership, validation and bounded ordering private to the Kanban component.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_KANBAN_BOARD_INTERNAL_H
#define UMICOM_KANBAN_BOARD_INTERNAL_H
#include "umicom/kanban/board.h"
#include <stdbool.h>
#define UMI_KANBAN_MISSING SIZE_MAX

typedef struct UmiKanbanRow {
    UmiKanbanCard card;
    size_t column;
    int archived;
} UmiKanbanRow;
struct UmiKanbanBoard {
    UmiKanbanConfig config;
    UmiKanbanRow *rows;
    size_t count;
    uint64_t revision;
    uint64_t next_sequence;
    UmiKanbanActivity activity[UMI_KANBAN_ACTIVITY_CAPACITY];
    size_t activity_start, activity_count;
};
bool UmiKanbanId(const char *text, bool optional);
bool UmiKanbanText(const char *text, size_t capacity, bool optional);
size_t UmiKanbanFind(const UmiKanbanBoard *board, const char *id);
size_t UmiKanbanColumnFind(const UmiKanbanConfig *config, const char *id);
UmiStatus UmiKanbanConfigCheck(const UmiKanbanConfig *config);
UmiStatus UmiKanbanCardCheck(const UmiKanbanCard *card);
UmiStatus UmiKanbanValidate(const UmiKanbanBoard *board, UmiKanbanProblem *problem);
UmiStatus UmiKanbanProblemSet(UmiKanbanProblem *problem, UmiKanbanProblemKind kind,
    UmiStatus status, const char *card, const char *column);
void UmiKanbanCopyRow(const UmiKanbanBoard *board, size_t index, UmiKanbanCardSnapshot *out);
size_t UmiKanbanUnfinishedDependencies(const UmiKanbanBoard *board, const UmiKanbanCard *card);
#endif
