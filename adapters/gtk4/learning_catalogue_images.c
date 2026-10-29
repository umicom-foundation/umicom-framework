/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_images.c
 *
 * PURPOSE:
 *   Decode verified bounded PNG resources through Media and publish owned pixels on the GTK thread.
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
#ifdef UMICOM_LEARNING_PNG
#include "umicom/media/png_image.h"
#endif

int UmiLearningCatalogueGtk4PngAvailable(void)
{
#ifdef UMICOM_LEARNING_PNG
    return UmiMediaPngAvailable();
#else
    return 0;
#endif
}
#ifdef UMICOM_LEARNING_PNG
static uint32_t ImageWord(const unsigned char *bytes)
{
    return ((uint32_t)bytes[0]<<24U) | ((uint32_t)bytes[1]<<16U) |
        ((uint32_t)bytes[2]<<8U) | (uint32_t)bytes[3];
}
static UmiStatus DecodeIllustration(LearningCatalogueJob *job,
    const unsigned char *bytes,size_t size)
{
    static const unsigned char signature[]={137U,80U,78U,71U,13U,10U,26U,10U};
    if(size<sizeof(signature) || memcmp(bytes,signature,sizeof(signature))!=0)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if(size<33U || ImageWord(bytes+8U)!=13U || memcmp(bytes+12U,"IHDR",4U)!=0)
        return UMI_STATUS_PARSE_ERROR;
    uint32_t width=ImageWord(bytes+16U),height=ImageWord(bytes+20U);
    if(width==0U || height==0U) return UMI_STATUS_PARSE_ERROR;
    /* This preflight only bounds the allocation. The shared decoder still
     * validates the complete PNG envelope, CRCs, format and pixel stream. */
    if(width>2048U || height>2048U || (uint64_t)width*height>1048576U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiMediaImageSurface *surface=NULL;
    UmiStatus status=UmiMediaPngDecode(bytes,size,NULL,&surface);
    if(status!=UMI_STATUS_OK) return status;
    UmiMediaImageSurfaceSnapshot snapshot;
    status=umi_media_image_surface_snapshot(surface,&snapshot);
    if(status!=UMI_STATUS_OK || snapshot.width!=width || snapshot.height!=height) {
        umi_media_image_surface_destroy(surface);
        return status==UMI_STATUS_OK?UMI_STATUS_INVALID_STATE:status;
    }
    size_t stride=(size_t)width*4U,byte_count=stride*(size_t)height;
    unsigned char *pixels=g_try_malloc(byte_count);
    UmiMediaRgbaPixel *row=g_try_new(UmiMediaRgbaPixel,width);
    if(pixels==NULL || row==NULL) status=UMI_STATUS_OUT_OF_MEMORY;
    for(size_t y=0U;status==UMI_STATUS_OK && y<(size_t)height;++y) {
        status=umi_media_image_surface_read_row(surface,y,row,(size_t)width);
        if(status!=UMI_STATUS_OK) break;
        for(size_t x=0U;x<(size_t)width;++x) {
            size_t at=y*stride+x*4U;
            pixels[at]=row[x].red;pixels[at+1U]=row[x].green;
            pixels[at+2U]=row[x].blue;pixels[at+3U]=row[x].alpha;
        }
    }
    g_free(row);umi_media_image_surface_destroy(surface);
    if(status!=UMI_STATUS_OK) { g_free(pixels);return status; }
    /* GBytes carries immutable raw pixels across the worker boundary. GTK
     * creates the texture later; no GTK widget is used by this worker. */
    job->image_pixels=g_bytes_new_take(pixels,byte_count);
    job->image_width=width;job->image_height=height;
    return UMI_STATUS_OK;
}
#endif
void LearningCataloguePrepareImage(LearningCatalogueJob *job,
    const unsigned char *bytes,size_t size)
{
    job->image_status=UMI_STATUS_NOT_IMPLEMENTED;
#ifdef UMICOM_LEARNING_PNG
    if(UmiMediaPngAvailable()) job->image_status=DecodeIllustration(job,bytes,size);
#else
    (void)bytes;(void)size;
#endif
    if(job->image_status==UMI_STATUS_OUT_OF_MEMORY) {
        job->status=job->image_status;return;
    }
    const char *message=job->image_status==UMI_STATUS_OK ?
        "Verified PNG illustration. Source colours are shown without colour-profile conversion." :
        job->image_status==UMI_STATUS_NOT_IMPLEMENTED ?
        "Verified image bytes. This reader needs the optional PNG decoder and a supported static PNG." :
        job->image_status==UMI_STATUS_CAPACITY_EXCEEDED ?
        "Verified image bytes. The illustration exceeds this reader's dimension or pixel limit." :
        "Verified image bytes. The PNG could not be decoded; no image pixels were published.";
    job->display=g_strdup(message);
    g_strlcpy(job->notice,message,sizeof(job->notice));
}
void LearningCatalogueImageControls(LearningCatalogue *owner)
{
    owner->illustration_view=gtk_picture_new();
    g_object_set_data_full(G_OBJECT(owner->illustration_view),"umicom-automation-id",
        g_strdup("learning.catalogue.illustration"),g_free);
    gtk_picture_set_can_shrink(GTK_PICTURE(owner->illustration_view),TRUE);
    gtk_picture_set_content_fit(GTK_PICTURE(owner->illustration_view),GTK_CONTENT_FIT_CONTAIN);
    /* Retain the direct layout for review. A bounded viewport now prevents a
     * large image's natural size from expanding the entire learning window. */
#if 0
    gtk_widget_set_size_request(owner->illustration_view,-1,240);
    gtk_widget_set_visible(owner->illustration_view,FALSE);
    gtk_box_append(GTK_BOX(owner->root),owner->illustration_view);
#endif
    owner->illustration_scroll=gtk_scrolled_window_new();
    g_object_set_data_full(G_OBJECT(owner->illustration_scroll),"umicom-automation-id",
        g_strdup("learning.catalogue.illustration-scroll"),g_free);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(owner->illustration_scroll),
        GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(owner->illustration_scroll),160);
    gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(owner->illustration_scroll),280);
    gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(owner->illustration_scroll),TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(owner->illustration_scroll),owner->illustration_view);
    gtk_widget_set_visible(owner->illustration_view,FALSE);
    gtk_widget_set_visible(owner->illustration_scroll,FALSE);
    gtk_box_append(GTK_BOX(owner->root),owner->illustration_scroll);
}
void LearningCatalogueImagePublish(LearningCatalogue *owner,LearningCatalogueJob *job)
{
    int selected=job->snapshot.selection.resource.kind==UMI_LEARNING_RESOURCE_IMAGE;
    owner->illustration=(UmiLearningCatalogueIllustrationState){
        selected?job->image_status:UMI_STATUS_NOT_FOUND,
        (size_t)job->image_width,(size_t)job->image_height,job->image_pixels!=NULL};
    if(owner->state.retired) return;
    GdkTexture *texture=NULL;
    if(job->image_pixels!=NULL)
        texture=gdk_memory_texture_new((int)job->image_width,(int)job->image_height,
            GDK_MEMORY_R8G8B8A8,job->image_pixels,(size_t)job->image_width*4U);
    gtk_picture_set_paintable(GTK_PICTURE(owner->illustration_view),
        texture!=NULL?GDK_PAINTABLE(texture):NULL);
    g_clear_object(&texture);
    if(owner->state.retired) return;
    char *alternative=g_utf8_make_valid(selected?job->snapshot.selection.resource.title:"",-1);
    gtk_picture_set_alternative_text(GTK_PICTURE(owner->illustration_view),alternative);
    g_free(alternative);
    if(owner->state.retired) return;
    gtk_widget_set_visible(owner->illustration_view,job->image_pixels!=NULL);
    if (owner->state.retired) return;
    gtk_widget_set_visible(owner->illustration_scroll,job->image_pixels!=NULL);
}
UmiStatus UmiLearningCatalogueGtk4CopyIllustration(GtkWidget *widget,
    UmiLearningCatalogueIllustrationState *out_state)
{
    LearningCatalogue *owner=LearningCatalogueOwner(widget);
    if(owner==NULL || out_state==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(!owner->state.reader.has_document ||
        owner->state.reader.selection.resource.kind!=UMI_LEARNING_RESOURCE_IMAGE)
        return UMI_STATUS_NOT_FOUND;
    *out_state=owner->illustration;
    return UMI_STATUS_OK;
}
