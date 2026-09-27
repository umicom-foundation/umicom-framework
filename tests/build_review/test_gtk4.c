/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_review/test_gtk4.c
 *
 * PURPOSE:
 *   Check review-window ownership using the real GTK adapter when a display is available.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/review_gtk4.h"
#include "umicom/build/parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while(0)
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root),name)==0) return root;
    for(GtkWidget *c=gtk_widget_get_first_child(root);c!=NULL;c=gtk_widget_get_next_sibling(c)) {
        GtkWidget *found=Find(c,name);if(found!=NULL)return found;
    }
    return NULL;
}
static char *Text(GtkWidget *view)
{
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter first,last;gtk_text_buffer_get_bounds(buffer,&first,&last);
    return gtk_text_buffer_get_text(buffer,&first,&last,FALSE);
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!gtk_init_check())return 77;
    UmiBuildHistory *history=NULL;UmiBuildResult *r=calloc(1U,sizeof *r);CHECK(r!=NULL);
    CHECK(umi_build_history_create(2U,&history)==UMI_STATUS_OK);
    r->state=UMI_BUILD_STATE_FAILED;r->phase=UMI_BUILD_PHASE_BUILD;r->status=UMI_STATUS_IO_ERROR;r->exit_code=1;r->operation_id=11U;
    strcpy(r->output,"notes.c:1:1: error: practice fault\n");
    CHECK(umi_build_parse_output(r->output,&r->diagnostics)==UMI_STATUS_OK);
    if(strcmp(argv[1],"invalid-record")==0)r->diagnostics.count=SIZE_MAX;
    if(strcmp(argv[1],"empty")!=0)CHECK(umi_build_history_append(history,r)==UMI_STATUS_OK);
    GtkWindow *window=UmiGtk4BuildReviewPresent(NULL,history);CHECK(window!=NULL);g_object_ref(window);
    GtkWidget *query=Find(GTK_WIDGET(window),"umicom-build-review-filter");
    GtkWidget *report=Find(GTK_WIDGET(window),"umicom-build-review-report");CHECK(query!=NULL && report!=NULL);
    if(strcmp(argv[1],"detached-owner")==0){umi_build_history_destroy(history);history=NULL;gtk_editable_set_text(GTK_EDITABLE(query),"practice");char *text=Text(report);CHECK(strstr(text,"Recorded failure")!=NULL);g_free(text);}
    else if(strcmp(argv[1],"filter")==0){gtk_editable_set_text(GTK_EDITABLE(query),"not-present");char *text=Text(report);CHECK(strstr(text,"0 visible / 1 retained")!=NULL && strstr(text,"Recorded failure")!=NULL);g_free(text);}
    else if(strcmp(argv[1],"empty")==0){char *text=Text(report);CHECK(strstr(text,"No retained")!=NULL);g_free(text);}
    else if(strcmp(argv[1],"invalid-record")==0){char *text=Text(report);CHECK(strstr(text,"Cannot capture")!=NULL);g_free(text);}
    else if(strcmp(argv[1],"retained-child")==0){g_object_ref(query);gtk_window_destroy(window);g_object_unref(window);window=NULL;gtk_editable_set_text(GTK_EDITABLE(query),"after close");g_object_unref(query);}
    else if(strcmp(argv[1],"lifecycle")!=0)return 2;
    if(window!=NULL){gtk_window_destroy(window);g_object_unref(window);}
    umi_build_history_destroy(history);free(r);
    while(g_main_context_iteration(NULL,FALSE)){}
    return 0;
}
