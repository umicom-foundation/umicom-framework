/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_practice.c
 *
 * PURPOSE:
 *   Capture an explicit practice destination without borrowing mutable UI state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_catalogue_private.h"
#include <string.h>

static UmiStatus PracticeOwner(GtkWidget *widget, LearningCatalogue **out)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    if (owner->state.busy) return UMI_STATUS_BUSY;
    if (!owner->state.reader.has_document) return UMI_STATUS_INVALID_STATE;
    *out = owner;
    return UMI_STATUS_OK;
}
static UmiStatus CopySelectedPack(LearningCatalogue *owner, UmiLearningPack **out)
{
    const UmiLearningReaderSnapshot *selected = &owner->state.reader;
    UmiStatus status = UmiLearningShelfCopyPack(owner->shelf, selected->pack.id, out);
    UmiLearningPackInfo info;
    UmiLearningPackResource resource;
    if (status == UMI_STATUS_OK) status = UmiLearningPackCopyInfo(*out, &info);
    if (status == UMI_STATUS_OK && info.revision != selected->pack.revision)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = UmiLearningPackFindResource(*out, selected->selection.resource.id, &resource);
    if (status == UMI_STATUS_OK &&
        strcmp(resource.sha256, selected->selection.resource.sha256) != 0)
        status = UMI_STATUS_INVALID_STATE;
    if (status != UMI_STATUS_OK) {
        UmiLearningPackDestroy(*out);
        *out = NULL;
    }
    return status;
}

UmiStatus UmiLearningCatalogueGtk4PreviewExample(GtkWidget *widget,
    UmiLearningPackResource *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    LearningCatalogue *owner = NULL;
    UmiStatus status = PracticeOwner(widget, &owner);
    if (status != UMI_STATUS_OK) return status;
    UmiLearningPack *pack = NULL;
    status = CopySelectedPack(owner, &pack);
    if (status == UMI_STATUS_OK)
        status = UmiLearningPracticeResolveExample(pack, owner->state.reader.selection.resource.id, out);
    UmiLearningPackDestroy(pack);
    return status;
}
UmiStatus UmiLearningCatalogueGtk4CopyExample(GtkWidget *widget,
    const char *destination_root, const char *destination_path)
{
    LearningCatalogue *owner = NULL;
    UmiStatus status = PracticeOwner(widget, &owner);
    if (status == UMI_STATUS_OK)
        status = UmiRootedFileValidatePath(destination_root, destination_path);
    if (status != UMI_STATUS_OK) return status;
    LearningCatalogueJob *job = g_try_new0(LearningCatalogueJob, 1U);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    g_weak_ref_init(&job->root, owner->root);
    job->action = LEARNING_CATALOGUE_COPY;
    job->copy.pack = owner->state.reader.pack;
    job->copy.selection = owner->state.reader.selection.resource;
    status = umi_path_copy(job->copy.destination_root, sizeof(job->copy.destination_root), destination_root);
    if (status == UMI_STATUS_OK)
        status = umi_path_copy(job->copy.destination_path, sizeof(job->copy.destination_path), destination_path);
    if (status == UMI_STATUS_OK &&
        (!g_utf8_validate(job->copy.destination_root, -1, NULL) ||
         !g_utf8_validate(job->copy.destination_path, -1, NULL)))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK) status = CopySelectedPack(owner, &job->pack);
    UmiLearningPackResource example;
    if (status == UMI_STATUS_OK)
        status = UmiLearningPracticeResolveExample(job->pack, job->copy.selection.id, &example);
    if (status != UMI_STATUS_OK) { LearningCatalogueJobFree(job); return status; }
    /* No source bytes, filename entry or reader pointer is borrowed by the job.
     * Admission does not alter the existing last-copy receipt. */
    g_object_ref(widget);
    status = LearningCatalogueDispatch(owner, job);
    g_object_unref(widget);
    return status;
}
UmiStatus UmiLearningCatalogueGtk4CopyLastReceipt(GtkWidget *widget,
    UmiLearningCatalogueCopyReceipt *out)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!owner->state.has_copy_result) return UMI_STATUS_NOT_FOUND;
    *out = owner->copy;
    return UMI_STATUS_OK;
}
