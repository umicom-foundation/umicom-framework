/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/declarative_renderer_gtk4.c
 *
 * PURPOSE:
 *   Render the initial semantic component set into GTK4 widgets while keeping GTK types inside the adapter boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is a presentation adapter.  GTK widgets are created here, while
 * authoritative application/designer state remains in the shared Framework.
 */

#include <gtk/gtk.h>
#include <string.h>
#include "umicom/declarative/renderer.h"

typedef struct GtkDeclContext { GtkWidget *root; GHashTable *widgets; } GtkDeclContext;
/* Provide the create widget operation used by this module and its client applications. */
static GtkWidget *create_widget(const UmiDeclNode *node)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(node->component_type,"window")==0) return gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(node->component_type,"split")==0) return gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (strcmp(node->component_type,"tabs")==0) return gtk_notebook_new();
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(node->component_type,"button")==0) return gtk_button_new_with_label(node->node_id);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(node->component_type,"label")==0) return gtk_label_new(node->node_id);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(node->component_type,"text")==0) return gtk_entry_new();
    return gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
}
/* Provide the begin render operation used by this module and its client applications. */
static UmiStatus begin_render(void *instance,const UmiDeclApplicationPlan *plan){GtkDeclContext *ctx=(GtkDeclContext *)instance;(void)plan;ctx->widgets=g_hash_table_new_full(g_str_hash,g_str_equal,g_free,NULL);return ctx->widgets!=NULL?UMI_STATUS_OK:UMI_STATUS_OUT_OF_MEMORY;}
/* Provide the component render operation used by this module and its client applications. */
static UmiStatus component_render(void *instance,const UmiDeclNode *node){GtkDeclContext *ctx=(GtkDeclContext *)instance;GtkWidget *widget,*parent;widget=create_widget(node);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(widget==NULL)return UMI_STATUS_OUT_OF_MEMORY;g_hash_table_insert(ctx->widgets,g_strdup(node->node_id),widget);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(node->parent_id[0]=='\0'){ctx->root=widget;return UMI_STATUS_OK;}parent=g_hash_table_lookup(ctx->widgets,node->parent_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(parent==NULL)return UMI_STATUS_NOT_FOUND;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(GTK_IS_BOX(parent))gtk_box_append(GTK_BOX(parent),widget);else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(GTK_IS_PANED(parent)){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(gtk_paned_get_start_child(GTK_PANED(parent))==NULL)gtk_paned_set_start_child(GTK_PANED(parent),widget);/* Use this fallback path when the earlier condition does not apply. */ else gtk_paned_set_end_child(GTK_PANED(parent),widget);}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(GTK_IS_NOTEBOOK(parent))gtk_notebook_append_page(GTK_NOTEBOOK(parent),widget,NULL);return UMI_STATUS_OK;}
/* Provide the end render operation used by this module and its client applications. */
static UmiStatus end_render(void *instance){(void)instance;return UMI_STATUS_OK;}
/* Provide the destroy render operation used by this module and its client applications. */
static void destroy_render(void *instance){GtkDeclContext *ctx=(GtkDeclContext *)instance;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ctx!=NULL&&ctx->widgets!=NULL){g_hash_table_destroy(ctx->widgets);ctx->widgets=NULL;}}
/*
 * Initialise decl gtk4 renderer from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_decl_gtk4_renderer_init(UmiDeclRenderer *renderer,void *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(renderer==NULL||context==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    renderer->structure_size=(uint32_t)sizeof(*renderer);renderer->abi_version=1U;renderer->instance=context;renderer->begin=begin_render;renderer->component=component_render;renderer->end=end_render;renderer->destroy=destroy_render;return UMI_STATUS_OK;
}
