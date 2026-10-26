/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/completion_storage.c
 *
 * PURPOSE:
 *   Write new review evidence without overwriting earlier decisions, and admit bounded archives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/kanban/completion_storage.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus Location(const char *path)
{
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    const char suffix[] = ".umicompletion";
    size_t length = strlen(path), tail = sizeof suffix - 1U;
    return length > tail && strcmp(path + length - tail, suffix) == 0
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiKanbanCompletionPlanWriteNew(const char *path,
    const UmiKanbanCompletionPlan *plan)
{
    UmiStatus status = Location(path);
    size_t size = 0U;
    if (status == UMI_STATUS_OK) status = UmiKanbanCompletionPlanEncode(plan, NULL, 0U, &size);
    void *bytes = NULL;
    if (status == UMI_STATUS_OK) {
        bytes = malloc(size);
        if (!bytes) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK) status = UmiKanbanCompletionPlanEncode(plan, bytes, size, &size);
    /* Complete validation precedes filesystem mutation. Exclusive creation
     * avoids a check-then-overwrite race against another retained review. */
    UmiOutputFile *file = NULL;
    if (status == UMI_STATUS_OK) status = UmiOutputFileCreate(path, &file);
    if (status == UMI_STATUS_OK) status = UmiOutputFileWrite(file, bytes, size);
    if (file) {
        UmiStatus close_status = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK) status = close_status;
    }
    UmiOutputFileDestroy(file);
    free(bytes);
    return status;
}
UmiStatus UmiKanbanCompletionPlanLoadFile(const char *path, UmiKanbanCompletionPlan **out)
{
    if (!out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = Location(path);
    unsigned char *bytes = NULL;
    size_t size = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(path, UMI_KANBAN_COMPLETION_MAXIMUM_BYTES, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = size ? UmiKanbanCompletionPlanDecode(bytes, size, out) : UMI_STATUS_PARSE_ERROR;
    UmiInputFileFree(bytes);
    return status;
}
