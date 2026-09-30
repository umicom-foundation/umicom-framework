/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_library_activation_gtk4.c
 * PURPOSE: Check native activation uses the queued, revision-checked library command.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/layout_library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); failed=1; goto cleanup;} } while(0)
typedef struct Fixture { UmiUiWorkspaceLibrarySnapshot snapshot; UmiGtk4WorkspaceLayoutLibrary *library; size_t applies; UmiStatus result; bool destroy; } Fixture;
static GtkWidget *Find(GtkWidget *root,const char *id)
{
    if(g_strcmp0(g_object_get_data(G_OBJECT(root),"umicom-automation-id"),id)==0)return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)){
        GtkWidget *found=Find(child,id);if(found!=NULL)return found;
    }return NULL;
}
static void Drain(void){for(size_t i=0;i<128 && g_main_context_pending(NULL);++i)(void)g_main_context_iteration(NULL,FALSE);}
static UmiStatus Read(UmiUiWorkspaceLibrarySnapshot *out,void *data){*out=((Fixture *)data)->snapshot;return UMI_STATUS_OK;}
static UmiStatus Apply(const UmiUiWorkspaceLibraryRequest *request,void *data)
{
    Fixture *f=data; ++f->applies;
    if(f->destroy){umi_gtk4_ws_layout_library_destroy(f->library);f->library=NULL;return UMI_STATUS_OK;}
    if(request->action!=UMI_UI_WORKSPACE_LIBRARY_ACTIVATE || request->expected_customisation_revision!=f->snapshot.customisation_revision)
        return f->result=UMI_STATUS_INVALID_STATE;
    for(size_t i=0;i<f->snapshot.layout_count;++i)f->snapshot.rows[i].active=strcmp(request->target_layout_id,f->snapshot.rows[i].layout_id)==0;
    ++f->snapshot.customisation_revision;return f->result=UMI_STATUS_OK;
}
int main(int argc,char **argv)
{
    /* The compressed guards obscured their independent scopes and triggered
     * misleading-indentation warnings. Explicit guards below replace them;
     * retain the original statement for engineering review. */
#if 0
    if(argc!=2)return 2;if(!gtk_init_check())return 77;
#endif
    /* Argument errors and an unavailable display keep their distinct exit
     * codes; GTK initialisation is attempted only for a valid invocation. */
    if (argc != 2) {
        return 2;
    }
    if (!gtk_init_check()) {
        return 77;
    }
    Fixture *f=calloc(1,sizeof *f); GtkWidget *root=NULL,*list=NULL; GtkListBoxRow *row=NULL; int failed=0;
    CHECK(f!=NULL);f->snapshot.layout_count=2;f->snapshot.customisation_revision=7;
    strcpy(f->snapshot.rows[0].layout_id,"test.first");strcpy(f->snapshot.rows[0].name,"First");f->snapshot.rows[0].active=true;
    strcpy(f->snapshot.rows[1].layout_id,"test.second");strcpy(f->snapshot.rows[1].name,"Second");
    CHECK(umi_gtk4_ws_layout_library_create(Read,Apply,f,&f->library)==UMI_STATUS_OK);
    root=g_object_ref(umi_gtk4_ws_layout_library_popover(f->library));list=Find(root,"workstation.layout-library.list");CHECK(GTK_IS_LIST_BOX(list));
    CHECK(!gtk_list_box_get_activate_on_single_click(GTK_LIST_BOX(list)));
    row=gtk_list_box_get_row_at_index(GTK_LIST_BOX(list),1);CHECK(row!=NULL);g_object_ref(row);
    gtk_list_box_select_row(GTK_LIST_BOX(list),row);Drain();CHECK(f->applies==0 && f->snapshot.rows[0].active);
    if(strcmp(argv[1],"editing")==0){f->snapshot.editing=true;CHECK(umi_gtk4_ws_layout_library_refresh(f->library)==UMI_STATUS_OK);}
    else if(strcmp(argv[1],"hidden")==0){GtkWidget *search=Find(root,"workstation.layout-library.search");CHECK(GTK_IS_SEARCH_ENTRY(search));gtk_editable_set_text(GTK_EDITABLE(search),"First");g_signal_emit_by_name(search,"search-changed");CHECK(!gtk_widget_get_child_visible(GTK_WIDGET(row)));}
    else if(strcmp(argv[1],"retained")==0){strcpy(f->snapshot.rows[1].name,"Rebuilt");CHECK(umi_gtk4_ws_layout_library_refresh(f->library)==UMI_STATUS_OK);CHECK(gtk_widget_get_parent(GTK_WIDGET(row))==NULL);}
    g_signal_emit_by_name(list,"row-activated",row);CHECK(f->applies==0);
    if(strcmp(argv[1],"pending")==0)g_signal_emit_by_name(list,"row-activated",row);
    else if(strcmp(argv[1],"stale")==0)++f->snapshot.customisation_revision;
    else if(strcmp(argv[1],"cancel")==0){umi_gtk4_ws_layout_library_destroy(f->library);f->library=NULL;}
    else if(strcmp(argv[1],"destroy")==0)f->destroy=true;
    else CHECK(strcmp(argv[1],"open")==0 || strcmp(argv[1],"editing")==0 || strcmp(argv[1],"hidden")==0 || strcmp(argv[1],"retained")==0);
    Drain();
    if(strcmp(argv[1],"editing")==0 || strcmp(argv[1],"hidden")==0 || strcmp(argv[1],"retained")==0 || strcmp(argv[1],"cancel")==0)CHECK(f->applies==0 && f->snapshot.rows[0].active);
    else {CHECK(f->applies==1);if(strcmp(argv[1],"stale")==0)CHECK(f->result==UMI_STATUS_INVALID_STATE && f->snapshot.rows[0].active);else if(!f->destroy)CHECK(f->result==UMI_STATUS_OK && f->snapshot.rows[1].active);}
    if(f->library==NULL){size_t calls=f->applies;g_signal_emit_by_name(list,"row-activated",row);Drain();CHECK(f->applies==calls);}
cleanup:
    if(f!=NULL)umi_gtk4_ws_layout_library_destroy(f->library);
    /* The compressed release sequence hid which operations were conditional.
     * Explicit scopes below preserve the release order and replace it; the
     * original sequence remains here for engineering review. */
#if 0
    if(row!=NULL)g_object_unref(row);if(root!=NULL)g_object_unref(root);free(f);return failed;
#endif
    /* Each retained GTK reference is released independently, including when
     * setup fails; fixture storage is always released before returning. */
    if (row != NULL) {
        g_object_unref(row);
    }
    if (root != NULL) {
        g_object_unref(root);
    }
    free(f);
    return failed;
}
