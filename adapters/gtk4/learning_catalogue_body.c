/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_body.c
 *
 * PURPOSE:
 *   Transfer immutable learning indexes to workers and publish complete body-search results.
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

static UmiStatus BodyReady(LearningCatalogue *owner)
{
    if(owner==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(owner->state.retired)return UMI_STATUS_INVALID_STATE;
    return owner->state.busy ? UMI_STATUS_BUSY : UMI_STATUS_OK;
}
static UmiStatus CapturePhrase(const char *phrase,UmiLearningTextQuery *query)
{
    if(phrase==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    size_t length=0U;
    while(length<sizeof(query->phrase) && phrase[length]!='\0') {
        unsigned char value=(unsigned char)phrase[length];
        if(value<32U || value==127U)return UMI_STATUS_INVALID_ARGUMENT;
        ++length;
    }
    if(length==sizeof(query->phrase))return UMI_STATUS_CAPACITY_EXCEEDED;
    if(length==0U || !g_utf8_validate(phrase,(gssize)length,NULL))return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(query->phrase,phrase,length+1U);
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningCatalogueGtk4CopyBodySearchState(GtkWidget *widget,
    UmiLearningCatalogueBodySearchState *out)
{
    LearningCatalogue *owner=LearningCatalogueOwner(widget);
    if(owner==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningShelfSnapshot shelf;
    UmiStatus status=UmiLearningShelfCopySnapshot(owner->shelf,&shelf);
    if(status!=UMI_STATUS_OK)return status;
    UmiLearningCatalogueBodySearchState copy=owner->text_state;
    copy.index_current=copy.has_index && copy.index.shelf_generation==shelf.generation;
    copy.results_current=copy.has_results && copy.results.shelf_generation==shelf.generation;
    *out=copy;
    return UMI_STATUS_OK;
}
/* Preserve the unfiltered request interface implementation for review. Its
 * public entry point remains; both routes now dispatch one owned validated query. */
#if 0
UmiStatus UmiLearningCatalogueGtk4SearchBodies(GtkWidget *widget,const char *phrase,
    int case_sensitive,int whole_word,int refresh)
{
    LearningCatalogue *owner=LearningCatalogueOwner(widget);
    UmiStatus status=BodyReady(owner);
    if(status!=UMI_STATUS_OK)return status;
    if((case_sensitive!=0 && case_sensitive!=1) || (whole_word!=0 && whole_word!=1) ||
        (refresh!=0 && refresh!=1))return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningTextQuery query;
    UmiLearningTextQueryInit(&query);
    query.case_sensitive=case_sensitive;query.whole_word=whole_word;query.maximum_results=128U;
    status=CapturePhrase(phrase,&query);
    if(status!=UMI_STATUS_OK)return status;
    UmiLearningCatalogueBodySearchState state;
    status=UmiLearningCatalogueGtk4CopyBodySearchState(widget,&state);
    if(status!=UMI_STATUS_OK)return status;
    LearningCatalogueJob *job=g_try_new0(LearningCatalogueJob,1U);
    if(job==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    g_weak_ref_init(&job->root,widget);
    job->action=LEARNING_CATALOGUE_TEXT_SEARCH;
    job->host=owner->host;job->text_query=query;
    if(!refresh && state.index_current && owner->text_index!=NULL) {
        /* Only one catalogue job runs at a time. Transferring the index avoids
         * borrowing the widget owner or copying every document for each query. */
        job->text_index=owner->text_index;owner->text_index=NULL;
        job->text_index_reused=1;
    } else status=UmiLearningShelfClone(owner->shelf,&job->selection_shelf);
    if(status!=UMI_STATUS_OK) { LearningCatalogueJobFree(job);return status; }
    g_object_ref(widget);
    status=LearningCatalogueDispatch(owner,job);
    g_object_unref(widget);
    return status;
}
#endif
UmiStatus UmiLearningCatalogueGtk4SearchBodiesWithQuery(GtkWidget *widget,
    const UmiLearningTextQuery *query,int refresh)
{
    LearningCatalogue *owner=LearningCatalogueOwner(widget);
    UmiStatus status=BodyReady(owner);
    if(status!=UMI_STATUS_OK)return status;
    if(refresh!=0 && refresh!=1)return UMI_STATUS_INVALID_ARGUMENT;
    status=UmiLearningTextQueryValidate(query);
    if(status!=UMI_STATUS_OK)return status;
    if(query->cancellation!=NULL || query->cancellation_user!=NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningCatalogueBodySearchState state;
    status=UmiLearningCatalogueGtk4CopyBodySearchState(widget,&state);
    if(status!=UMI_STATUS_OK)return status;
    LearningCatalogueJob *job=g_try_new0(LearningCatalogueJob,1U);
    if(job==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    g_weak_ref_init(&job->root,widget);
    job->action=LEARNING_CATALOGUE_TEXT_SEARCH;
    job->host=owner->host;job->text_query=*query;
    if(!refresh && state.index_current && owner->text_index!=NULL) {
        /* Only one catalogue job runs at a time. Transferring the index avoids
         * borrowing the widget owner or copying every document for each query. */
        job->text_index=owner->text_index;owner->text_index=NULL;
        job->text_index_reused=1;
    } else status=UmiLearningShelfClone(owner->shelf,&job->selection_shelf);
    if(status!=UMI_STATUS_OK) { LearningCatalogueJobFree(job);return status; }
    g_object_ref(widget);
    status=LearningCatalogueDispatch(owner,job);
    g_object_unref(widget);
    return status;
}

UmiStatus UmiLearningCatalogueGtk4SearchBodies(GtkWidget *widget,const char *phrase,
    int case_sensitive,int whole_word,int refresh)
{
    UmiStatus status=BodyReady(LearningCatalogueOwner(widget));
    if(status!=UMI_STATUS_OK)return status;
    UmiLearningTextQuery query;
    UmiLearningTextQueryInit(&query);
    query.case_sensitive=case_sensitive;query.whole_word=whole_word;query.maximum_results=128U;
    status=CapturePhrase(phrase,&query);
    return status==UMI_STATUS_OK ? UmiLearningCatalogueGtk4SearchBodiesWithQuery(widget,&query,refresh) : status;
}
void LearningCatalogueBodyWorker(LearningCatalogueJob *job)
{
    job->status=UMI_STATUS_OK;
    if(job->text_index==NULL)
        job->status=UmiLearningTextIndexCreate(job->selection_shelf,NULL,NULL,NULL,
            &job->text_index,&job->text_problem);
    if(job->status==UMI_STATUS_OK)
        job->status=UmiLearningTextSearchCreate(job->text_index,&job->text_query,&job->text_results);
}
void LearningCatalogueBodyComplete(LearningCatalogue *owner,LearningCatalogueJob *job)
{
    GtkStringList *rows=NULL;
    UmiLearningTextIndexInfo index_info={0};
    UmiLearningTextSearchInfo search_info={0};
    if(job->status==UMI_STATUS_OK)
        job->status=UmiLearningTextIndexCopyInfo(job->text_index,&index_info);
    if(job->status==UMI_STATUS_OK)
        job->status=UmiLearningTextSearchCopyInfo(job->text_results,&search_info);
    if(job->status==UMI_STATUS_OK) {
        rows=gtk_string_list_new(NULL);
        for(size_t i=0U;job->status==UMI_STATUS_OK && i<search_info.result_count;++i) {
            UmiLearningTextHit hit;
            job->status=UmiLearningTextSearchResultAt(job->text_results,i,&hit);
            if(job->status==UMI_STATUS_OK) {
                char *label=g_strdup_printf("%s — %s [%s]\n%s%s",hit.resource.title,
                    hit.pack.id,hit.pack.locale,hit.preview,hit.preview_truncated ? " …" : "");
                gtk_string_list_append(rows,label);
                g_free(label);
            }
        }
    }
    /* A failed query still returns its earlier usable index. A newly built
     * candidate replaces the old index only together with successful results. */
    if(job->text_index_reused) {
        owner->text_index=job->text_index;job->text_index=NULL;
    }
    if(job->status==UMI_STATUS_OK) {
        if(!job->text_index_reused) {
            UmiLearningTextIndexDestroy(owner->text_index);
            owner->text_index=job->text_index;job->text_index=NULL;
        }
        UmiLearningTextSearchDestroy(owner->text_results);
        owner->text_results=job->text_results;job->text_results=NULL;
        owner->text_state.index=index_info;
        owner->text_state.results=search_info;
        owner->text_state.query=job->text_query;
        owner->text_state.has_index=1;owner->text_state.has_results=1;
    }
    owner->text_state.last_status=job->status;
    owner->text_state.problem=job->text_problem;
    owner->text_state.problem.status=job->status;
    if(job->status==UMI_STATUS_OK) {
        /* Copy owners and state before GTK notifications; busy remains set by
         * the dispatcher until this entire publication completes. */
        gtk_drop_down_set_model(GTK_DROP_DOWN(owner->text_rows),G_LIST_MODEL(rows));
        if(!owner->state.retired)
            gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->text_rows),
                search_info.result_count!=0U ? 0U : GTK_INVALID_LIST_POSITION);
    }
    g_clear_object(&rows);
    if(!owner->state.retired) {
        char *message=job->status==UMI_STATUS_OK ?
            g_strdup_printf("%zu matching documents; %zu shown%s. %s Read result verifies the selected file again.",
                search_info.matching_documents,search_info.result_count,
                search_info.truncated ? " (list limited)" : "",
                job->text_index_reused ? "Searched the existing text snapshot." : "Built a verified text snapshot.") :
            g_strdup_printf("%s. Pack: %s; resource: %s. Earlier results were kept.",
                umi_status_text(job->status),job->text_problem.pack_id,job->text_problem.resource_id);
        gtk_label_set_text(GTK_LABEL(owner->text_status),message);
        if(!owner->state.retired)LearningCatalogueMessage(owner,job->status,message);
        g_free(message);
    }
}
void LearningCatalogueBodyShelfChanged(LearningCatalogue *owner)
{
    if(owner->state.retired || owner->text_status==NULL)return;
    UmiLearningCatalogueBodySearchState state;
    if(UmiLearningCatalogueGtk4CopyBodySearchState(owner->root,&state)==UMI_STATUS_OK &&
        ((state.has_index && !state.index_current) || (state.has_results && !state.results_current)))
        gtk_label_set_text(GTK_LABEL(owner->text_status),
            "The learning library changed. Search again to build a current snapshot before reading these results.");
}
UmiStatus UmiLearningCatalogueGtk4BodySearchResultAt(GtkWidget *widget,size_t position,
    UmiLearningTextHit *out)
{
    LearningCatalogue *owner=LearningCatalogueOwner(widget);
    if(owner==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(owner->text_results==NULL)return UMI_STATUS_NOT_FOUND;
    return UmiLearningTextSearchResultAt(owner->text_results,position,out);
}
UmiStatus UmiLearningCatalogueGtk4ReadBodySearchResult(GtkWidget *widget,size_t position)
{
    LearningCatalogue *owner=LearningCatalogueOwner(widget);
    UmiStatus status=BodyReady(owner);
    if(status!=UMI_STATUS_OK)return status;
    UmiLearningCatalogueBodySearchState state;
    status=UmiLearningCatalogueGtk4CopyBodySearchState(widget,&state);
    if(status!=UMI_STATUS_OK)return status;
    if(!state.has_results)return UMI_STATUS_NOT_FOUND;
    if(!state.results_current)return UMI_STATUS_INVALID_STATE;
    UmiLearningTextHit hit;
    status=UmiLearningTextSearchResultAt(owner->text_results,position,&hit);
    /* Preserve document-only opening; the shared request now applies the copied
     * location only after the reader verifies its resource identity again. */
#if 0
    return status==UMI_STATUS_OK ? LearningCatalogueOpenSelection(owner,&hit.pack,&hit.resource) : status;
#endif
    return status==UMI_STATUS_OK ? LearningCatalogueOpenTextHit(owner,&hit) : status;
}
