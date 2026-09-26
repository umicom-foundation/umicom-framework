/* Umicom Framework teaching project: framework / window.c
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * This inspectable lesson is not a replacement for production Framework services. */
#include <gtk/gtk.h>
#include "branding.h"
#include "umicom/data/data_server.h"
/* View state borrows the service. Main owns it until g_application_run returns. */
typedef struct NotesView {UmiDataServer *server;GtkTextBuffer *buffer;GtkLabel *status;} NotesView;
static void Save(GtkButton *button,gpointer context)
{
    NotesView *view=context;GtkTextIter begin,end;(void)button;
    gtk_text_buffer_get_bounds(view->buffer,&begin,&end);
    char *text=gtk_text_buffer_get_text(view->buffer,&begin,&end,FALSE);
    UmiStatus status=umi_data_server_set(view->server,"note",text);
    gtk_label_set_text(view->status,status==UMI_STATUS_OK?"Saved in memory":"Save failed: check the text size");
    g_free(text);
}
static void Load(GtkButton *button,gpointer context)
{
    NotesView *view=context;char text[4096];(void)button;
    UmiStatus status=umi_data_server_get(view->server,"note",text,sizeof text);
    if (status==UMI_STATUS_OK) gtk_text_buffer_set_text(view->buffer,text,-1);
    gtk_label_set_text(view->status,status==UMI_STATUS_OK?"Loaded saved text":"No saved note is available");
}
static void Activate(GtkApplication *application,gpointer context)
{
    NotesView *view=context;
    GtkWidget *existing=GTK_WIDGET(gtk_application_get_active_window(application));
    if (existing!=NULL) {gtk_window_present(GTK_WINDOW(existing));return;}
    GtkWidget *window=gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(window),"Umicom Notes - Framework lesson");
    gtk_window_set_default_size(GTK_WINDOW(window),680,440);
    /* Embed the supplied logo rather than searching beside the working folder. */
    GBytes *logoBytes=g_bytes_new_static(UMICOM_LESSON_LOGO,sizeof UMICOM_LESSON_LOGO);
    GError *logoError=NULL;
    GdkTexture *logo=gdk_texture_new_from_bytes(logoBytes,&logoError);
    g_bytes_unref(logoBytes);
    if(logo!=NULL){
        GtkWidget *header=gtk_header_bar_new();
        GtkWidget *picture=gtk_picture_new_for_paintable(GDK_PAINTABLE(logo));
        gtk_widget_set_size_request(picture,150,40);
        gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header),picture);
        gtk_window_set_titlebar(GTK_WINDOW(window),header);
        g_object_unref(logo);
    }
    g_clear_error(&logoError);
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);
    gtk_widget_set_margin_start(box,16);gtk_widget_set_margin_end(box,16);
    gtk_widget_set_margin_top(box,16);gtk_widget_set_margin_bottom(box,16);
    gtk_box_append(GTK_BOX(box),gtk_label_new("Practice only: memory storage is lost when this window closes."));
    GtkWidget *editor=gtk_text_view_new(),*scroll=gtk_scrolled_window_new();
    view->buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor));
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(editor),GTK_WRAP_WORD_CHAR);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),editor);
    gtk_widget_set_vexpand(scroll,TRUE);gtk_box_append(GTK_BOX(box),scroll);
    GtkWidget *row=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8),*save=gtk_button_new_with_label("Save"),*load=gtk_button_new_with_label("Load");
    gtk_box_append(GTK_BOX(row),save);gtk_box_append(GTK_BOX(row),load);
    gtk_box_append(GTK_BOX(box),row);view->status=GTK_LABEL(gtk_label_new("Write a fictional note, then choose Save."));
    gtk_box_append(GTK_BOX(box),GTK_WIDGET(view->status));
    g_signal_connect(save,"clicked",G_CALLBACK(Save),view);
    g_signal_connect(load,"clicked",G_CALLBACK(Load),view);
    gtk_window_set_child(GTK_WINDOW(window),box);gtk_window_present(GTK_WINDOW(window));
}
int main(int argc,char **argv)
{
    NotesView view={0};
    if (umi_data_server_create_memory(&view.server)!=UMI_STATUS_OK) return 1;
    GtkApplication *application=gtk_application_new("org.umicom.lesson.notes",G_APPLICATION_NON_UNIQUE);
    g_signal_connect(application,"activate",G_CALLBACK(Activate),&view);
    int result=g_application_run(G_APPLICATION(application),argc,argv);
    g_object_unref(application);umi_data_server_destroy(view.server);return result;
}
