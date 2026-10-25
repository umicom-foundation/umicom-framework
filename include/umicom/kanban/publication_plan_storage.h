/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/publication_plan_storage.h
 *
 * PURPOSE:
 *   Read and atomically replace private publication plans independently of board documents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_KANBAN_PUBLICATION_PLAN_STORAGE_H
#define UMICOM_KANBAN_PUBLICATION_PLAN_STORAGE_H
#include "umicom/kanban/publication_plan.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Existing absolute private directory and one ordinary .umipublic leaf.
 * No directory creation, board mutation, HTML generation or upload occurs.
 * Save replaces through the rooted-file contract; writers must be serialized.
 * Load publishes an independent candidate only after full validation.
 * These synchronous operations belong on a host worker, not the GTK thread. */
UmiStatus UmiKanbanPublicationPlanLoad(const char *directory, const char *filename,
                                       UmiKanbanPublicationPlan **out);
UmiStatus UmiKanbanPublicationPlanSave(const char *directory, const char *filename,
                                       const UmiKanbanPublicationPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
