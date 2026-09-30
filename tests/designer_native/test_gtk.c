/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_native/test_gtk.c
 * PURPOSE:
 *   Real GTK tests. A missing display is NOT RUN (77), never a pass.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Real GTK tests. A missing display is NOT RUN (77), never a pass. */
#include "umicom/designer/native_gtk4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while(0)
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    if(!gtk_init_check()) return 77;
    UmiDeclDocument *doc=NULL;
    GtkWindow *window=NULL;
    CHECK(UmiDesignerNativeNotesDocument(&doc)==UMI_STATUS_OK);
    if(strcmp(argv[1],"invalid")==0) {
        CHECK(umi_decl_document_remove_node(doc,"result")==UMI_STATUS_OK);
        CHECK(UmiDesignerNativeGtkCreate(doc,&window)!=UMI_STATUS_OK && window==NULL);
        umi_decl_document_destroy(doc); return 0;
    }
    CHECK(UmiDesignerNativeGtkCreate(doc,&window)==UMI_STATUS_OK);
    GtkWidget *editor=UmiDesignerNativeGtkRefControl(window,"notes");
    GtkWidget *button=UmiDesignerNativeGtkRefControl(window,"count");
    GtkWidget *label=UmiDesignerNativeGtkRefControl(window,"result");
    CHECK(editor!=NULL && button!=NULL && label!=NULL);
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor));
    if(strcmp(argv[1],"count")==0) {
        gtk_text_buffer_set_text(buffer,"Aé🙂",-1);
        g_signal_emit_by_name(button,"clicked");
        CHECK(strcmp(gtk_label_get_text(GTK_LABEL(label)),"3 Unicode characters")==0);
    } else if(strcmp(argv[1],"retained_action")==0) {
        gtk_window_destroy(window);
        g_object_unref(window); window=NULL;
        /* The externally retained button now has no root/owner. */
        g_signal_emit_by_name(button,"clicked");
        CHECK(strcmp(gtk_label_get_text(GTK_LABEL(label)),"Choose Count characters after editing the draft.")==0);
    } else if(strcmp(argv[1],"independent")==0) {
        GtkWindow *second=NULL;
        CHECK(UmiDesignerNativeGtkCreate(doc,&second)==UMI_STATUS_OK);
        GtkWidget *other=UmiDesignerNativeGtkRefControl(second,"notes"); CHECK(other!=NULL);
        gtk_text_buffer_set_text(buffer,"changed first",-1);
        CHECK(gtk_text_buffer_get_char_count(gtk_text_view_get_buffer(GTK_TEXT_VIEW(other)))==27);
        g_object_unref(other); gtk_window_destroy(second); g_object_unref(second);
    } else if(strcmp(argv[1],"editor_limit")==0) {
        char *large=malloc(262146U); CHECK(large!=NULL);
        memset(large,'a',262145U); large[262145]='\0';
        gtk_text_buffer_set_text(buffer,"",-1);
        gtk_text_buffer_insert_at_cursor(buffer,large,262144);
        CHECK(gtk_text_buffer_get_char_count(buffer)==262144);
        gtk_text_buffer_insert_at_cursor(buffer,"x",1);
        CHECK(gtk_text_buffer_get_char_count(buffer)==262144);
        free(large);
    } else if(strcmp(argv[1],"lifecycle")==0) {
        CHECK(UmiDesignerNativeGtkRefControl(window,"missing")==NULL);
        CHECK(gtk_text_buffer_get_char_count(buffer)==27);
        umi_decl_document_destroy(doc); doc=NULL;
        g_signal_emit_by_name(button,"clicked");
        CHECK(strcmp(gtk_label_get_text(GTK_LABEL(label)),"27 Unicode characters")==0);
    } else return 2;
    g_object_unref(editor); g_object_unref(button); g_object_unref(label);
    if(window!=NULL) { gtk_window_destroy(window); g_object_unref(window); }
    umi_decl_document_destroy(doc); return 0;
}
