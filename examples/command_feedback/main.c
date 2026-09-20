/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/command_feedback/main.c
 * PURPOSE:
 *   Follow a Notes save request from a panel to a controller, keep the draft
 *   after a refused save, then verify the bytes written by a successful retry.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/presentation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The file belongs to this example. Nothing outside it is opened or changed.
 * rejectSave simulates a destination that is temporarily unavailable. */
typedef struct NotesContext {
    FILE *file;
    const char *draft;
    int rejectSave;
} NotesContext;

/* A controller translates an intent into work. A failure result and its
 * explanation travel together; returning OK would promise more than happened. */
static UmiStatus NotesController(void *context,
    const UmiApplicationPresentationPanelPlacement *placement,
    UmiApplicationPresentationSurfaceEvent event, const char *command,
    UmiApplicationPresentationSurfaceUpdate *update)
{
    NotesContext *notes = context;
    (void)placement;
    *update = (UmiApplicationPresentationSurfaceUpdate){0};
    update->state = UMI_APPLICATION_PRESENTATION_STATE_READY;
    update->dirty = 1;
    if (event != UMI_APPLICATION_PRESENTATION_EVENT_COMMAND) return UMI_STATUS_OK;
    if (strcmp(command, "notes.save") != 0) {
        return UmiApplicationPresentationSurfaceFailure(UMI_STATUS_NOT_IMPLEMENTED,
            "This Notes controller handles notes.save only.", update);
    }
    if (notes->rejectSave) {
        return UmiApplicationPresentationSurfaceFailure(UMI_STATUS_IO_ERROR,
            "The Notes destination is unavailable. Keep the draft and try again.", update);
    }
    if (fseek(notes->file, 0L, SEEK_SET) != 0 ||
        fputs(notes->draft, notes->file) == EOF || fflush(notes->file) != 0) {
        return UmiApplicationPresentationSurfaceFailure(UMI_STATUS_IO_ERROR,
            "The Notes file could not be written. The draft is still open.", update);
    }
    update->dirty = 0;
    (void)snprintf(update->message, sizeof(update->message), "The Notes file was saved.");
    return UMI_STATUS_OK;
}

int main(void)
{
    /* The bounded runtime contains several panel records. Allocate it on the
     * heap rather than assuming every platform has a large main-thread stack. */
    UmiApplicationPresentationSurfaceRuntime *runtime = calloc(1U, sizeof(*runtime));
    UmiApplicationPresentationHeadlessSurfaceHost *host = calloc(1U, sizeof(*host));
    UmiApplicationPresentationSurfaceItem *panel;
    UmiApplicationPresentationCommandAvailability availability;
    NotesContext notes = {NULL, "Umicom Notes: review the project before publishing.\n", 1};
    const char *component = "umicom.development.editor";
    char saved[128] = {0};
    int result = 1;
    if (runtime == NULL || host == NULL) goto cleanup;
    notes.file = tmpfile();
    if (notes.file == NULL) goto cleanup;

    /* Reuse the existing editor panel in a headless Studio recipe for this
     * API lesson. This does not replace Studio's production editor or Save. */
    if (umi_application_presentation_surface_runtime_init(
            "org.umicom.workspace.studio.standard", runtime) != UMI_STATUS_OK) goto cleanup;
    if (umi_application_presentation_surface_runtime_register_controller(
            runtime, component, NotesController, &notes) != UMI_STATUS_OK) goto cleanup;
    umi_application_presentation_headless_surface_host_init(host);
    if (umi_application_presentation_surface_runtime_bind_host(runtime,
            umi_application_presentation_headless_surface_host_interface(host)) != UMI_STATUS_OK) goto cleanup;
    if (umi_application_presentation_surface_runtime_start(runtime) != UMI_STATUS_OK) goto cleanup;
    panel = umi_application_presentation_surface_session_find(&runtime->session, component);
    if (panel == NULL) goto cleanup;

    /* Inspection does not run a command. Even a connected route can later fail. */
    if (UmiApplicationPresentationSurfaceCommandCheck(runtime, component, &availability)
            != UMI_STATUS_OK || !availability.can_dispatch) goto cleanup;
    printf("Before Save: unsaved=%d\n", panel->dirty);
    if (umi_application_presentation_surface_runtime_command(runtime, component,
            "notes.save") != UMI_STATUS_IO_ERROR || panel->dirty != 1) goto cleanup;
    printf("Save refused: %s\nUnsaved=%d\n", panel->message, panel->dirty);

    /* Retry only after resolving the cause. Check the file, not just the return
     * code or changed label, before considering the exercise complete. */
    notes.rejectSave = 0;
    if (umi_application_presentation_surface_runtime_command(runtime, component,
            "notes.save") != UMI_STATUS_OK || panel->dirty != 0) goto cleanup;
    if (fseek(notes.file, 0L, SEEK_SET) != 0 || fgets(saved, sizeof(saved), notes.file) == NULL ||
        strcmp(saved, notes.draft) != 0) goto cleanup;
    printf("Retry: %s\nVerified saved text: %s", panel->message, saved);
    result = 0;
cleanup:
    if (runtime != NULL && runtime->started)
        (void)umi_application_presentation_surface_runtime_stop(runtime);
    if (notes.file != NULL && fclose(notes.file) != 0) result = 1;
    free(host);
    free(runtime);
    return result;
}
