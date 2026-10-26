/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/completion_storage.h
 *
 * PURPOSE:
 *   Retain explicit review decisions as private, separately owned evidence files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_KANBAN_COMPLETION_STORAGE_H
#define UMICOM_KANBAN_COMPLETION_STORAGE_H
#include "umicom/kanban/completion_plan.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Use one ordinary absolute .umicompletion path in an existing trusted private
 * directory. These synchronous operations belong on a worker in native hosts.
 * No board mutation, directory creation, repository write, upload, encryption
 * or authentication occurs. The file contains private review information.
 *
 * WriteNew validates and encodes the whole plan before exclusive creation.
 * An existing leaf is never replaced, including another review or a symlink.
 * A later I/O failure can leave a partial new file; it is retained for diagnosis.
 * Loading validates the complete bounded archive before publishing an owned
 * plan. A loaded plan still needs Check against the live board before use.
 *
 * Source file identity and reviewer authority are caller responsibilities.
 * Parent aliases may be followed by the native file service; this is not
 * workspace confinement. Serialize writers and select a directory you control.
 * Saving the board and this evidence are separate operations, not a transaction. */
UmiStatus UmiKanbanCompletionPlanWriteNew(const char *path,
    const UmiKanbanCompletionPlan *plan);
UmiStatus UmiKanbanCompletionPlanLoadFile(const char *path,
    UmiKanbanCompletionPlan **out);
#ifdef __cplusplus
}
#endif
#endif
