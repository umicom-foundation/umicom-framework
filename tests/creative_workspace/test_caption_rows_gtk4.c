/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_caption_rows_gtk4.c
 * PURPOSE: Check caption forms, undo and stale-row protection through real shared GTK controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/captions.h"
#include "umicom/ui/gtk4/automation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); exit(1); } } while(0)
static const char original[] = "1\n00:00:01,000 --> 00:00:04,000\nFirst\n\n2\n00:00:03,000 --> 00:00:05,000\nSecond\n\n";
static GtkWidget *Get(GtkWidget *root, const char *id)
{
    GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root,id);
    CHECK(widget != NULL);
    return widget;
}
static void Click(GtkWidget *root, const char *id) { g_signal_emit_by_name(Get(root,id),"clicked"); }
static void Set(GtkWidget *root,const char *id,const char *text)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Get(root,id))),text,-1);
}
static char *Read(GtkWidget *root,const char *id)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Get(root,id)));
    GtkTextIter first,last; gtk_text_buffer_get_bounds(buffer,&first,&last);
    return gtk_text_buffer_get_text(buffer,&first,&last,FALSE);
}
static void Number(GtkWidget *root,const char *id,double value)
{
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(Get(root,id)),value);
}
static void Expect(GtkWidget *root,const char *text)
{
    char *actual=Read(root,"captions.editor"); CHECK(!strcmp(actual,text)); g_free(actual);
}
/* A host may change the main document while loading the row form. The completed
 * form must not claim it still describes the prepared document it started with. */
static void Interfere(GtkTextBuffer *buffer,gpointer root)
{
    (void)buffer;
    Set(root,"captions.editor","A newer draft from the host");
}
int main(int argc,char **argv)
{
    CHECK(argc==2);
    if(!gtk_init_check()) return 77;
    const char *mode=argv[1];
    GtkWidget *root=NULL; CHECK(UmiCaptionsGtkCreate(&root)==UMI_STATUS_OK);
    g_object_ref_sink(root);
    Set(root,"captions.editor",original); Click(root,"captions.prepare");
    Click(root,"captions.row-load");
    if(!strcmp(mode,"load")) {
        char *text=Read(root,"captions.row-text"); CHECK(!strcmp(text,"First")); g_free(text);
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(Get(root,"captions.row-begin")))==1000);
        Expect(root,original);
    } else if(!strcmp(mode,"replace") || !strcmp(mode,"undo")) {
        Set(root,"captions.row-text","Revised");
        Click(root,"captions.row-replace");
        const char *expected="1\n00:00:01,000 --> 00:00:04,000\nRevised\n\n2\n00:00:03,000 --> 00:00:05,000\nSecond\n\n";
        Expect(root,expected);
        if(!strcmp(mode,"undo")) {
            Click(root,"captions.undo"); Expect(root,original);
            Click(root,"captions.undo"); Expect(root,expected);
        }
    } else if(!strcmp(mode,"insert")) {
        Number(root,"captions.row-number",3); Number(root,"captions.row-begin",6000); Number(root,"captions.row-end",7000);
        Set(root,"captions.row-text","Third"); Click(root,"captions.row-insert");
        Expect(root,"1\n00:00:01,000 --> 00:00:04,000\nFirst\n\n2\n00:00:03,000 --> 00:00:05,000\nSecond\n\n3\n00:00:06,000 --> 00:00:07,000\nThird\n\n");
    } else if(!strcmp(mode,"remove")) {
        Click(root,"captions.row-remove");
        Expect(root,"1\n00:00:03,000 --> 00:00:05,000\nSecond\n\n");
        Click(root,"captions.undo"); Expect(root,original);
    } else if(!strcmp(mode,"split")) {
        Number(root,"captions.row-split-time",2000);
        Set(root,"captions.row-text","Part one"); Set(root,"captions.row-second-text","Part two");
        Click(root,"captions.row-split");
        Expect(root,"1\n00:00:01,000 --> 00:00:02,000\nPart one\n\n2\n00:00:02,000 --> 00:00:04,000\nPart two\n\n3\n00:00:03,000 --> 00:00:05,000\nSecond\n\n");
    } else if(!strcmp(mode,"merge")) {
        Set(root,"captions.row-text","This unsaved text must not be merged");
        Click(root,"captions.row-merge");
        Expect(root,"1\n00:00:01,000 --> 00:00:05,000\nFirst\nSecond\n\n");
    } else if(!strcmp(mode,"invalid")) {
        Number(root,"captions.row-begin",4500); Click(root,"captions.row-replace");
        Expect(root,original);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Get(root,"captions.message"))),"Caption edit:")!=NULL);
    } else if(!strcmp(mode,"stale")) {
        Number(root,"captions.offset",1000); Click(root,"captions.shift");
        char *shifted=Read(root,"captions.editor");
        Click(root,"captions.row-remove"); Expect(root,shifted); g_free(shifted);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Get(root,"captions.message"))),"Load this caption again")!=NULL);
    } else if(!strcmp(mode,"row-selection")) {
        Number(root,"captions.row-number",2); Click(root,"captions.row-remove"); Expect(root,original);
    } else if(!strcmp(mode,"draft-change")) {
        Set(root,"captions.editor","Keep the new unfinished draft");
        Click(root,"captions.row-replace"); Expect(root,"Keep the new unfinished draft");
    } else if(!strcmp(mode,"reentrant")) {
        GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(Get(root,"captions.row-text")));
        gulong handler=g_signal_connect(buffer,"changed",G_CALLBACK(Interfere),root);
        Click(root,"captions.row-load"); g_signal_handler_disconnect(buffer,handler);
        Click(root,"captions.row-replace"); Expect(root,"A newer draft from the host");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Get(root,"captions.message"))),"Prepare")!=NULL);
    } else if(!strcmp(mode,"retained")) {
        GtkWidget *button=g_object_ref(Get(root,"captions.row-remove"));
        g_object_unref(root); root=NULL;
        g_signal_emit_by_name(button,"clicked"); g_object_unref(button);
    } else CHECK(false);
    if(root!=NULL) g_object_unref(root);
    return 0;
}
