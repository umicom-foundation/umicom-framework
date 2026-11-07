/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_manifest_route_gtk4.c
 *
 * PURPOSE:
 *   Exercise named course loading, replacement and remembered routes through the native catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "manifest_route_fixture.h"
#include "umicom/ui/gtk4/learning_catalogue.h"
#include "../support/main_context.h"
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); result=1; goto cleanup; } } while(0)
static GtkWidget *Find(GtkWidget *widget, const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(widget),"umicom-automation-id");
    if(tag!=NULL && strcmp(tag,id)==0) return widget;
    for(GtkWidget *child=gtk_widget_get_first_child(widget); child!=NULL;
        child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,id);
        if(found!=NULL) return found;
    }
    return NULL;
}
/* Bound each dispatch slice as well as the whole wait: an always-ready GTK
 * source must not hide the deadline or turn a failing fixture into a hang. */
static int Finish(void)
{
    gint64 deadline=g_get_monotonic_time()+10*G_TIME_SPAN_SECOND;
    while(UmiLearningCatalogueGtk4PendingReads()!=0U && g_get_monotonic_time()<deadline) {
        (void)UmiTestDispatchSlice(NULL,deadline,64U);
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads()==0U;
}
int main(int argc, char **argv)
{
    if(argc!=3) return 2;
    if(!gtk_init_check()) return 77;
    int result=0;
    UmiVersion host={0U,9U,0U};
    GtkWidget *widget=UmiLearningCatalogueGtk4Create(host);
    if(widget==NULL) return 1;
    g_object_ref_sink(widget);
    UmiLearningCatalogueGtkState state;
    UmiLearningLibraryChoice choice;
    CHECK(RouteFixture(argv[2],"first.json","route.first",1U)==UMI_STATUS_OK);
    if(strcmp(argv[1],"invalid-route")==0) {
        CHECK(UmiLearningCatalogueGtk4LoadManifest(widget,argv[2],"../first.json")!=UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4PrepareReplacementManifest(widget,argv[2],"../first.json")!=UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4LoadManifest(widget,argv[2],NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4CopyState(widget,&state)==UMI_STATUS_OK);
        CHECK(!state.busy && state.pack_count==0U && UmiLearningCatalogueGtk4PendingReads()==0U);
        goto cleanup;
    }
    if(strcmp(argv[1],"load-button")==0) {
        GtkWidget *folder=Find(widget,"learning.catalogue.path");
        GtkWidget *manifest=Find(widget,"learning.catalogue.manifest");
        GtkWidget *load=Find(widget,"learning.catalogue.load");
        CHECK(folder!=NULL && manifest!=NULL && load!=NULL);
        gtk_editable_set_text(GTK_EDITABLE(folder),argv[2]);
        gtk_editable_set_text(GTK_EDITABLE(manifest),"first.json");
        g_signal_emit_by_name(load,"clicked");
        /* The accepted request owns the route; later typing cannot redirect it. */
        gtk_editable_set_text(GTK_EDITABLE(manifest),"absent.json");
    } else CHECK(UmiLearningCatalogueGtk4LoadManifest(widget,argv[2],"first.json")==UMI_STATUS_OK);
    if(strcmp(argv[1],"retire-pending")==0) {
        UmiLearningCatalogueGtk4Retire(widget);
        CHECK(Finish());
        CHECK(UmiLearningCatalogueGtk4CopyState(widget,&state)==UMI_STATUS_OK);
        CHECK(state.retired && state.pack_count==0U);
        goto cleanup;
    }
    CHECK(Finish());
    CHECK(UmiLearningCatalogueGtk4CopyState(widget,&state)==UMI_STATUS_OK);
    CHECK(state.last_status==UMI_STATUS_OK && state.pack_count==1U);
    CHECK(UmiLearningCatalogueGtk4Open(widget,0U)==UMI_STATUS_OK && Finish());
    CHECK(UmiLearningCatalogueGtk4RememberLibrary(widget)==UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4LibraryChoiceAt(widget,0U,&choice)==UMI_STATUS_OK);
    CHECK(strcmp(choice.manifest,"first.json")==0);
    if(strcmp(argv[1],"two-courses")==0) {
        CHECK(RouteFixture(argv[2],"second.json","route.second",1U)==UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4LoadManifest(widget,argv[2],"second.json")==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyState(widget,&state)==UMI_STATUS_OK);
        CHECK(state.last_status==UMI_STATUS_OK && state.pack_count==2U && state.search.result_count==2U);
    } else if(strcmp(argv[1],"reopen")==0) {
        char settings[UMI_PATH_CAPACITY];
        CHECK(umi_path_join(argv[2],"route-choices.json",settings,sizeof(settings))==UMI_STATUS_OK);
        UmiStatus removed=UmiRootedFileRemove(argv[2],"route-choices.json");
        CHECK(removed==UMI_STATUS_OK || removed==UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4SaveLibrariesNew(widget,settings)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyState(widget,&state)==UMI_STATUS_OK && state.last_status==UMI_STATUS_OK);
        UmiLearningCatalogueGtk4Retire(widget);
        g_object_unref(widget);
        widget=UmiLearningCatalogueGtk4Create(host);
        if(widget==NULL) return 1;
        g_object_ref_sink(widget);
        CHECK(UmiLearningCatalogueGtk4LoadLibraries(widget,settings)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4ReopenLibrary(widget,0U)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4Open(widget,0U)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyState(widget,&state)==UMI_STATUS_OK);
        CHECK(state.last_status==UMI_STATUS_OK && state.reader.has_document);
        CHECK(strcmp(state.reader.pack.id,"route.first")==0);
    } else if(strcmp(argv[1],"replacement")==0) {
        UmiLearningCatalogueReplacement review;
        CHECK(RouteFixture(argv[2],"replacement.json","route.first",2U)==UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4PrepareReplacementManifest(widget,argv[2],"replacement.json")==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyReplacement(widget,&review)==UMI_STATUS_OK);
        CHECK(review.proposed.revision==2U);
        CHECK(UmiLearningCatalogueGtk4AcceptReplacement(widget,review.review_serial)==UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4CopyState(widget,&state)==UMI_STATUS_OK);
        CHECK(state.reader.pack.revision==1U);
        CHECK(UmiLearningCatalogueGtk4ForgetLibrary(widget,0U)==UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4Open(widget,0U)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4RememberLibrary(widget)==UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4LibraryChoiceAt(widget,0U,&choice)==UMI_STATUS_OK);
        CHECK(strcmp(choice.manifest,"replacement.json")==0);
    } else CHECK(strcmp(argv[1],"load-button")==0);
cleanup:
    UmiLearningCatalogueGtk4Retire(widget);
    if(!Finish()) result=1;
    g_object_unref(widget);
    return result;
}
