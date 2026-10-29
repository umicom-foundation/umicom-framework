/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_selection.c
 *
 * PURPOSE:
 *   Open copied search or help identities through one verified reader lifecycle.
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

/* Preserve shelf-first dispatch for review. Continue an existing reader from
 * its owned pack; only a new session needs a current catalogue copy. */
#if 0
UmiStatus LearningCatalogueOpenSelection(LearningCatalogue *owner,
    const UmiLearningPackInfo *info, const UmiLearningPackResource *resource)
{
    if (owner == NULL || info == NULL || resource == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    if (owner->state.busy) return UMI_STATUS_BUSY;
    LearningCatalogueJob *job = g_try_new0(LearningCatalogueJob, 1U);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    g_weak_ref_init(&job->root, owner->root);
    job->action = LEARNING_CATALOGUE_OPEN;
    job->selected.pack = *info;
    job->selected.resource = *resource;
    UmiLearningPack *pack = NULL;
    UmiStatus status = UmiLearningShelfCopyPack(owner->shelf, info->id, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningShelfCreate(&job->selection_shelf);
    if (status == UMI_STATUS_OK) status = UmiLearningShelfAdopt(job->selection_shelf, &pack);
    UmiLearningPackDestroy(pack);
    if (status != UMI_STATUS_OK) { LearningCatalogueJobFree(job); return status; }
    /* Search and contextual help share per-pack history. Transfer the reader
     * only after all request metadata has an independent owner. */
    if (owner->reader != NULL && owner->state.reader.pack.revision == info->revision &&
        strcmp(owner->state.reader.pack.id, info->id) == 0) {
        job->continues_reader = 1;
        job->reader = owner->reader;
        owner->reader = NULL;
    }
    GtkWidget *widget = g_object_ref(owner->root);
    status = LearningCatalogueDispatch(owner, job);
    g_object_unref(widget);
    return status;
}
#endif
/* Preserve document-only dispatch for review. The shared request now carries
 * an optional copied text location before dispatch, so reentrant notifications
 * cannot attach a search position to a different reader request. */
#if 0
UmiStatus LearningCatalogueOpenSelection(LearningCatalogue *owner,
    const UmiLearningPackInfo *info, const UmiLearningPackResource *resource)
{
    if (owner == NULL || info == NULL || resource == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    if (owner->state.busy) return UMI_STATUS_BUSY;
    LearningCatalogueJob *job = g_try_new0(LearningCatalogueJob, 1U);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    g_weak_ref_init(&job->root, owner->root);
    job->action = LEARNING_CATALOGUE_OPEN;
    job->selected.pack = *info;
    job->selected.resource = *resource;
    UmiStatus status = UMI_STATUS_OK;
    /* An existing reader owns its metadata independently of the catalogue.
     * The worker validates the requested revision and digest before navigation. */
    if (owner->reader != NULL && owner->state.reader.pack.revision == info->revision &&
        strcmp(owner->state.reader.pack.id, info->id) == 0) {
        job->continues_reader = 1;
        job->reader = owner->reader;
        owner->reader = NULL;
    } else {
        UmiLearningPack *pack = NULL;
        status = UmiLearningShelfCopyPack(owner->shelf, info->id, &pack);
        if (status == UMI_STATUS_OK) status = UmiLearningShelfCreate(&job->selection_shelf);
        if (status == UMI_STATUS_OK) status = UmiLearningShelfAdopt(job->selection_shelf, &pack);
        UmiLearningPackDestroy(pack);
        if (status != UMI_STATUS_OK) { LearningCatalogueJobFree(job); return status; }
    }
    GtkWidget *widget = g_object_ref(owner->root);
    status = LearningCatalogueDispatch(owner, job);
    g_object_unref(widget);
    return status;
}
#endif
static UmiStatus OpenSelectionRequest(LearningCatalogue *owner,
    const UmiLearningPackInfo *info, const UmiLearningPackResource *resource,
    const UmiLearningTextHit *text_hit)
{
    if (owner == NULL || info == NULL || resource == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    if (owner->state.busy) return UMI_STATUS_BUSY;
    LearningCatalogueJob *job = g_try_new0(LearningCatalogueJob, 1U);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    g_weak_ref_init(&job->root, owner->root);
    job->action = LEARNING_CATALOGUE_OPEN;
    job->selected.pack = *info;
    job->selected.resource = *resource;
    if (text_hit != NULL) {
        job->text_hit = *text_hit;
        job->selects_text_hit = 1;
    }
    UmiStatus status = UMI_STATUS_OK;
    /* An existing reader owns its metadata independently of the catalogue.
     * The worker validates the requested revision and digest before navigation. */
    if (owner->reader != NULL && owner->state.reader.pack.revision == info->revision &&
        strcmp(owner->state.reader.pack.id, info->id) == 0) {
        job->continues_reader = 1;
        job->reader = owner->reader;
        owner->reader = NULL;
    } else {
        UmiLearningPack *pack = NULL;
        status = UmiLearningShelfCopyPack(owner->shelf, info->id, &pack);
        if (status == UMI_STATUS_OK) status = UmiLearningShelfCreate(&job->selection_shelf);
        if (status == UMI_STATUS_OK) status = UmiLearningShelfAdopt(job->selection_shelf, &pack);
        UmiLearningPackDestroy(pack);
        if (status != UMI_STATUS_OK) { LearningCatalogueJobFree(job); return status; }
    }
    GtkWidget *widget = g_object_ref(owner->root);
    status = LearningCatalogueDispatch(owner, job);
    g_object_unref(widget);
    return status;
}

UmiStatus LearningCatalogueOpenSelection(LearningCatalogue *owner,
    const UmiLearningPackInfo *info, const UmiLearningPackResource *resource)
{
    return OpenSelectionRequest(owner, info, resource, NULL);
}
UmiStatus LearningCatalogueOpenTextHit(LearningCatalogue *owner, const UmiLearningTextHit *hit)
{
    if (hit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return OpenSelectionRequest(owner, &hit->pack, &hit->resource, hit);
}
UmiStatus UmiLearningCatalogueGtk4OpenHelp(GtkWidget *widget, const UmiLearningHelpMap *map,
    const UmiLearningHelpRequest *request)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    if (owner->state.busy) return UMI_STATUS_BUSY;
    UmiLearningHelpSelection selected;
    UmiStatus status = UmiLearningHelpResolve(map, owner->shelf, request, &selected);
    return status == UMI_STATUS_OK ?
        LearningCatalogueOpenSelection(owner, &selected.pack, &selected.resource) : status;
}
