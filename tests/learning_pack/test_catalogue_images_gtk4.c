/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_images_gtk4.c
 *
 * PURPOSE:
 *   Verify optional PNG ownership, verified resource history and native illustration publication.
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
#include "../media/png_fixture.h"
#include "../../adapters/gtk4/learning_catalogue_private.h"
#include <stdio.h>
#include <string.h>
#define CHECK(expression) do { if(!(expression)) { \
    fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#expression);failed=1;goto cleanup; \
} } while(0)
static int Finish(void)
{
    gint64 end=g_get_monotonic_time()+15*G_TIME_SPAN_SECOND;
    while(UmiLearningCatalogueGtk4PendingReads()!=0U && g_get_monotonic_time()<end) {
        while(g_main_context_iteration(NULL,FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads()==0U;
}
static GtkWidget *Find(GtkWidget *root,const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if(tag!=NULL && strcmp(tag,id)==0) return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,id);if(found!=NULL)return found;
    }
    return NULL;
}
static int Fixture(const char *root,const unsigned char *bytes,size_t size)
{
    const char *guide="<h1>Image guide</h1><p><a href='image.png'>Open the illustration</a></p>";
    char image_hash[UMI_SHA256_HEX_CAPACITY],guide_hash[UMI_SHA256_HEX_CAPACITY],json[2048];
    if(UmiSha256Buffer(bytes,size,image_hash)!=UMI_STATUS_OK ||
       UmiSha256Buffer(guide,strlen(guide),guide_hash)!=UMI_STATUS_OK) return 0;
    int count=snprintf(json,sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"image.fixture\","
        "\"title\":\"Illustration fixture\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"image\",\"title\":\"Four colour squares\",\"topic\":\"illustration\",\"kind\":\"image\","
        "\"path\":\"image.png\",\"sha256\":\"%s\"},"
        "{\"id\":\"guide\",\"title\":\"Image guide\",\"topic\":\"reading\",\"kind\":\"guide\","
        "\"path\":\"guide.html\",\"sha256\":\"%s\"}]}",image_hash,guide_hash);
    return count>0 && (size_t)count<sizeof(json) &&
        UmiRootedFileWrite(root,"image.png",bytes,size)==UMI_STATUS_OK &&
        UmiRootedFileWrite(root,"guide.html",guide,strlen(guide))==UMI_STATUS_OK &&
        UmiRootedFileWrite(root,"learning-pack.json",json,(size_t)count)==UMI_STATUS_OK;
}
typedef struct Observer { GtkWidget *root;unsigned calls;UmiStatus status,mutation; } Observer;
static void PaintableChanged(GObject *object,GParamSpec *property,gpointer data)
{
    (void)object;(void)property;
    Observer *observer=data;++observer->calls;
    UmiLearningCatalogueIllustrationState state;
    observer->status=UmiLearningCatalogueGtk4CopyIllustration(observer->root,&state);
    observer->mutation=UmiLearningCatalogueGtk4SetReadingZoom(observer->root,120U);
    UmiLearningCatalogueGtk4Retire(observer->root);
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    if(!gtk_init_check())return 77;
    const char *mode=argv[1];
    const char *known[]={"image","rgba-bytes","rgb","changed","history","linked-image",
        "bad-crc","oversize","unsupported","retire-pending","destroy-pending","retire-publication","arguments"};
    int matched=0;for(size_t i=0U;i<sizeof(known)/sizeof(known[0]);++i)if(strcmp(mode,known[i])==0)matched=1;
    if(!matched)return 2;
    int failed=0,available=UmiLearningCatalogueGtk4PngAvailable();
    /* Fallback coverage must not be reported as a decoder/paintable pass. */
    int needs_decoder=strcmp(mode,"rgba-bytes")==0 || strcmp(mode,"rgb")==0 ||
        strcmp(mode,"bad-crc")==0 || strcmp(mode,"oversize")==0 || strcmp(mode,"retire-publication")==0;
    if(!available && needs_decoder) {
        fprintf(stderr,"PNG decoder unavailable; this codec/publication case was not exercised.\n");
        return 77;
    }
    printf("PNG decoder: %s; %s\n",available?"available":"unavailable",
        available?"real codec path selected":"verified-resource fallback path selected");
    GtkWidget *panel=NULL,*picture=NULL;
    GdkPaintable *retained=NULL;
    gulong signal=0U;
    LearningCatalogueJob direct={0};
    UmiLearningCatalogueGtkState state,before;
    UmiLearningCatalogueIllustrationState image;
    TestPngFixture fixture=PngFixture(strcmp(mode,"rgb")==0?"rgb":"rgba");
    unsigned char *bytes=g_memdup2(fixture.bytes,fixture.size);
    size_t size=fixture.size;
    CHECK(bytes!=NULL);
    if(strcmp(mode,"bad-crc")==0)bytes[29U]^=1U;
    if(strcmp(mode,"oversize")==0) { bytes[16U]=0U;bytes[17U]=0U;bytes[18U]=0x10U;bytes[19U]=0U; }
    if(strcmp(mode,"unsupported")==0) { memset(bytes,'x',size); }
    if(strcmp(mode,"rgba-bytes")==0) {
        LearningCataloguePrepareImage(&direct,bytes,size);
        CHECK(direct.status==UMI_STATUS_OK);
        CHECK(direct.image_status==(available?UMI_STATUS_OK:UMI_STATUS_NOT_IMPLEMENTED));
        if(available) {
            gsize count=0U;const unsigned char *pixels=g_bytes_get_data(direct.image_pixels,&count);
            CHECK(count==16U && direct.image_width==2U && direct.image_height==2U);
            CHECK(pixels[0]==255U && pixels[1]==0U && pixels[2]==0U && pixels[3]==255U);
            CHECK(pixels[4]==0U && pixels[5]==255U && pixels[6]==0U && pixels[7]==128U);
        } else CHECK(direct.image_pixels==NULL);
        goto cleanup;
    }
    CHECK(Fixture(argv[2],bytes,size));
    panel=UmiLearningCatalogueGtk4Create((UmiVersion){0,9,0});CHECK(panel!=NULL);g_object_ref_sink(panel);
    picture=Find(panel,"learning.catalogue.illustration");CHECK(GTK_IS_PICTURE(picture));
    GtkWidget *viewport=Find(panel,"learning.catalogue.illustration-scroll");
    CHECK(GTK_IS_SCROLLED_WINDOW(viewport));
    CHECK(gtk_scrolled_window_get_max_content_height(GTK_SCROLLED_WINDOW(viewport))==280);
    CHECK(!gtk_widget_get_visible(viewport));
    if(strcmp(mode,"arguments")==0) {
        memset(&image,0x47,sizeof(image));UmiLearningCatalogueIllustrationState saved=image;
        CHECK(UmiLearningCatalogueGtk4CopyIllustration(NULL,&image)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4CopyIllustration(panel,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4CopyIllustration(panel,&image)==UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&image,&saved,sizeof(image))==0);
        goto cleanup;
    }
    CHECK(UmiLearningCatalogueGtk4Load(panel,argv[2])==UMI_STATUS_OK && Finish());
    if(strcmp(mode,"linked-image")==0) {
        CHECK(UmiLearningCatalogueGtk4Search(panel,"guide")==UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4Open(panel,0U)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4OpenReadingLink(panel,0U)==UMI_STATUS_OK);
    } else {
        CHECK(UmiLearningCatalogueGtk4Search(panel,"illustration")==UMI_STATUS_OK);
        size_t selected=0U; /* Only the image has this exact fixture topic. */
        Observer observer={panel,0U,UMI_STATUS_NOT_FOUND,UMI_STATUS_OK};
        if(strcmp(mode,"retire-publication")==0 && available)
            signal=g_signal_connect(picture,"notify::paintable",G_CALLBACK(PaintableChanged),&observer);
        CHECK(UmiLearningCatalogueGtk4Open(panel,selected)==UMI_STATUS_OK);
        if(strcmp(mode,"destroy-pending")==0) {
            UmiLearningCatalogueGtk4Retire(panel);g_clear_object(&panel);picture=NULL;
            CHECK(Finish());goto cleanup;
        }
        if(strcmp(mode,"retire-pending")==0 || (strcmp(mode,"retire-publication")==0 && !available)) {
            UmiLearningCatalogueGtk4Retire(panel);CHECK(Finish());
            CHECK(gtk_picture_get_paintable(GTK_PICTURE(picture))==NULL);
            goto cleanup;
        }
        CHECK(Finish());
        if(strcmp(mode,"retire-publication")==0) {
            CHECK(observer.calls>0U && observer.status==UMI_STATUS_OK && observer.mutation==UMI_STATUS_BUSY);
            CHECK(UmiLearningCatalogueGtk4CopyState(panel,&state)==UMI_STATUS_OK && state.retired && !state.busy);
            g_signal_handler_disconnect(picture,signal);signal=0U;
            goto cleanup;
        }
    }
    CHECK(Finish());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&before)==UMI_STATUS_OK && before.last_status==UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4CopyIllustration(panel,&image)==UMI_STATUS_OK);
    UmiStatus expected=!available?UMI_STATUS_NOT_IMPLEMENTED:
        strcmp(mode,"oversize")==0?UMI_STATUS_CAPACITY_EXCEEDED:
        strcmp(mode,"unsupported")==0?UMI_STATUS_NOT_IMPLEMENTED:
        strcmp(mode,"bad-crc")==0?UMI_STATUS_PARSE_ERROR:UMI_STATUS_OK;
    CHECK(image.status==expected && image.has_pixels==(expected==UMI_STATUS_OK));
    if(expected==UMI_STATUS_OK) {
        CHECK(image.width==2U && image.height==(strcmp(mode,"rgb")==0?1U:2U));
        CHECK(gtk_widget_get_visible(picture) && gtk_widget_get_visible(viewport));
        CHECK(strcmp(gtk_picture_get_alternative_text(GTK_PICTURE(picture)),"Four colour squares")==0);
        retained=g_object_ref(gtk_picture_get_paintable(GTK_PICTURE(picture)));
        CHECK(GDK_IS_TEXTURE(retained) && gdk_texture_get_width(GDK_TEXTURE(retained))==2);
    } else CHECK(!gtk_widget_get_visible(picture) && gtk_picture_get_paintable(GTK_PICTURE(picture))==NULL);
    if(strcmp(mode,"changed")==0) {
        CHECK(UmiRootedFileWrite(argv[2],"image.png","changed",7U)==UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4Reload(panel)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&state)==UMI_STATUS_OK && state.last_status==UMI_STATUS_INVALID_STATE);
        CHECK(state.reader.revision==before.reader.revision);
        CHECK(gtk_picture_get_paintable(GTK_PICTURE(picture))==retained);
    }
    if(strcmp(mode,"history")==0) {
        CHECK(UmiLearningCatalogueGtk4Follow(panel,"guide.html")==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyIllustration(panel,&image)==UMI_STATUS_NOT_FOUND);
        CHECK(!gtk_widget_get_visible(picture) && gtk_picture_get_paintable(GTK_PICTURE(picture))==NULL);
        if(retained!=NULL)CHECK(gdk_texture_get_width(GDK_TEXTURE(retained))==2);
        CHECK(UmiLearningCatalogueGtk4Back(panel)==UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyIllustration(panel,&image)==UMI_STATUS_OK && image.has_pixels==available);
    }
cleanup:
    if(signal!=0U && picture!=NULL)g_signal_handler_disconnect(picture,signal);
    if(panel!=NULL)UmiLearningCatalogueGtk4Retire(panel);
    if(!Finish())failed=1;
    g_clear_object(&retained);g_clear_object(&panel);
    g_clear_pointer(&direct.image_pixels,g_bytes_unref);g_free(direct.display);g_free(bytes);
    return failed;
}
