/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/developers.h
 * PURPOSE: Resolve stable developer identifiers to display names without granting account permissions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_KANBAN_DEVELOPERS_H
#define UMICOM_KANBAN_DEVELOPERS_H
#include "umicom/kanban/board.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_KANBAN_MAX_DEVELOPERS 128U
typedef struct UmiKanbanDeveloper {
    char id[UMI_KANBAN_ID_SIZE];
    char name[128];
} UmiKanbanDeveloper;
/* Developer records belong to the board and its portable archive. They are
 * display labels, not authenticated accounts or permission grants. Use a
 * PUT_DEVELOPER edit with card.id and card.title (name, at most 127 bytes).
 * An existing ID changes its display name without rewriting card assignments.
 * IDs are stable; names may repeat, so hosts must disambiguate by ID.
 * Historical card assignee IDs need not have a registered developer record. */
size_t umi_kanban_board_developer_count(const UmiKanbanBoard *board);
UmiStatus umi_kanban_board_developer_at(const UmiKanbanBoard *board,
    uint64_t expected_revision, size_t index, UmiKanbanDeveloper *out_developer);
UmiStatus umi_kanban_board_developer_find(const UmiKanbanBoard *board,
    uint64_t expected_revision, const char *id, UmiKanbanDeveloper *out_developer);
#ifdef __cplusplus
}
#endif
#endif
