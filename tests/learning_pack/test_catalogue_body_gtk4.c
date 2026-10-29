/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_body_gtk4.c
 *
 * PURPOSE:
 *   Verify native body-search caching, stale-result refusal and asynchronous ownership.
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

#define CHECK(x) do { if(!(x)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);result=1;goto cleanup;} } while(0)
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
typedef struct Observer {
    GtkWidget *panel;
    size_t calls;
    UmiStatus reentrant;
    int saw_results, retire;
} Observer;
static void Published(GObject *object,GParamSpec *spec,gpointer data)
{
    (void)object;(void)spec;
    Observer *observer=data;
    ++observer->calls;
    UmiLearningCatalogueBodySearchState state;
    observer->saw_results=UmiLearningCatalogueGtk4CopyBodySearchState(observer->panel,&state)==UMI_STATUS_OK &&
        state.has_results && state.results_current && state.results.matching_documents==3U;
    observer->reentrant=UmiLearningCatalogueGtk4SearchBodies(observer->panel,"ownership",0,0,0);
    if(observer->retire)UmiLearningCatalogueGtk4Retire(observer->panel);
}
static void RetireOnLabel(GObject *object,GParamSpec *spec,gpointer data)
{
    (void)object;(void)spec;
    UmiLearningCatalogueGtk4Retire(GTK_WIDGET(data));
}
static int Check(const char *name,const char *directory)
{
    int result=0,known=1;
    GtkWidget *panel=NULL,*retained=NULL;
    UmiLearningShelf *fixture_shelf=NULL;
    TextFixture fixture=TextFixtureDefault();
    Observer observer={0};
    CHECK(UmiLearningShelfCreate(&fixture_shelf)==UMI_STATUS_OK);
    CHECK(TextFixtureAdopt(fixture_shelf,directory,"text.pack",&fixture)==UMI_STATUS_OK);
    panel=UmiLearningCatalogueGtk4Create((UmiVersion){0U,9U,0U});
    CHECK(panel!=NULL);g_object_ref_sink(panel);UmiTestExpandSections(panel);
    UmiLearningCatalogueBodySearchState state;
    CHECK(UmiLearningCatalogueGtk4CopyBodySearchState(panel,&state)==UMI_STATUS_OK && !state.has_index && !state.has_results);
    if(strcmp(name,"empty-shelf")!=0)CHECK(UmiLearningCatalogueGtk4Load(panel,directory)==UMI_STATUS_OK && Finish());
    if(strcmp(name,"controls")==0) {
        const char *ids[]={"body-query","body-case","body-word","body-search","body-refresh","body-results","body-read","body-status"};
        for(size_t i=0U;i<sizeof(ids)/sizeof(ids[0]);++i) {
            char id[96];snprintf(id,sizeof(id),"learning.catalogue.%s",ids[i]);CHECK(Find(panel,id)!=NULL);
        }
        goto cleanup;
    }
    if(strcmp(name,"arguments")==0) {
        CHECK(UmiLearningCatalogueGtk4SearchBodies(NULL,"x",0,0,0)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,NULL,0,0,0)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"",0,0,0)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"x",2,0,0)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"x",0,-1,0)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"x",0,0,2)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"x\n",0,0,0)==UMI_STATUS_INVALID_ARGUMENT);
        char invalid[]={(char)0xff,0};
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,invalid,0,0,0)==UMI_STATUS_INVALID_ARGUMENT);
        char long_phrase[257];memset(long_phrase,'x',256U);long_phrase[256U]='\0';
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,long_phrase,0,0,0)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiLearningCatalogueGtk4CopyBodySearchState(panel,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4CopyBodySearchState(NULL,&state)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4ReadBodySearchResult(panel,0U)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4BodySearchResultAt(panel,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);
        goto cleanup;
    }
    if(strcmp(name,"retired")==0) {
        UmiLearningCatalogueGtk4Retire(panel);
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"ownership",0,0,0)==UMI_STATUS_INVALID_STATE);
        goto cleanup;
    }
    if(strcmp(name,"reentrant")==0 || strcmp(name,"retire-publication")==0) {
        observer.panel=panel;observer.retire=strcmp(name,"retire-publication")==0;
        g_signal_connect(Find(panel,"learning.catalogue.body-results"),"notify::model",G_CALLBACK(Published),&observer);
    }
    if(strcmp(name,"buttons")==0) {
        gtk_editable_set_text(GTK_EDITABLE(Find(panel,"learning.catalogue.body-query")),"ownership");
        g_signal_emit_by_name(Find(panel,"learning.catalogue.body-search"),"clicked");
    } else CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"ownership",0,0,0)==UMI_STATUS_OK);
    if(strcmp(name,"busy")==0) {
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"ownership",0,0,0)==UMI_STATUS_BUSY);
        CHECK(UmiLearningCatalogueGtk4ReadBodySearchResult(panel,0U)==UMI_STATUS_BUSY);
    }
    if(strcmp(name,"retire-pending")==0 || strcmp(name,"destroy-pending")==0) {
        UmiLearningCatalogueGtk4Retire(panel);
        if(strcmp(name,"destroy-pending")==0)g_clear_object(&panel);
        CHECK(Finish());
        goto cleanup;
    }
    CHECK(Finish());
    UmiLearningCatalogueGtkState main_state;
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&main_state)==UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4CopyBodySearchState(panel,&state)==UMI_STATUS_OK);
    CHECK(state.has_index && state.has_results && state.index_current && state.results_current);
    CHECK(state.last_status==UMI_STATUS_OK && state.results.matching_documents==(strcmp(name,"empty-shelf")==0 ? 0U : 3U));
    if(strcmp(name,"empty-shelf")==0)goto cleanup;
    if(strcmp(name,"reentrant")==0 || strcmp(name,"retire-publication")==0) {
        CHECK(observer.calls>0U && observer.saw_results && observer.reentrant==UMI_STATUS_BUSY);
        goto cleanup;
    }
    if(strcmp(name,"cached-retire-pending")==0 || strcmp(name,"cached-destroy-pending")==0) {
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"ownership",0,0,0)==UMI_STATUS_OK);
        UmiLearningCatalogueGtk4Retire(panel);
        if(strcmp(name,"cached-destroy-pending")==0)g_clear_object(&panel);
        CHECK(Finish());
        goto cleanup;
    }
    if(strcmp(name,"retire-dispatch")==0) {
        GtkWidget *status=Find(panel,"learning.catalogue.status");CHECK(status!=NULL);
        g_signal_connect(status,"notify::label",G_CALLBACK(RetireOnLabel),panel);
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"ownership",0,0,0)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);
        goto cleanup;
    }
    if(strcmp(name,"retained-button")==0) {
        GtkWidget *button=Find(panel,"learning.catalogue.body-search");
        CHECK(button!=NULL);
        retained=g_object_ref(button);
        UmiLearningCatalogueGtk4Retire(panel);g_clear_object(&panel);
        g_signal_emit_by_name(retained,"clicked");
        CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);
        goto cleanup;
    }
    if(strcmp(name,"cached")==0 || strcmp(name,"refresh-missing")==0 || strcmp(name,"refresh-changed")==0) {
        if(strcmp(name,"refresh-changed")==0)CHECK(UmiRootedFileWrite(directory,fixture.paths[1],"changed",7U)==UMI_STATUS_OK);
        else CHECK(UmiRootedFileRemove(directory,fixture.paths[1])==UMI_STATUS_OK);
        int refresh=strcmp(name,"cached")!=0;
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"ownership",0,0,refresh)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyBodySearchState(panel,&state)==UMI_STATUS_OK);
        CHECK(state.has_index && state.has_results && state.results.matching_documents==3U);
        CHECK(state.last_status==(!refresh ? UMI_STATUS_OK : strcmp(name,"refresh-changed")==0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_NOT_FOUND));
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"ownership",0,0,0)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyBodySearchState(panel,&state)==UMI_STATUS_OK && state.last_status==UMI_STATUS_OK);
    } else if(strcmp(name,"open")==0 || strcmp(name,"open-changed")==0 || strcmp(name,"buttons")==0) {
        if(strcmp(name,"buttons")==0)g_signal_emit_by_name(Find(panel,"learning.catalogue.body-read"),"clicked");
        else CHECK(UmiLearningCatalogueGtk4ReadBodySearchResult(panel,0U)==UMI_STATUS_OK);
        CHECK(Finish() && UmiLearningCatalogueGtk4CopyState(panel,&main_state)==UMI_STATUS_OK);
        CHECK(main_state.reader.has_document && strcmp(main_state.reader.selection.resource.id,"guide")==0);
        if(strcmp(name,"open-changed")==0) {
            CHECK(UmiRootedFileWrite(directory,fixture.paths[1],"changed",7U)==UMI_STATUS_OK);
            CHECK(UmiLearningCatalogueGtk4ReadBodySearchResult(panel,1U)==UMI_STATUS_OK && Finish());
            CHECK(UmiLearningCatalogueGtk4CopyState(panel,&main_state)==UMI_STATUS_OK);
            CHECK(main_state.last_status==UMI_STATUS_INVALID_STATE && strcmp(main_state.reader.selection.resource.id,"guide")==0);
        }
    } else if(strcmp(name,"shelf-changed")==0) {
        CHECK(TextFixtureAdopt(fixture_shelf,directory,"other.pack",&fixture)==UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4Load(panel,directory)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyBodySearchState(panel,&state)==UMI_STATUS_OK && !state.index_current && !state.results_current);
        CHECK(UmiLearningCatalogueGtk4ReadBodySearchResult(panel,0U)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,"ownership",0,0,0)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyBodySearchState(panel,&state)==UMI_STATUS_OK && state.results_current && state.results.matching_documents==6U);
    } else if(strcmp(name,"case")==0 || strcmp(name,"whole-word")==0 || strcmp(name,"missing")==0) {
        const char *phrase=strcmp(name,"whole-word")==0 ? "own" : strcmp(name,"missing")==0 ? "absentword" : "ownership";
        CHECK(UmiLearningCatalogueGtk4SearchBodies(panel,phrase,strcmp(name,"case")==0,strcmp(name,"whole-word")==0,0)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyBodySearchState(panel,&state)==UMI_STATUS_OK);
        CHECK(state.results.matching_documents==(strcmp(name,"case")==0 ? 2U : 0U));
    } else if(strcmp(name,"outputs")==0) {
        UmiLearningTextHit hit,previous;memset(&hit,0x5a,sizeof(hit));memcpy(&previous,&hit,sizeof(hit));
        CHECK(UmiLearningCatalogueGtk4BodySearchResultAt(panel,99U,&hit)==UMI_STATUS_NOT_FOUND && memcmp(&hit,&previous,sizeof(hit))==0);
        CHECK(UmiLearningCatalogueGtk4BodySearchResultAt(panel,1U,&hit)==UMI_STATUS_OK && strcmp(hit.resource.id,"source")==0);
        CHECK(UmiLearningCatalogueGtk4ReadBodySearchResult(panel,99U)==UMI_STATUS_NOT_FOUND);
    } else if(strcmp(name,"default")!=0 && strcmp(name,"busy")!=0)known=0;
    CHECK(known);
cleanup:
    if(panel!=NULL)UmiLearningCatalogueGtk4Retire(panel);
    g_clear_object(&panel);
    if(!Finish())result=1;
    g_clear_object(&retained);
    UmiLearningShelfDestroy(fixture_shelf);
    return result;
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    if(!gtk_init_check())return 77;
    return Check(argv[1],argv[2]);
}
