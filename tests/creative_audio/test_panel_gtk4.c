/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * GTK-only, in-memory lifecycle tests. No file read, write or playback. */
#include "umicom/ui/gtk4/creative_audio.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);result=1;goto done;}}while(0)
static GtkWidget *Find(GtkWidget *root,const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if(tag&&strcmp(tag,id)==0)return root;
    for(GtkWidget *p=gtk_widget_get_first_child(root);p;p=gtk_widget_get_next_sibling(p)){
        GtkWidget *found=Find(p,id);if(found)return found;
    }return NULL;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    (void)g_setenv("GTK_A11Y","test",TRUE);(void)g_setenv("GSETTINGS_BACKEND","memory",TRUE);
    if(!gtk_init_check())return 77;
    const unsigned char wave[]={'R','I','F','F',40,0,0,0,'W','A','V','E','f','m','t',' ',16,0,0,0,
        1,0,1,0,128,187,0,0,0,119,1,0,2,0,16,0,'d','a','t','a',4,0,0,0,0,128,255,127};
    int result=0;GtkWidget *root=UmiCreativeAudioGtkCreate(),*retained=NULL,*second=NULL;
    g_object_ref_sink(root);
    GtkWidget *write=Find(root,"creative.audio.write"),*preview=Find(root,"creative.audio.preview");
    CHECK(write&&preview&&!gtk_widget_get_sensitive(write));
    CHECK(UmiCreativeAudioGtkLoadBytes(root,wave,sizeof(wave))==UMI_STATUS_OK);
    g_signal_emit_by_name(preview,"clicked");CHECK(gtk_widget_get_sensitive(write));
    if(!strcmp(argv[1],"invalidate")){
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(root,"creative.audio.gain")),500);
        CHECK(!gtk_widget_get_sensitive(write));
        g_signal_emit_by_name(preview,"clicked");CHECK(gtk_widget_get_sensitive(write));
    }else if(!strcmp(argv[1],"bad_load")){
        CHECK(UmiCreativeAudioGtkLoadBytes(root,"bad",3)!=UMI_STATUS_OK);
        g_signal_emit_by_name(preview,"clicked");CHECK(gtk_widget_get_sensitive(write));
    }else if(!strcmp(argv[1],"independent")){
        second=UmiCreativeAudioGtkCreate();g_object_ref_sink(second);
        CHECK(!gtk_widget_get_sensitive(Find(second,"creative.audio.write")));
    }else if(!strcmp(argv[1],"retained_button")){
        retained=g_object_ref(preview);g_object_unref(root);root=NULL;g_signal_emit_by_name(retained,"clicked");
    }else if(!strcmp(argv[1],"retained_drawing")){
        retained=g_object_ref(Find(root,"creative.audio.waveform"));g_object_unref(root);root=NULL;
        CHECK(GTK_IS_DRAWING_AREA(retained));gtk_widget_queue_draw(retained);
    }else if(strcmp(argv[1],"preview")!=0){result=2;}
done:
    if(retained)g_object_unref(retained);
    if(root)g_object_unref(root);
    if(second)g_object_unref(second);
    return result;
}
