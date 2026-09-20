/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workflow_tools/test_review_gtk4.c
 * PURPOSE:
 *   Exercise the real finance review component through GTK signals and tags.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/finance_review.h"
#include "umicom/ui/gtk4/automation.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while(0)

static GtkWidget *Find(GtkWidget *root,const char *id) {
    /* Read the stable tag installed by umi_gtk4_automation_tag_widget. */
    const char *actual=g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if(actual!=NULL && strcmp(actual,id)==0)return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,id);if(found!=NULL)return found;
    }return NULL;
}
static int Run(const char *name) {
    GtkWidget *root=NULL;
    int cash=strcmp(name,"cash")==0;
    CHECK(UmiGtk4FinanceReviewCreate(cash?UMI_GTK4_FINANCE_REVIEW_CASH:UMI_GTK4_FINANCE_REVIEW_ACCOUNT,&root)==UMI_STATUS_OK);
    g_object_ref_sink(root);
    GtkWidget *amount0=Find(root,"finance.review.amount.0"), *amount1=Find(root,"finance.review.amount.1"),
      *amount2=Find(root,"finance.review.amount.2"), *calculate=Find(root,"finance.review.calculate"),
      *result=Find(root,"finance.review.result");
    CHECK(amount0&&amount1&&amount2&&calculate&&result);
    gtk_editable_set_text(GTK_EDITABLE(amount0),"1000.00");
    gtk_editable_set_text(GTK_EDITABLE(amount1),"100.00");
    gtk_editable_set_text(GTK_EDITABLE(amount2),"250.00");
    if(cash)gtk_editable_set_text(GTK_EDITABLE(Find(root,"finance.review.amount.3")),"200.00");
    if(strcmp(name,"invalid")==0)gtk_editable_set_text(GTK_EDITABLE(amount0),"bad");
    g_signal_emit_by_name(calculate,"clicked");
    const char *text=gtk_label_get_text(GTK_LABEL(result));
    if(strcmp(name,"invalid")==0)CHECK(strstr(text,"invalid")!=NULL);
    else if(cash)CHECK(strstr(text,"850.00")!=NULL && strstr(text,"650.00")!=NULL);
    else CHECK(strstr(text,"900.00")!=NULL && strstr(text,"650.00")!=NULL);
    if(strcmp(name,"invalidate")==0) {
        gtk_editable_set_text(GTK_EDITABLE(amount2),"100.00");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(result)),"Inputs changed")!=NULL);
    } else if(strcmp(name,"clear")==0) {
        g_signal_emit_by_name(Find(root,"finance.review.clear"),"clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(amount0)),"0")==0);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(result)),"cleared")!=NULL);
    } else if(strcmp(name,"lifetime")==0) {
        g_object_ref(calculate);g_object_unref(root);root=NULL;
        /* Signal connections watch the destroyed root, not a freed user context. */
        g_signal_emit_by_name(calculate,"clicked");g_object_unref(calculate);
    } else if(!cash && strcmp(name,"account")!=0 && strcmp(name,"invalid")!=0) return 2;
    if(root!=NULL)g_object_unref(root);
    return 0;
}
int main(int argc,char **argv) {
    if(argc!=2)return 2;
    if(!gtk_init_check())return 77;
    return Run(argv[1]);
}
