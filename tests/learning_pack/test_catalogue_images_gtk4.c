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
#include "../support/gtk4_expanders.h"
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
    /* Keep the original link fixture while adding explicit img references.
     * Existing link navigation still exercises the same first anchor. */
#if 0
    const char *guide="<h1>Image guide</h1><p><a href='image.png'>Open the illustration</a></p>";
#endif
    const char *guide="<h1>Image guide</h1><p><a href='image.png'>Open the illustration</a></p>"
        "<img src='image.png' alt='Owned colours'>"
        "<img src='https://invalid.example/image.png' alt='Remote'>"
        "<img src='guide.html' alt='Wrong kind'>"
        "<img src='absent.png' alt='Missing'>"
        "<img src='../outside.png' alt='Outside'>"
        "<img src='image.png#part' alt='Fragment'>"
        "<img src='image.png' SRC='other.png' alt='Ambiguous'>"
        "<img alt='No source'><img src='' alt='Empty source'>"
        "<img src='image.png#' alt='Empty fragment'>";
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
static void ReadingImagesChanged(GObject *object,GParamSpec *property,gpointer data)
{
    (void)object;(void)property;
    Observer *observer=data;++observer->calls;
    UmiBrowserReadingImage image;
    observer->status=UmiLearningCatalogueGtk4ReadingImageAt(observer->root,0U,&image);
    observer->mutation=UmiLearningCatalogueGtk4OpenReadingImage(observer->root,0U);
    UmiLearningCatalogueGtk4Retire(observer->root);
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    if(!gtk_init_check())return 77;
    const char *mode=argv[1];
    const char *known[]={"image","rgba-bytes","rgb","changed","history","linked-image",
        "bad-crc","oversize","unsupported","retire-pending","destroy-pending","retire-publication","arguments",
        "reference-open","reference-control","reference-missing","reference-remote","reference-wrong-kind",
        "reference-traversal","reference-fragment","reference-ambiguous","reference-no-source",
        "reference-empty-source","reference-empty-fragment","reference-changed","reference-history","reference-busy",
        "reference-retired","reference-arguments","reference-retire-publication","reference-notice"};
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
    if(g_str_has_prefix(mode,"reference-")) {
        UmiTestExpandSections(panel);
        GtkWidget *image_rows=Find(panel,"learning.catalogue.reading-images");CHECK(GTK_IS_DROP_DOWN(image_rows));
        UmiBrowserReadingImage reference;
        CHECK(UmiLearningCatalogueGtk4ReadingImageAt(panel,0U,&reference)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4Search(panel,"guide")==UMI_STATUS_OK);
        Observer publication={panel,0U,UMI_STATUS_NOT_FOUND,UMI_STATUS_OK};
        gulong published_signal=strcmp(mode,"reference-retire-publication")==0?
            g_signal_connect(image_rows,"notify::model",G_CALLBACK(ReadingImagesChanged),&publication):0U;
        UmiStatus opened=UmiLearningCatalogueGtk4Open(panel,0U);
        int finished=Finish();
        if(published_signal!=0U)g_signal_handler_disconnect(image_rows,published_signal);
        CHECK(opened==UMI_STATUS_OK && finished);
        if(strcmp(mode,"reference-retire-publication")==0) {
            CHECK(publication.calls>0U && publication.status==UMI_STATUS_OK && publication.mutation==UMI_STATUS_BUSY);
            CHECK(UmiLearningCatalogueGtk4CopyState(panel,&state)==UMI_STATUS_OK && state.retired && !state.busy);
            goto cleanup;
        }
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&before)==UMI_STATUS_OK && before.has_reading_structure);
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(image_rows)))==10U);
        CHECK(UmiLearningCatalogueGtk4ReadingImageAt(panel,0U,&reference)==UMI_STATUS_OK &&
            reference.target_status==UMI_STATUS_OK && strcmp(reference.target,"image.png")==0);
        if(strcmp(mode,"reference-arguments")==0) {
            UmiBrowserReadingImage saved=reference;
            CHECK(UmiLearningCatalogueGtk4ReadingImageAt(NULL,0U,&reference)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningCatalogueGtk4ReadingImageAt(panel,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningCatalogueGtk4ReadingImageAt(panel,99U,&reference)==UMI_STATUS_NOT_FOUND);
            CHECK(strcmp(reference.target,saved.target)==0 && reference.byte_start==saved.byte_start);
            CHECK(UmiLearningCatalogueGtk4OpenReadingImage(NULL,0U)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLearningCatalogueGtk4OpenReadingImage(panel,99U)==UMI_STATUS_NOT_FOUND);
            goto cleanup;
        }
        if(strcmp(mode,"reference-retired")==0) {
            UmiLearningCatalogueGtk4Retire(panel);
            CHECK(UmiLearningCatalogueGtk4OpenReadingImage(panel,0U)==UMI_STATUS_INVALID_STATE);goto cleanup;
        }
        if(strcmp(mode,"reference-notice")==0) {
            GtkWidget *button=Find(panel,"learning.catalogue.reading-open-image");CHECK(GTK_IS_BUTTON(button));
            GtkWidget *label=Find(panel,"learning.catalogue.status");CHECK(GTK_IS_LABEL(label));
            gtk_drop_down_set_selected(GTK_DROP_DOWN(image_rows),1U);
            Observer notice={panel,0U,UMI_STATUS_NOT_FOUND,UMI_STATUS_OK};
            gulong notice_signal=g_signal_connect(label,"notify::label",G_CALLBACK(ReadingImagesChanged),&notice);
            g_signal_emit_by_name(button,"clicked");
            g_signal_handler_disconnect(label,notice_signal);
            CHECK(notice.calls>0U && notice.status==UMI_STATUS_OK && notice.mutation==UMI_STATUS_BUSY);
            CHECK(UmiLearningCatalogueGtk4CopyState(panel,&state)==UMI_STATUS_OK && state.retired && !state.busy);
            CHECK(state.reader.revision==before.reader.revision && UmiLearningCatalogueGtk4PendingReads()==0U);
            goto cleanup;
        }
        const struct {const char *mode;size_t index;UmiStatus expected;} refusals[]={
            {"reference-remote",1U,UMI_STATUS_INVALID_ARGUMENT},
            {"reference-wrong-kind",2U,UMI_STATUS_INVALID_ARGUMENT},
            {"reference-missing",3U,UMI_STATUS_NOT_FOUND},
            {"reference-traversal",4U,UMI_STATUS_INVALID_ARGUMENT},
            {"reference-fragment",5U,UMI_STATUS_INVALID_ARGUMENT},
            {"reference-ambiguous",6U,UMI_STATUS_INVALID_ARGUMENT},
            {"reference-no-source",7U,UMI_STATUS_NOT_FOUND},
            {"reference-empty-source",8U,UMI_STATUS_NOT_FOUND},
            {"reference-empty-fragment",9U,UMI_STATUS_INVALID_ARGUMENT}
        };
        for(size_t i=0U;i<sizeof(refusals)/sizeof(refusals[0]);++i)if(strcmp(mode,refusals[i].mode)==0) {
            CHECK(UmiLearningCatalogueGtk4OpenReadingImage(panel,refusals[i].index)==refusals[i].expected);
            CHECK(UmiLearningCatalogueGtk4PendingReads()==0U);
            CHECK(UmiLearningCatalogueGtk4CopyState(panel,&state)==UMI_STATUS_OK &&
                state.reader.revision==before.reader.revision && state.reader.history_count==before.reader.history_count);
            goto cleanup;
        }
        if(strcmp(mode,"reference-changed")==0)
            CHECK(UmiRootedFileWrite(argv[2],"image.png","changed",7U)==UMI_STATUS_OK);
        if(strcmp(mode,"reference-control")==0) {
            GtkWidget *button=Find(panel,"learning.catalogue.reading-open-image");CHECK(GTK_IS_BUTTON(button));
            g_signal_emit_by_name(button,"clicked");
        } else CHECK(UmiLearningCatalogueGtk4OpenReadingImage(panel,0U)==UMI_STATUS_OK);
        if(strcmp(mode,"reference-busy")==0)
            CHECK(UmiLearningCatalogueGtk4OpenReadingImage(panel,0U)==UMI_STATUS_BUSY);
        CHECK(Finish());CHECK(UmiLearningCatalogueGtk4CopyState(panel,&state)==UMI_STATUS_OK);
        if(strcmp(mode,"reference-changed")==0) {
            CHECK(state.last_status==UMI_STATUS_INVALID_STATE && state.reader.revision==before.reader.revision);
            CHECK(state.has_reading_structure && strcmp(state.reader.selection.resource.id,"guide")==0);
        } else {
            CHECK(state.last_status==UMI_STATUS_OK && strcmp(state.reader.selection.resource.id,"image")==0);
            CHECK(UmiLearningCatalogueGtk4CopyIllustration(panel,&image)==UMI_STATUS_OK && image.has_pixels==available);
            CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(image_rows)))==0U);
            if(strcmp(mode,"reference-history")==0) {
                CHECK(UmiLearningCatalogueGtk4Back(panel)==UMI_STATUS_OK && Finish());
                CHECK(UmiLearningCatalogueGtk4ReadingImageAt(panel,0U,&reference)==UMI_STATUS_OK);
                CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(image_rows)))==10U);
            }
        }
        goto cleanup;
    }
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
