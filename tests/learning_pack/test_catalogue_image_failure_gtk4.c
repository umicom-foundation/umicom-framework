/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_image_failure_gtk4.c
 *
 * PURPOSE:
 *   Verify illustration allocation refusals preserve the admitted reader and visible content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/learning_catalogue.h"
#include "umicom/media/png_image.h"
#include "umicom/platform/rooted_files.h"
#include "../media/png_fixture.h"
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

static atomic_int fail_pixels,fail_row,fail_decode,failures;
static gpointer PixelAllocation(gsize size)
{
    if(atomic_load(&fail_pixels)) { atomic_fetch_add(&failures,1);return NULL; }
    return g_try_malloc(size);
}
static gpointer RowAllocation(gsize count,gsize size)
{
    if(atomic_load(&fail_row)) { atomic_fetch_add(&failures,1);return NULL; }
    return g_try_malloc_n(count,size);
}
static int AvailableDecoder(void) { return 1; }
static UmiStatus DecodeFixture(const void *bytes,size_t size,
    const UmiCancellationToken *cancel,UmiMediaImageSurface **out)
{
    (void)bytes;(void)size;(void)cancel;
    if(atomic_load(&fail_decode)) { atomic_fetch_add(&failures,1);return UMI_STATUS_OUT_OF_MEMORY; }
    /* This deterministic provider seam exercises adapter ownership even when
     * host libpng is absent. Separate native_images cases use the real codec. */
    return umi_media_image_surface_create(2U,2U,out);
}
#define UMICOM_LEARNING_PNG 1
#define UmiMediaPngAvailable AvailableDecoder
#define UmiMediaPngDecode DecodeFixture
#define g_try_malloc PixelAllocation
#undef g_try_new
#define g_try_new(type,count) ((type *)RowAllocation((count),sizeof(type)))
#include "../../adapters/gtk4/learning_catalogue_images.c"
#undef g_try_new
#undef g_try_malloc
#undef UmiMediaPngDecode
#undef UmiMediaPngAvailable
#undef UMICOM_LEARNING_PNG

#define CHECK(x) do { if(!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); \
    failed=1;goto cleanup; } } while(0)
static int Drain(void)
{
    gint64 until=g_get_monotonic_time()+15*G_TIME_SPAN_SECOND;
    while(UmiLearningCatalogueGtk4PendingReads()!=0U && g_get_monotonic_time()<until) {
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
static char *DisplayedText(GtkWidget *panel)
{
    GtkWidget *view=Find(panel,"learning.catalogue.content");
    if(!GTK_IS_TEXT_VIEW(view))return NULL;
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter first,last;gtk_text_buffer_get_bounds(buffer,&first,&last);
    return gtk_text_buffer_get_text(buffer,&first,&last,FALSE);
}
static int Fixture(const char *directory)
{
    TestPngFixture image=PngFixture("rgba");
    const char guide[]="<h1>Keep this guide</h1><p>Reader ownership.</p>";
    char gh[UMI_SHA256_HEX_CAPACITY],ih[UMI_SHA256_HEX_CAPACITY],json[2048];
    if(UmiSha256Buffer(guide,sizeof(guide)-1U,gh)!=UMI_STATUS_OK ||
        UmiSha256Buffer(image.bytes,image.size,ih)!=UMI_STATUS_OK)return 0;
    int n=snprintf(json,sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"image.failure\","
        "\"title\":\"Image transaction\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"guide\",\"title\":\"Keep this guide\",\"topic\":\"reading\",\"kind\":\"guide\","
        "\"path\":\"guide.html\",\"sha256\":\"%s\"},"
        "{\"id\":\"image\",\"title\":\"Pixel ownership\",\"topic\":\"illustration\",\"kind\":\"image\","
        "\"path\":\"image.png\",\"sha256\":\"%s\"}]}",gh,ih);
    return n>0 && (size_t)n<sizeof(json) &&
        UmiRootedFileWrite(directory,"guide.html",guide,sizeof(guide)-1U)==UMI_STATUS_OK &&
        UmiRootedFileWrite(directory,"image.png",image.bytes,image.size)==UMI_STATUS_OK &&
        UmiRootedFileWrite(directory,"learning-pack.json",json,(size_t)n)==UMI_STATUS_OK;
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    const char *mode=argv[1];
    const char *known[]={"pixels","row","decode","initial","reload","back"};
    int accepted=0;
    for(size_t i=0U;i<sizeof(known)/sizeof(known[0]);++i)if(strcmp(mode,known[i])==0)accepted=1;
    if(!accepted)return 2;
    if(!gtk_init_check())return 77;
    int failed=0;
    GtkWidget *panel=NULL,*picture=NULL;
    GdkPaintable *saved_pixels=NULL;
    char *before_text=NULL,*after_text=NULL;
    UmiLearningCatalogueGtkState before,after;
    UmiLearningCatalogueIllustrationState image_before={0},image_after={0};
    CHECK(Fixture(argv[2]));
    panel=UmiLearningCatalogueGtk4Create((UmiVersion){0U,9U,0U});
    CHECK(panel!=NULL);g_object_ref_sink(panel);
    CHECK(UmiLearningCatalogueGtk4Load(panel,argv[2])==UMI_STATUS_OK && Drain());
    picture=Find(panel,"learning.catalogue.illustration");CHECK(GTK_IS_PICTURE(picture));
    if(strcmp(mode,"initial")!=0) {
        if(strcmp(mode,"reload")==0 || strcmp(mode,"back")==0) {
            CHECK(UmiLearningCatalogueGtk4Search(panel,"illustration")==UMI_STATUS_OK);
            CHECK(UmiLearningCatalogueGtk4Open(panel,0U)==UMI_STATUS_OK && Drain());
            CHECK(UmiLearningCatalogueGtk4CopyIllustration(panel,&image_before)==UMI_STATUS_OK && image_before.has_pixels);
            if(strcmp(mode,"back")==0)
                CHECK(UmiLearningCatalogueGtk4Follow(panel,"guide.html")==UMI_STATUS_OK && Drain());
        } else {
            CHECK(UmiLearningCatalogueGtk4Search(panel,"reading")==UMI_STATUS_OK);
            CHECK(UmiLearningCatalogueGtk4Open(panel,0U)==UMI_STATUS_OK && Drain());
        }
    }
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&before)==UMI_STATUS_OK && before.last_status==UMI_STATUS_OK);
    before_text=DisplayedText(panel);CHECK(before_text!=NULL);
    saved_pixels=gtk_picture_get_paintable(GTK_PICTURE(picture));
    if(saved_pixels!=NULL)g_object_ref(saved_pixels);
    atomic_store(&fail_decode,strcmp(mode,"decode")==0);
    atomic_store(&fail_row,strcmp(mode,"row")==0);
    atomic_store(&fail_pixels,strcmp(mode,"decode")!=0 && strcmp(mode,"row")!=0);
    if(strcmp(mode,"initial")==0) {
        CHECK(UmiLearningCatalogueGtk4Search(panel,"illustration")==UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4Open(panel,0U)==UMI_STATUS_OK);
    } else if(strcmp(mode,"reload")==0)CHECK(UmiLearningCatalogueGtk4Reload(panel)==UMI_STATUS_OK);
    else if(strcmp(mode,"back")==0)CHECK(UmiLearningCatalogueGtk4Back(panel)==UMI_STATUS_OK);
    else CHECK(UmiLearningCatalogueGtk4Follow(panel,"image.png")==UMI_STATUS_OK);
    CHECK(Drain() && atomic_load(&failures)==1);
    atomic_store(&fail_pixels,0);atomic_store(&fail_row,0);atomic_store(&fail_decode,0);
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK);
    CHECK(after.last_status==UMI_STATUS_OUT_OF_MEMORY && !after.busy);
    CHECK(after.reader.has_document==before.reader.has_document &&
        after.reader.revision==before.reader.revision &&
        after.reader.history_count==before.reader.history_count &&
        after.reader.history_index==before.reader.history_index &&
        strcmp(after.reader.selection.resource.id,before.reader.selection.resource.id)==0);
    after_text=DisplayedText(panel);CHECK(after_text!=NULL && strcmp(before_text,after_text)==0);
    CHECK(gtk_picture_get_paintable(GTK_PICTURE(picture))==saved_pixels);
    if(strcmp(mode,"reload")==0) {
        CHECK(UmiLearningCatalogueGtk4CopyIllustration(panel,&image_after)==UMI_STATUS_OK);
        CHECK(image_after.status==image_before.status && image_after.width==image_before.width &&
            image_after.height==image_before.height && image_after.has_pixels==image_before.has_pixels);
    }
    /* The candidate's failure must not poison the worker or retained reader. */
    CHECK(UmiLearningCatalogueGtk4Search(panel,"illustration")==UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4Open(panel,0U)==UMI_STATUS_OK && Drain());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after)==UMI_STATUS_OK && after.last_status==UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4CopyIllustration(panel,&image_after)==UMI_STATUS_OK && image_after.has_pixels);
cleanup:
    if(panel!=NULL)UmiLearningCatalogueGtk4Retire(panel);
    if(!Drain())failed=1;
    atomic_store(&fail_pixels,0);atomic_store(&fail_row,0);atomic_store(&fail_decode,0);
    g_clear_object(&saved_pixels);g_clear_object(&panel);g_free(before_text);g_free(after_text);
    return failed;
}
