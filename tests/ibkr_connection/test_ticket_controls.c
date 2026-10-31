/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_ticket_controls.c
 *
 * PURPOSE:
 *   Exercise real order-ticket and watchlist controls without a brokerage connection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/workspace_gtk4.h"
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
static GtkWidget *Find(GtkWidget *root,const char *name)
{
    if (!strcmp(gtk_widget_get_name(root),name)) return root;
    for (GtkWidget *c=gtk_widget_get_first_child(root);c;c=gtk_widget_get_next_sibling(c)) {
        GtkWidget *found=Find(c,name); if(found)return found;
    }
    return NULL;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!gtk_init_check())return 77;
    GtkWindow *parent=GTK_WINDOW(gtk_window_new()); g_object_ref(parent);
    UmiBrokerAccessPlan plan; UmiBrokerWorkspaceGtk *workspace=NULL;
    CHECK(UmiBrokerAccessPlanCreate(UMI_BROKER_ACCESS_PAPER,&plan)==UMI_STATUS_OK);
    CHECK(UmiBrokerWorkspaceGtkCreate(parent,&plan,&workspace)==UMI_STATUS_OK);
    GtkWidget *root=UmiBrokerWorkspaceGtkWidget(workspace);g_object_ref(root);
    gtk_window_set_child(parent,root);
    GtkWidget *submit=Find(root,"broker-ticket-submit");
    CHECK(submit && !gtk_widget_get_sensitive(submit));
    if(!strcmp(argv[1],"ticket-default")) {
        g_signal_emit_by_name(Find(root,"broker-ticket-enable"),"clicked");
        g_signal_emit_by_name(Find(root,"broker-ticket-review"),"clicked");
        g_signal_emit_by_name(submit,"clicked");
        CHECK(!gtk_widget_get_sensitive(submit));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root,"broker-ticket-review-text"))),"No reviewed order"));
    } else if(!strcmp(argv[1],"pages")) {
        CHECK(UmiBrokerWorkspaceGtkSelectPage(workspace,UMI_BROKER_PAGE_ORDER_TICKET)==UMI_STATUS_OK);
        CHECK(UmiBrokerWorkspaceGtkMovePage(workspace,UMI_BROKER_PAGE_ORDER_TICKET,1U)==UMI_STATUS_OK);
        CHECK(UmiBrokerWorkspaceGtkSelectPage(workspace,UMI_BROKER_PAGE_WATCHLIST)==UMI_STATUS_OK);
        CHECK(UmiBrokerWorkspaceGtkMovePage(workspace,UMI_BROKER_PAGE_WATCHLIST,1U)==UMI_STATUS_OK);
        CHECK(Find(root,"broker-ticket-submit")==submit && !gtk_widget_get_sensitive(submit));
    } else if(!strcmp(argv[1],"watchlist") || !strcmp(argv[1],"watchlist-roundtrip") ||
              !strcmp(argv[1],"watchlist-malformed") || !strcmp(argv[1],"watchlist-oversized")) {
        GtkWidget *rows=Find(root,"broker-watchlist-rows");
        CHECK(rows);
        gtk_editable_set_text(GTK_EDITABLE(Find(root,"broker-workspace-contract")),"123");
        gtk_editable_set_text(GTK_EDITABLE(Find(root,"broker-workspace-exchange")),"SMART");
        gtk_editable_set_text(GTK_EDITABLE(Find(root,"broker-watchlist-label")),"Example share");
        g_signal_emit_by_name(Find(root,"broker-watchlist-add"),"clicked");
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows)))==1U);
        g_signal_emit_by_name(Find(root,"broker-watchlist-add"),"clicked");
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows)))==1U);
        char *directory=NULL, *file=NULL;
        if(strcmp(argv[1],"watchlist")) {
            directory=g_dir_make_tmp("umicom-watchlist-XXXXXX",NULL); CHECK(directory);
            file=g_build_filename(directory,"watchlist.ini",NULL);
            gtk_editable_set_text(GTK_EDITABLE(Find(root,"broker-watchlist-path")),file);
            g_signal_emit_by_name(Find(root,"broker-watchlist-save"),"clicked");
            CHECK(g_file_test(file,G_FILE_TEST_IS_REGULAR));
        }
        gtk_drop_down_set_selected(GTK_DROP_DOWN(rows),0U);
        g_signal_emit_by_name(Find(root,"broker-watchlist-remove"),"clicked");
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows)))==0U);
        if(file) {
            g_signal_emit_by_name(Find(root,"broker-watchlist-open"),"clicked");
            CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows)))==1U);
            GtkStringObject *item=g_list_model_get_item(gtk_drop_down_get_model(GTK_DROP_DOWN(rows)),0U);
            CHECK(item && strstr(gtk_string_object_get_string(item),"Example share"));
            g_object_unref(item);
            if(!strcmp(argv[1],"watchlist-malformed")) {
                CHECK(g_file_set_contents(file,"[Watchlist]\nversion=99\ncount=1\nname=Invalid\n",-1,NULL));
            } else if(!strcmp(argv[1],"watchlist-oversized")) {
                char *bytes=g_malloc0(65538U); memset(bytes,'x',65537U);
                CHECK(g_file_set_contents(file,bytes,65537,NULL));g_free(bytes);
            }
            if(strcmp(argv[1],"watchlist-roundtrip")) {
                g_signal_emit_by_name(Find(root,"broker-watchlist-open"),"clicked");
                CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows)))==1U);
                CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(root,"broker-watchlist-name"))),"My watchlist"));
            }
            /* Only this case's newly-created temporary file and directory are removed. */
            CHECK(g_remove(file)==0);CHECK(g_rmdir(directory)==0);
            g_free(file);g_free(directory);
        }
    } else if(!strcmp(argv[1],"retired")) {
        GtkWidget *add=Find(root,"broker-watchlist-add");
        g_object_ref(add);g_object_ref(submit);
        UmiBrokerWorkspaceGtkDestroy(workspace);workspace=NULL;
        g_signal_emit_by_name(add,"clicked");g_signal_emit_by_name(submit,"clicked");
        g_object_unref(add);g_object_unref(submit);
    } else return 2;
    UmiBrokerWorkspaceGtkDestroy(workspace);
    gtk_window_destroy(parent);g_object_unref(root);g_object_unref(parent);
    return 0;
}
