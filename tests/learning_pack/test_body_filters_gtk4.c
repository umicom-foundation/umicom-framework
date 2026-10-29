/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_body_filters_gtk4.c
 *
 * PURPOSE:
 *   Verify copied native query filters, retained results and error-notice reentrancy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "text_fixture.h"
#include "umicom/ui/gtk4/learning_catalogue.h"
#include "../support/gtk4_expanders.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);result=1;goto cleanup; } } while(0)
static int Finish(void)
{
    gint64 deadline=g_get_monotonic_time()+15*G_TIME_SPAN_SECOND;
    while(UmiLearningCatalogueGtk4PendingReads()!=0U && g_get_monotonic_time()<deadline) {
        while(g_main_context_iteration(NULL,FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads()==0U;
}
static GtkWidget *Find(GtkWidget *widget,const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(widget),"umicom-automation-id");
    if(tag!=NULL && strcmp(tag,id)==0)return widget;
    for(GtkWidget *child=gtk_widget_get_first_child(widget);child!=NULL;child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,id);if(found!=NULL)return found;
    }
    return NULL;
}
static int Cancel(void *user)
{
    ++*(unsigned *)user;return 1;
}
typedef struct Notice {
    GtkWidget *panel;
    unsigned calls;
    int busy;
    UmiStatus status;
} Notice;
static void NoticeChanged(GObject *object,GParamSpec *property,gpointer data)
{
    (void)object;(void)property;
    Notice *notice=data;
    ++notice->calls;
    UmiLearningCatalogueGtkState state;
    notice->busy=UmiLearningCatalogueGtk4CopyState(notice->panel,&state)==UMI_STATUS_OK && state.busy;
    notice->status=UmiLearningCatalogueGtk4SearchBodies(notice->panel,"ownership",0,0,0);
}
static int Check(const char *name,const char *directory)
{
    int result=0,known=1;
    unsigned callbacks=0U;
    GtkWidget *panel=NULL;
    UmiLearningShelf *shelf=NULL;
    TextFixture fixture=TextFixtureDefault();
    CHECK(UmiLearningShelfCreate(&shelf)==UMI_STATUS_OK);
    CHECK(TextFixtureAdopt(shelf,directory,"filter.pack",&fixture)==UMI_STATUS_OK);
    panel=UmiLearningCatalogueGtk4Create((UmiVersion){0U,9U,0U});
    CHECK(panel!=NULL);g_object_ref_sink(panel);UmiTestExpandSections(panel);
    CHECK(UmiLearningCatalogueGtk4Load(panel,directory)==UMI_STATUS_OK && Finish());
    CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"ownership",0,0,0)==UMI_STATUS_OK && Finish());
    UmiLearningCatalogueBodySearchState before,state;
    CHECK(UmiLearningCatalogueGtk4CopyBodySearchState(panel,&before)==UMI_STATUS_OK);
    CHECK(strcmp(before.query.phrase,"ownership")==0 && before.query.maximum_results==128U);
    UmiLearningTextQuery query;UmiLearningTextQueryInit(&query);
    memcpy(query.phrase,"ownership",sizeof("ownership"));
    UmiStatus expected=UMI_STATUS_OK;
    size_t matches=3U,shown=3U;
    if(strncmp(name,"control-",8U)==0) {
        GtkWidget *phrase=Find(panel,"learning.catalogue.body-query");
        GtkWidget *product=Find(panel,"learning.catalogue.body-product");
        GtkWidget *locale=Find(panel,"learning.catalogue.body-locale");
        GtkWidget *kind=Find(panel,"learning.catalogue.body-kind");
        GtkWidget *button=Find(panel,"learning.catalogue.body-search");
        GtkWidget *status=Find(panel,"learning.catalogue.status");
        CHECK(phrase!=NULL && product!=NULL && locale!=NULL && kind!=NULL && button!=NULL && status!=NULL);
        gtk_editable_set_text(GTK_EDITABLE(phrase),"ownership");
        if(strcmp(name,"control-product")==0)gtk_editable_set_text(GTK_EDITABLE(product),"studio");
        else if(strcmp(name,"control-locale")==0)gtk_editable_set_text(GTK_EDITABLE(locale),"EN-gb");
        else if(strcmp(name,"control-kind")==0) {
            gtk_drop_down_set_selected(GTK_DROP_DOWN(kind),3U);matches=1U;
        } else if(strcmp(name,"control-reset")==0) {
            gtk_editable_set_text(GTK_EDITABLE(product),"missing");
            g_signal_emit_by_name(button,"clicked");
            CHECK(Finish() && UmiLearningCatalogueGtk4CopyBodySearchState(panel,&state)==UMI_STATUS_OK && state.results.matching_documents==0U);
            gtk_editable_set_text(GTK_EDITABLE(product),"");
        } else if(strcmp(name,"control-notice")==0 || strcmp(name,"control-utf8-capacity")==0) {
            if(strcmp(name,"control-notice")==0)gtk_editable_set_text(GTK_EDITABLE(phrase),"");
            else {
                char large[UMI_TEACHER_ID_CAPACITY*2U];
                for(size_t i=0U;i<sizeof(large)-2U;i+=2U) {large[i]=(char)0xc3;large[i+1U]=(char)0xa9;}
                large[sizeof(large)-2U]='\0';
                gtk_editable_set_text(GTK_EDITABLE(product),large);
            }
            Notice notice={panel,0U,0,UMI_STATUS_OK};
            gulong signal=g_signal_connect(status,"notify::label",G_CALLBACK(NoticeChanged),&notice);
            g_signal_emit_by_name(button,"clicked");
            g_signal_handler_disconnect(status,signal);
            CHECK(notice.calls>0U && notice.busy && notice.status==UMI_STATUS_BUSY);
            CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);
            CHECK(UmiLearningCatalogueGtk4CopyBodySearchState(panel,&state)==UMI_STATUS_OK);
            CHECK(state.results.matching_documents==3U && strcmp(state.query.phrase,"ownership")==0);
            goto cleanup;
        } else known=0;
        CHECK(known);g_signal_emit_by_name(button,"clicked");
        CHECK(Finish() && UmiLearningCatalogueGtk4CopyBodySearchState(panel,&state)==UMI_STATUS_OK);
        CHECK(state.results.matching_documents==matches);
        if(strcmp(name,"control-product")==0)CHECK(strcmp(state.query.product,"studio")==0);
        if(strcmp(name,"control-locale")==0)CHECK(strcmp(state.query.locale,"EN-gb")==0);
        if(strcmp(name,"control-kind")==0)CHECK(state.query.kind==UMI_LEARNING_RESOURCE_EXAMPLE);
        goto cleanup;
    }
    if(strcmp(name,"product")==0)memcpy(query.product,"studio",sizeof("studio"));
    else if(strcmp(name,"product-case")==0) {memcpy(query.product,"Studio",sizeof("Studio"));matches=shown=0U;}
    else if(strcmp(name,"locale")==0)memcpy(query.locale,"EN-gb",sizeof("EN-gb"));
    else if(strcmp(name,"locale-fallback")==0) {memcpy(query.locale,"en",sizeof("en"));matches=shown=0U;}
    else if(strcmp(name,"guide")==0) {query.kind=UMI_LEARNING_RESOURCE_GUIDE;matches=shown=1U;}
    else if(strcmp(name,"example")==0) {query.kind=UMI_LEARNING_RESOURCE_EXAMPLE;matches=shown=1U;}
    else if(strcmp(name,"lesson")==0) {query.kind=UMI_LEARNING_RESOURCE_LESSON;matches=shown=1U;}
    else if(strcmp(name,"image")==0) {query.kind=UMI_LEARNING_RESOURCE_IMAGE;matches=shown=0U;}
    else if(strcmp(name,"combined")==0) {
        memcpy(query.product,"studio",sizeof("studio"));memcpy(query.locale,"en-GB",sizeof("en-GB"));
        query.kind=UMI_LEARNING_RESOURCE_EXAMPLE;matches=shown=1U;
    } else if(strcmp(name,"limit")==0) {query.maximum_results=1U;shown=1U;}
    else if(strcmp(name,"maximum")==0)query.maximum_results=UMI_LEARNING_TEXT_RESULTS_MAX;
    else if(strcmp(name,"callback")==0) {
        query.cancellation=Cancel;query.cancellation_user=&callbacks;expected=UMI_STATUS_INVALID_ARGUMENT;
    } else if(strcmp(name,"callback-user")==0) {query.cancellation_user=&callbacks;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(name,"unterminated")==0) {memset(query.product,'x',sizeof(query.product));expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(strcmp(name,"invalid-kind")==0) {query.kind=99U;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(name,"zero-limit")==0) {query.maximum_results=0U;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(name,"empty")==0) {query.phrase[0]='\0';expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(strcmp(name,"null")==0 || strcmp(name,"refresh")==0)expected=UMI_STATUS_INVALID_ARGUMENT;
    else if(strcmp(name,"snapshot")!=0 && strcmp(name,"cached")!=0)known=0;
    CHECK(known);
    if(strcmp(name,"cached")==0)CHECK(UmiRootedFileRemove(directory,fixture.paths[1])==UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4SearchBodiesWithQuery(panel,strcmp(name,"null")==0 ? NULL : &query,
        strcmp(name,"refresh")==0 ? 2 : 0)==expected);
    if(strcmp(name,"snapshot")==0) {
        /* Mutation of caller storage after acceptance must not change the job
         * or the query receipt later published alongside its result owners. */
        memset(&query,0xff,sizeof(query));
    }
    if(expected==UMI_STATUS_OK) {
        CHECK(Finish() && UmiLearningCatalogueGtk4CopyBodySearchState(panel,&state)==UMI_STATUS_OK);
        CHECK(state.last_status==UMI_STATUS_OK && state.results.matching_documents==matches && state.results.result_count==shown);
        CHECK(state.results.truncated==(shown<matches));
        CHECK(strcmp(state.query.phrase,"ownership")==0 && state.query.cancellation==NULL && state.query.cancellation_user==NULL);
    } else {
        CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);
        CHECK(UmiLearningCatalogueGtk4CopyBodySearchState(panel,&state)==UMI_STATUS_OK);
        CHECK(state.results.matching_documents==before.results.matching_documents &&
            state.query.maximum_results==before.query.maximum_results && strcmp(state.query.phrase,before.query.phrase)==0);
    }
    CHECK(callbacks==0U);
cleanup:
    if(panel!=NULL)UmiLearningCatalogueGtk4Retire(panel);
    g_clear_object(&panel);
    if(!Finish())result=1;
    UmiLearningShelfDestroy(shelf);
    return result;
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    if(!gtk_init_check())return 77;
    return Check(argv[1],argv[2]);
}
