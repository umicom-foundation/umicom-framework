/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_text_comparison_gtk4.c
 * PURPOSE: Exercise the real read-only GTK comparison and retained controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/text_comparison.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);failed=1;goto cleanup;}}while(0)
static GtkWidget *Find(GtkWidget *root,const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if(tag&&strcmp(tag,id)==0)return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child;child=gtk_widget_get_next_sibling(child)){
        GtkWidget *found=Find(child,id);if(found)return found;
    }
    return NULL;
}
int main(void)
{
    if(!gtk_init_check())return 77;
    GtkWidget *root=NULL,*next=NULL;int failed=0;
    CHECK(UmiGtk4TextComparisonCreate("old\n",4,"new\n",4,"Editor","Saved",&root)==UMI_STATUS_OK);
    g_object_ref_sink(root);
    GtkWidget *left=Find(root,"umicom.comparison.left"),*right=Find(root,"umicom.comparison.right");
    next=Find(root,"umicom.comparison.next");
    CHECK(GTK_IS_TEXT_VIEW(left)&&GTK_IS_TEXT_VIEW(right)&&GTK_IS_BUTTON(next));
    CHECK(!gtk_text_view_get_editable(GTK_TEXT_VIEW(left))&&!gtk_text_view_get_editable(GTK_TEXT_VIEW(right)));
    CHECK(gtk_widget_get_sensitive(next));
    g_signal_emit_by_name(next,"clicked");
    CHECK(!gtk_widget_get_sensitive(next));
    GtkTextIter first,last;
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(left));
    CHECK(gtk_text_buffer_get_selection_bounds(buffer,&first,&last));
    char *text=gtk_text_buffer_get_text(buffer,&first,&last,FALSE);
    int matches=strcmp(text,"old\n")==0;g_free(text);CHECK(matches);
    g_object_ref(next);
    g_object_unref(root);root=NULL;
    /* The root-bound signal disconnects on destruction; this is a no-op. */
    g_signal_emit_by_name(next,"clicked");g_clear_object(&next);
cleanup:
    if(root)g_object_unref(root);
    return failed;
}
