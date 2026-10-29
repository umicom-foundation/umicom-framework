/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_presentation_failure_gtk4.c
 *
 * PURPOSE:
 *   Inject display allocation failures into the real native catalogue worker.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/learning_catalogue.h"
#include "umicom/platform/rooted_files.h"
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static atomic_int fail_buffer, fail_preview, failures;
static gpointer ControlledBuffer(gsize size)
{
    if(atomic_load(&fail_buffer)) { atomic_fetch_add(&failures,1); return NULL; }
    return g_try_malloc(size);
}
static gpointer ControlledPreview(gsize count,gsize size)
{
    if(atomic_load(&fail_preview)) { atomic_fetch_add(&failures,1); return NULL; }
    return g_try_malloc_n(count,size);
}
/* Structured HTML uses an independently allocated core model. Preserve the
 * same allocation-refusal contract when that model replaces the plain preview. */
static UmiStatus ControlledReading(const void *bytes, size_t size, UmiBrowserReadingDocument **out)
{
    if (atomic_load(&fail_preview)) {
        atomic_fetch_add(&failures, 1); *out = NULL; return UMI_STATUS_OUT_OF_MEMORY;
    }
    return UmiBrowserReadingDocumentCreate(bytes, size, out);
}
/* Compile the actual worker in this test object with two narrow allocation
 * substitutions. The linked archive supplies the other catalogue objects;
 * its worker object is not needed. No production failure switch is introduced. */
#define UmiBrowserReadingDocumentCreate ControlledReading
#define g_try_malloc ControlledBuffer
#undef g_try_new
#define g_try_new(type,count) ((type *)ControlledPreview((count),sizeof(type)))
#include "../../adapters/gtk4/learning_catalogue_jobs.c"
#undef g_try_new
#undef g_try_malloc
#undef UmiBrowserReadingDocumentCreate

#define CHECK(x) do { if(!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); \
    failed=1;goto cleanup; } } while(0)
static int Drain(void)
{
    gint64 end=g_get_monotonic_time()+15*G_TIME_SPAN_SECOND;
    while(UmiLearningCatalogueGtk4PendingReads()!=0U && g_get_monotonic_time()<end) {
        while(g_main_context_iteration(NULL,FALSE)){}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads()==0U;
}
static GtkWidget *Find(GtkWidget *widget,const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(widget),"umicom-automation-id");
    if(tag!=NULL && strcmp(tag,id)==0)return widget;
    for(GtkWidget *child=gtk_widget_get_first_child(widget);child!=NULL;child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,id); if(found!=NULL)return found;
    }
    return NULL;
}
static char *Content(GtkWidget *panel)
{
    GtkWidget *view=Find(panel,"learning.catalogue.content");
    if(!GTK_IS_TEXT_VIEW(view))return NULL;
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter first,last;gtk_text_buffer_get_bounds(buffer,&first,&last);
    return gtk_text_buffer_get_text(buffer,&first,&last,FALSE);
}
static int Fixture(const char *directory)
{
    static const char a[]="<h1>First lesson</h1><p>Original reader.</p>";
    static const char b[]="<h1>Second lesson</h1><p>Candidate reader.</p>";
    char ah[UMI_SHA256_HEX_CAPACITY],bh[UMI_SHA256_HEX_CAPACITY],json[1800];
    if(UmiSha256Buffer(a,sizeof(a)-1U,ah)!=UMI_STATUS_OK ||
        UmiSha256Buffer(b,sizeof(b)-1U,bh)!=UMI_STATUS_OK)return 0;
    int n=snprintf(json,sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"preview.fixture\","
        "\"title\":\"Preview transaction\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"first\",\"title\":\"First\",\"topic\":\"ownership\",\"kind\":\"guide\","
        "\"path\":\"first.html\",\"sha256\":\"%s\"},"
        "{\"id\":\"second\",\"title\":\"Second\",\"topic\":\"ownership\",\"kind\":\"lesson\","
        "\"path\":\"second.html\",\"sha256\":\"%s\"}]}",ah,bh);
    return n>0 && (size_t)n<sizeof(json) &&
        UmiRootedFileWrite(directory,"first.html",a,sizeof(a)-1U)==UMI_STATUS_OK &&
        UmiRootedFileWrite(directory,"second.html",b,sizeof(b)-1U)==UMI_STATUS_OK &&
        UmiRootedFileWrite(directory,"learning-pack.json",json,(size_t)n)==UMI_STATUS_OK;
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    if(!gtk_init_check())return 77;
    int failed=0;
    GtkWidget *panel=NULL;
    char *before_text=NULL,*after_text=NULL;
    UmiLearningCatalogueGtkState before,after;
    CHECK(Fixture(argv[2]));
    panel=UmiLearningCatalogueGtk4Create((UmiVersion){0U,9U,0U});
    CHECK(panel!=NULL);g_object_ref_sink(panel);
    CHECK(UmiLearningCatalogueGtk4Load(panel,argv[2])==UMI_STATUS_OK && Drain());
    if(strcmp(argv[1],"initial-preview")!=0) {
        CHECK(UmiLearningCatalogueGtk4Open(panel,0U)==UMI_STATUS_OK && Drain());
        if(strcmp(argv[1],"back-preview")==0)
            CHECK(UmiLearningCatalogueGtk4Open(panel,1U)==UMI_STATUS_OK && Drain());
    }
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&before)==UMI_STATUS_OK && before.last_status==UMI_STATUS_OK);
    /* Successful setup must have traversed the structured preparation path,
     * not only the legacy text preview that can mask a missing integration. */
    if (before.reader.has_document) {
        UmiBrowserReadingSummary structure;
        CHECK(before.has_reading_structure);
        CHECK(UmiLearningCatalogueGtk4CopyReadingSummary(panel,&structure)==UMI_STATUS_OK);
        CHECK(structure.spans>0U);
    }
    before_text=Content(panel);CHECK(before_text!=NULL);
    atomic_store(&fail_buffer,strcmp(argv[1],"follow-buffer")==0);
    atomic_store(&fail_preview,strcmp(argv[1],"follow-buffer")!=0);
    if(strcmp(argv[1],"initial-preview")==0 || strcmp(argv[1],"open-preview")==0)
        CHECK(UmiLearningCatalogueGtk4Open(panel,1U)==UMI_STATUS_OK);
    else if(strcmp(argv[1],"back-preview")==0)
        CHECK(UmiLearningCatalogueGtk4Back(panel)==UMI_STATUS_OK);
    else if(strcmp(argv[1],"reload-preview")==0)
        CHECK(UmiLearningCatalogueGtk4Reload(panel)==UMI_STATUS_OK);
    else {
        CHECK(strcmp(argv[1],"follow-buffer")==0 || strcmp(argv[1],"follow-preview")==0);
        CHECK(UmiLearningCatalogueGtk4Follow(panel,"second.html")==UMI_STATUS_OK);
    }
    CHECK(Drain() && atomic_load(&failures)==1);
    atomic_store(&fail_buffer,0);atomic_store(&fail_preview,0);
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK);
    CHECK(after.last_status==UMI_STATUS_OUT_OF_MEMORY && !after.busy && !after.retired);
    CHECK(after.reader.has_document==before.reader.has_document &&
        after.reader.revision==before.reader.revision &&
        after.reader.history_count==before.reader.history_count &&
        after.reader.history_index==before.reader.history_index &&
        strcmp(after.reader.selection.resource.id,before.reader.selection.resource.id)==0);
    after_text=Content(panel);CHECK(after_text!=NULL && strcmp(before_text,after_text)==0);
    /* A real retry must still have a usable reader after the failed candidate
     * was released. Check this through the same public navigation interface. */
    CHECK(UmiLearningCatalogueGtk4Open(panel,1U)==UMI_STATUS_OK && Drain());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK);
    CHECK(after.last_status==UMI_STATUS_OK && strcmp(after.reader.selection.resource.id,"second")==0);
cleanup:
    if(panel!=NULL)UmiLearningCatalogueGtk4Retire(panel);
    if(!Drain())failed=1;
    atomic_store(&fail_buffer,0);atomic_store(&fail_preview,0);
    g_clear_object(&panel);g_free(before_text);g_free(after_text);
    return failed;
}
