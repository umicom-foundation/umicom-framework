/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_body_controls.c
 *
 * PURPOSE:
 *   Present explicit body-search, refresh and verified result-reading controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_catalogue_private.h"
#include "umicom/ui/gtk4/drop_down.h"
#include <string.h>

static void BodyTag(GtkWidget *widget,const char *name)
{
    g_object_set_data_full(G_OBJECT(widget),"umicom-automation-id",g_strdup(name),g_free);
}
static UmiStatus BodyQueryControls(LearningCatalogue *owner, UmiLearningTextQuery *query)
{
    UmiLearningTextQueryInit(query);
    query->maximum_results = 128U;
    /* Entry limits count characters, while API arrays count UTF-8 bytes.
     * Refuse over-capacity input instead of silently truncating a filter. */
    const char *phrase = gtk_editable_get_text(GTK_EDITABLE(owner->text_query));
    const char *product = gtk_editable_get_text(GTK_EDITABLE(owner->text_product));
    const char *locale = gtk_editable_get_text(GTK_EDITABLE(owner->text_locale));
    if (g_strlcpy(query->phrase, phrase, sizeof(query->phrase)) >= sizeof(query->phrase) ||
        g_strlcpy(query->product, product, sizeof(query->product)) >= sizeof(query->product) ||
        g_strlcpy(query->locale, locale, sizeof(query->locale)) >= sizeof(query->locale))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    query->case_sensitive = gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->text_case));
    query->whole_word = gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->text_word));
    static const unsigned kinds[] = {0U, UMI_LEARNING_RESOURCE_LESSON,
        UMI_LEARNING_RESOURCE_GUIDE, UMI_LEARNING_RESOURCE_EXAMPLE};
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->text_kind));
    if ((size_t)selected >= sizeof(kinds)/sizeof(kinds[0])) return UMI_STATUS_INVALID_ARGUMENT;
    query->kind = kinds[selected];
    return UmiLearningTextQueryValidate(query);
}
static GtkWidget *BodyFilterEntry(GtkWidget *box, const char *label_text,
    const char *placeholder, const char *id, int maximum)
{
    GtkWidget *label = gtk_label_new(label_text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
    gtk_entry_set_max_length(GTK_ENTRY(entry), maximum);
    BodyTag(entry, id);
    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), entry);
    return entry;
}

/* Preserve unfiltered button dispatch for review. The current controls capture
 * a bounded query and guard error-notice callbacks as well as accepted jobs. */
#if 0
static void BodyAction(GtkButton *button,gpointer data)
{
    GtkWidget *root=GTK_WIDGET(data);
    g_object_ref(root);
    LearningCatalogue *owner=LearningCatalogueOwner(root);
    if(owner!=NULL && !owner->state.retired) {
        const char *action=g_object_get_data(G_OBJECT(button),"learning-body-action");
        UmiStatus status;
        if(strcmp(action,"read")==0) {
            guint selected=gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->text_rows));
            status=selected==GTK_INVALID_LIST_POSITION ? UMI_STATUS_NOT_FOUND :
                UmiLearningCatalogueGtk4ReadBodySearchResult(root,(size_t)selected);
        } else {
            status=UmiLearningCatalogueGtk4SearchBodies(root,
                gtk_editable_get_text(GTK_EDITABLE(owner->text_query)),
                gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->text_case)),
                gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->text_word)),
                strcmp(action,"refresh")==0);
        }
        if(status!=UMI_STATUS_OK && !owner->state.retired)
            LearningCatalogueMessage(owner,status,"The body-search action was not accepted.");
    }
    g_object_unref(root);
}
#endif
static void BodyAction(GtkButton *button,gpointer data)
{
    GtkWidget *root=GTK_WIDGET(data);
    g_object_ref(root);
    LearningCatalogue *owner=LearningCatalogueOwner(root);
    if(owner!=NULL && !owner->state.retired && !owner->state.busy) {
        const char *action=g_object_get_data(G_OBJECT(button),"learning-body-action");
        UmiStatus status;
        if(strcmp(action,"read")==0) {
            guint selected=gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->text_rows));
            status=selected==GTK_INVALID_LIST_POSITION ? UMI_STATUS_NOT_FOUND :
                UmiLearningCatalogueGtk4ReadBodySearchResult(root,(size_t)selected);
        } else {
            UmiLearningTextQuery query;
            status=BodyQueryControls(owner,&query);
            if(status==UMI_STATUS_OK)
                status=UmiLearningCatalogueGtk4SearchBodiesWithQuery(root,&query,strcmp(action,"refresh")==0);
        }
        if(status!=UMI_STATUS_OK && !owner->state.retired && !owner->state.busy) {
            /* Error notices are observable too. Keep callbacks from starting a
             * second mutation until this notice has finished publication. */
            owner->state.busy=1;
            LearningCatalogueMessage(owner,status,"The body-search action was not accepted. Check the phrase and filters.");
            owner->state.busy=0;
        }
    }
    g_object_unref(root);
}
static void BodyButton(LearningCatalogue *owner,GtkWidget *box,const char *label,const char *action)
{
    GtkWidget *button=gtk_button_new_with_label(label);
    char *id=g_strdup_printf("learning.catalogue.body-%s",action);
    BodyTag(button,id);g_free(id);
    g_object_set_data(G_OBJECT(button),"learning-body-action",(gpointer)action);
    g_signal_connect_object(button,"clicked",G_CALLBACK(BodyAction),owner->root,0);
    gtk_box_append(GTK_BOX(box),button);
}
void LearningCatalogueBodyControls(LearningCatalogue *owner)
{
    GtkWidget *expander=gtk_expander_new("Search inside guides and examples");
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
    GtkWidget *explanation=gtk_label_new(
        "Search builds a verified text snapshot when needed. Refresh search rereads files. "
        "Later queries reuse the snapshot. Read result verifies the file and selects its first match.");
    gtk_label_set_wrap(GTK_LABEL(explanation),TRUE);
    gtk_label_set_xalign(GTK_LABEL(explanation),0.0F);
    gtk_box_append(GTK_BOX(box),explanation);
    owner->text_query=gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(owner->text_query),(int)UMI_LEARNING_TEXT_QUERY_CAPACITY-1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(owner->text_query),"Phrase inside a guide or source example");
    BodyTag(owner->text_query,"learning.catalogue.body-query");
    gtk_box_append(GTK_BOX(box),owner->text_query);
    owner->text_product=BodyFilterEntry(box,"Product ID (optional, exact)","For example: studio",
        "learning.catalogue.body-product",(int)UMI_TEACHER_ID_CAPACITY-1);
    owner->text_locale=BodyFilterEntry(box,"Language tag (optional)","For example: en-GB",
        "learning.catalogue.body-locale",(int)UMI_LEARNING_PACK_LOCALE_CAPACITY-1);
    GtkWidget *kind_label=gtk_label_new("Resource kind");
    gtk_label_set_xalign(GTK_LABEL(kind_label),0.0F);
    gtk_box_append(GTK_BOX(box),kind_label);
    const char *kind_names[]={"All readable resources","Lessons","Guides","Examples",NULL};
    owner->text_kind=umi_ui_gtk4_drop_down_new_take_string_list(gtk_string_list_new(kind_names));
    gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->text_kind),0U);
    BodyTag(owner->text_kind,"learning.catalogue.body-kind");
    gtk_box_append(GTK_BOX(box),owner->text_kind);
    owner->text_case=gtk_check_button_new_with_label("Match case");
    owner->text_word=gtk_check_button_new_with_label("Whole word");
    BodyTag(owner->text_case,"learning.catalogue.body-case");
    BodyTag(owner->text_word,"learning.catalogue.body-word");
    gtk_box_append(GTK_BOX(box),owner->text_case);
    gtk_box_append(GTK_BOX(box),owner->text_word);
    GtkWidget *buttons=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
    BodyButton(owner,buttons,"Search text","search");
    BodyButton(owner,buttons,"Refresh search","refresh");
    gtk_box_append(GTK_BOX(box),buttons);
    owner->text_rows=umi_ui_gtk4_drop_down_new_take_string_list(gtk_string_list_new(NULL));
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(owner->text_rows),TRUE);
    BodyTag(owner->text_rows,"learning.catalogue.body-results");
    gtk_box_append(GTK_BOX(box),owner->text_rows);
    BodyButton(owner,box,"Read result","read");
    owner->text_status=gtk_label_new("No body-search snapshot yet.");
    gtk_label_set_wrap(GTK_LABEL(owner->text_status),TRUE);
    gtk_label_set_selectable(GTK_LABEL(owner->text_status),TRUE);
    gtk_label_set_xalign(GTK_LABEL(owner->text_status),0.0F);
    BodyTag(owner->text_status,"learning.catalogue.body-status");
    gtk_box_append(GTK_BOX(box),owner->text_status);
    gtk_expander_set_child(GTK_EXPANDER(expander),box);
    gtk_box_append(GTK_BOX(owner->controls),expander);
}
