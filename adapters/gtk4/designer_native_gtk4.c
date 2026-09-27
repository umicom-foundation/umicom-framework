/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/designer_native_gtk4.c
 *
 * PURPOSE:
 *   Build real GTK controls from the canonical native designer profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/native_gtk4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "desktop_system_brand.inc"

#define STATE_KEY "umicom-designer-native-state"
#define EDITOR_CHARACTER_LIMIT 262144

typedef struct NativeState {
    size_t count;
    char ids[UMI_DESIGNER_NATIVE_NODE_LIMIT][UMI_DECL_ID_CAPACITY];
    GWeakRef controls[UMI_DESIGNER_NATIVE_NODE_LIMIT];
} NativeState;

typedef struct CountAction {
    GWeakRef window;
    size_t source;
    size_t target;
} CountAction;

static const UmiDeclValue *Property(const UmiDeclNode *node, const char *name)
{
    for(size_t i=0U;i<node->attribute_count;++i)
        if(strcmp(node->attributes[i].name,name)==0) return &node->attributes[i].value;
    return NULL;
}
static const char *Text(const UmiDeclNode *node,const char *name,const char *fallback)
{ const UmiDeclValue *v=Property(node,name); return v!=NULL ? v->text : fallback; }
static int Integer(const UmiDeclNode *node,const char *name,int fallback)
{ const UmiDeclValue *v=Property(node,name); return v!=NULL ? (int)v->integer_value : fallback; }
static gboolean Boolean(const UmiDeclNode *node,const char *name,gboolean fallback)
{ const UmiDeclValue *v=Property(node,name); return v!=NULL ? v->boolean_value!=0 : fallback; }
static int Is(const UmiDeclNode *node,const char *type)
{ return strcmp(node->component_type,type)==0; }
static GtkOrientation Orientation(const UmiDeclNode *node,const char *fallback)
{ return strcmp(Text(node,"orientation",fallback),"horizontal")==0 ? GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL; }

static void StateDestroy(gpointer data)
{
    NativeState *state=data;
    if(state==NULL) return;
    for(size_t i=0U;i<state->count;++i) g_weak_ref_clear(&state->controls[i]);
    free(state);
}
static void ActionDestroy(gpointer data,GClosure *closure)
{
    CountAction *action=data;
    (void)closure;
    if(action!=NULL) { g_weak_ref_clear(&action->window); free(action); }
}
static void CountClicked(GtkButton *button,gpointer data)
{
    CountAction *action=data;
    GObject *window=g_weak_ref_get(&action->window);
    GObject *source=NULL,*target=NULL;
    if(window==NULL) return;
    NativeState *state=g_object_get_data(window,STATE_KEY);
    if(state==NULL || action->source>=state->count || action->target>=state->count ||
        (gpointer)gtk_widget_get_root(GTK_WIDGET(button))!=(gpointer)window) goto finish;
    source=g_weak_ref_get(&state->controls[action->source]);
    target=g_weak_ref_get(&state->controls[action->target]);
    if(source==NULL || target==NULL || !GTK_IS_LABEL(target) ||
        (gpointer)gtk_widget_get_root(GTK_WIDGET(source))!=(gpointer)window ||
        (gpointer)gtk_widget_get_root(GTK_WIDGET(target))!=(gpointer)window) goto finish;
    glong count;
    if(GTK_IS_TEXT_VIEW(source)) {
        GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(source));
        count=(glong)gtk_text_buffer_get_char_count(buffer);
    } else if(GTK_IS_EDITABLE(source)) {
        count=g_utf8_strlen(gtk_editable_get_text(GTK_EDITABLE(source)),-1);
    } else goto finish;
    char message[96];
    (void)snprintf(message,sizeof message,"%ld Unicode character%s",(long)count,count==1 ? "" : "s");
    gtk_label_set_text(GTK_LABEL(target),message);
finish:
    g_clear_object(&source); g_clear_object(&target); g_object_unref(window);
}

/* The insert iterator is not modified. Refusing the default handler leaves
 * both text and iterator valid; no large temporary copy of the draft is made. */
static void LimitEditor(GtkTextBuffer *buffer,const GtkTextIter *where,
    gchar *text,gint length,gpointer data)
{
    (void)where; (void)data;
    gint present=gtk_text_buffer_get_char_count(buffer);
    glong incoming=g_utf8_strlen(text,length);
    if(incoming<0 || present>EDITOR_CHARACTER_LIMIT ||
        incoming>(glong)(EDITOR_CHARACTER_LIMIT-present))
        g_signal_stop_emission_by_name(buffer,"insert-text");
}

static GtkWidget *BrandImage(const unsigned char *data,size_t length,int width,int height)
{
    GBytes *bytes=g_bytes_new_static(data,length);
    GError *error=NULL;
    GdkTexture *texture=gdk_texture_new_from_bytes(bytes,&error);
    g_bytes_unref(bytes);
    if(texture==NULL) { g_clear_error(&error); return gtk_label_new("Umicom"); }
    GtkWidget *picture=gtk_picture_new_for_paintable(GDK_PAINTABLE(texture));
    g_object_unref(texture);
    gtk_picture_set_can_shrink(GTK_PICTURE(picture),TRUE);
    gtk_widget_set_size_request(picture,width,height);
    gtk_widget_set_halign(picture,GTK_ALIGN_START);
    return picture;
}

static size_t StateIndex(const NativeState *state,const char *id)
{
    for(size_t i=0U;i<state->count;++i) if(strcmp(state->ids[i],id)==0) return i;
    return SIZE_MAX;
}
static void DropUnparented(GtkWidget *widget)
{ if(widget!=NULL) { g_object_ref_sink(widget); g_object_unref(widget); } }

static GtkWidget *BuildNode(NativeState *state,const UmiDeclNode *nodes,
    size_t index,GtkWindow *window,UmiStatus *status)
{
    const UmiDeclNode *node=&nodes[index];
    GtkWidget *widget=NULL,*control=NULL;
    if(Is(node,"pane")) widget=gtk_box_new(Orientation(node,"vertical"),Integer(node,"spacing",8));
    else if(Is(node,"split")) widget=gtk_paned_new(Orientation(node,"horizontal"));
    else if(Is(node,"tabs")) {
        widget=gtk_notebook_new(); gtk_notebook_set_scrollable(GTK_NOTEBOOK(widget),TRUE);
    } else if(Is(node,"label")) {
        widget=gtk_label_new(Text(node,"text",Text(node,"title","")));
        gtk_label_set_wrap(GTK_LABEL(widget),TRUE); gtk_label_set_xalign(GTK_LABEL(widget),0.0F);
    } else if(Is(node,"text")) {
        widget=gtk_entry_new(); gtk_entry_set_max_length(GTK_ENTRY(widget),4096);
        gtk_editable_set_text(GTK_EDITABLE(widget),Text(node,"text",""));
        gtk_entry_set_placeholder_text(GTK_ENTRY(widget),Text(node,"placeholder",""));
    } else if(Is(node,"editor")) {
        control=gtk_text_view_new();
        GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(control));
        gtk_text_buffer_set_text(buffer,Text(node,"text",""),-1);
        g_signal_connect(buffer,"insert-text",G_CALLBACK(LimitEditor),NULL);
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(control),Boolean(node,"wrap",TRUE) ? GTK_WRAP_WORD_CHAR : GTK_WRAP_NONE);
        gtk_text_view_set_editable(GTK_TEXT_VIEW(control),!Boolean(node,"read_only",FALSE));
        widget=gtk_scrolled_window_new(); gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(widget),control);
        gtk_widget_set_vexpand(widget,TRUE); gtk_widget_set_hexpand(widget,TRUE);
    } else if(Is(node,"button")) widget=gtk_button_new_with_label(Text(node,"title",node->node_id));
    else { *status=UMI_STATUS_NOT_IMPLEMENTED; return NULL; }
    if(control==NULL) control=widget;
    gtk_widget_set_name(control,node->node_id);
    g_weak_ref_set(&state->controls[index],G_OBJECT(control));
    gtk_widget_set_visible(widget,Boolean(node,"visible",TRUE));
    gtk_widget_set_sensitive(widget,Boolean(node,"enabled",TRUE));
    gtk_widget_set_tooltip_text(control,Text(node,"tooltip",NULL));
    size_t childNumber=0U;
    for(size_t i=0U;i<state->count;++i) {
        if(strcmp(nodes[i].parent_id,node->node_id)!=0) continue;
        GtkWidget *child=BuildNode(state,nodes,i,window,status);
        if(child==NULL) { DropUnparented(widget); return NULL; }
        if(Is(node,"pane")) gtk_box_append(GTK_BOX(widget),child);
        else if(Is(node,"split")) {
            if(childNumber==0U) gtk_paned_set_start_child(GTK_PANED(widget),child);
            else gtk_paned_set_end_child(GTK_PANED(widget),child);
        } else if(Is(node,"tabs")) {
            GtkWidget *label=gtk_label_new(Text(&nodes[i],"title",nodes[i].node_id));
            if(gtk_notebook_append_page(GTK_NOTEBOOK(widget),child,label)<0) {
                DropUnparented(child); DropUnparented(label); DropUnparented(widget);
                *status=UMI_STATUS_INTERNAL_ERROR; return NULL;
            }
        } else { DropUnparented(child); DropUnparented(widget); *status=UMI_STATUS_INVALID_STATE; return NULL; }
        ++childNumber;
    }
    if(Is(node,"split")) gtk_paned_set_position(GTK_PANED(widget),Integer(node,"position",240));
    if(Is(node,"button")) {
        if(Property(node,"action")==NULL) {
            gtk_widget_set_sensitive(widget,FALSE);
            gtk_widget_set_tooltip_text(widget,"No action is assigned to this button.");
        } else {
            CountAction *action=calloc(1U,sizeof *action);
            if(action==NULL) { DropUnparented(widget); *status=UMI_STATUS_OUT_OF_MEMORY; return NULL; }
            g_weak_ref_init(&action->window,G_OBJECT(window));
            action->source=StateIndex(state,Text(node,"source",""));
            action->target=StateIndex(state,Text(node,"target",""));
            g_signal_connect_data(widget,"clicked",G_CALLBACK(CountClicked),action,ActionDestroy,0);
        }
    }
    return widget;
}

UmiStatus UmiDesignerNativeGtkCreate(const UmiDeclDocument *document,GtkWindow **outWindow)
{
    UmiDeclDocumentSnapshot snapshot;
    NativeState *state=NULL;
    UmiDeclNode *nodes=NULL;
    UmiStatus status;
    GtkWindow *window=NULL;
    size_t root=SIZE_MAX,content=SIZE_MAX;
    if(outWindow==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outWindow=NULL;
    status=UmiDesignerNativeValidate(document,NULL,0U);
    if(status!=UMI_STATUS_OK) return status;
    if(gdk_display_get_default()==NULL) return UMI_STATUS_UNAVAILABLE;
    status=umi_decl_document_snapshot(document,&snapshot);
    if(status!=UMI_STATUS_OK) return status;
    state=calloc(1U,sizeof *state);
    nodes=calloc(snapshot.node_count,sizeof *nodes);
    if(state==NULL || nodes==NULL) { free(state); free(nodes); return UMI_STATUS_OUT_OF_MEMORY; }
    state->count=snapshot.node_count;
    for(size_t i=0U;i<state->count;++i) g_weak_ref_init(&state->controls[i],NULL);
    for(size_t i=0U;i<state->count;++i) {
        status=umi_decl_document_node_at(document,i,&nodes[i]);
        if(status!=UMI_STATUS_OK) goto failure;
        (void)umi_decl_copy_text(state->ids[i],sizeof state->ids[i],nodes[i].node_id);
        if(nodes[i].parent_id[0]=='\0') root=i;
    }
    if(root==SIZE_MAX) { status=UMI_STATUS_INVALID_STATE; goto failure; }
    for(size_t i=0U;i<state->count;++i) if(strcmp(nodes[i].parent_id,nodes[root].node_id)==0) { content=i; break; }
    if(content==SIZE_MAX) { status=UMI_STATUS_INVALID_STATE; goto failure; }
    window=GTK_WINDOW(gtk_window_new());
    /* gtk_window_new is owned by GTK's top-level list; take our API reference. */
    g_object_ref(window);
    g_object_set_data_full(G_OBJECT(window),STATE_KEY,state,StateDestroy);
    g_weak_ref_set(&state->controls[root],G_OBJECT(window));
    gtk_window_set_title(window,Text(&nodes[root],"title",snapshot.application_id));
    gtk_window_set_default_size(window,Integer(&nodes[root],"width",800),Integer(&nodes[root],"height",600));
    gtk_widget_set_sensitive(GTK_WIDGET(window),Boolean(&nodes[root],"enabled",TRUE));
    gtk_widget_set_tooltip_text(GTK_WIDGET(window),Text(&nodes[root],"tooltip",NULL));
    GtkWidget *outer=gtk_box_new(GTK_ORIENTATION_VERTICAL,12);
    gtk_window_set_child(window,outer);
    gtk_widget_set_margin_start(outer,16); gtk_widget_set_margin_end(outer,16);
    gtk_widget_set_margin_top(outer,16); gtk_widget_set_margin_bottom(outer,16);
    GtkWidget *branding=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,12);
    gtk_box_append(GTK_BOX(branding),BrandImage(UMICOM_SYSTEM_ICON,sizeof UMICOM_SYSTEM_ICON,32,32));
    gtk_box_append(GTK_BOX(branding),BrandImage(UMICOM_SYSTEM_LOGO,sizeof UMICOM_SYSTEM_LOGO,180,38));
    gtk_box_append(GTK_BOX(outer),branding);
    GtkWidget *body=BuildNode(state,nodes,content,window,&status);
    if(body==NULL) goto failure;
    gtk_box_append(GTK_BOX(outer),body);
    free(nodes); *outWindow=window; return UMI_STATUS_OK;
failure:
    free(nodes);
    if(window!=NULL) { gtk_window_destroy(window); g_object_unref(window); }
    else StateDestroy(state);
    return status;
}

GtkWidget *UmiDesignerNativeGtkRefControl(GtkWindow *window,const char *nodeId)
{
    if(window==NULL || nodeId==NULL) return NULL;
    NativeState *state=g_object_get_data(G_OBJECT(window),STATE_KEY);
    if(state==NULL) return NULL;
    size_t i=StateIndex(state,nodeId);
    if(i==SIZE_MAX) return NULL;
    GObject *object=g_weak_ref_get(&state->controls[i]);
    if(object==NULL) return NULL;
    if(object!=G_OBJECT(window) && (gpointer)gtk_widget_get_root(GTK_WIDGET(object))!=(gpointer)window) {
        g_object_unref(object); return NULL;
    }
    if(object==G_OBJECT(window) && gtk_window_get_child(window)==NULL) { g_object_unref(object); return NULL; }
    return GTK_WIDGET(object);
}
static gboolean CloseLoop(GtkWindow *window,gpointer data)
{ (void)window; g_main_loop_quit(data); return TRUE; }
UmiStatus UmiDesignerNativeGtkRun(const UmiDeclDocument *document)
{
    GtkWindow *window=NULL;
    if(!gtk_init_check()) return UMI_STATUS_UNAVAILABLE;
    UmiStatus status=UmiDesignerNativeGtkCreate(document,&window);
    if(status!=UMI_STATUS_OK) return status;
    GMainLoop *loop=g_main_loop_new(NULL,FALSE);
    gulong handler=g_signal_connect(window,"close-request",G_CALLBACK(CloseLoop),loop);
    gtk_window_present(window);
    g_main_loop_run(loop);
    g_signal_handler_disconnect(window,handler);
    gtk_window_destroy(window); g_object_unref(window); g_main_loop_unref(loop);
    return UMI_STATUS_OK;
}
