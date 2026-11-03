/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_modules.c
 *
 * PURPOSE:
 *   Present host-offered learning modules using copied routes and the shared reader.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "learning_catalogue_private.h"
#include <string.h>

typedef struct LearningModuleChoice {
    char directory[UMI_PATH_CAPACITY];
    char pack[UMI_TEACHER_ID_CAPACITY];
    char product[UMI_TEACHER_ID_CAPACITY];
    char resource[UMI_TEACHER_ID_CAPACITY];
} LearningModuleChoice;

static void ModuleOpen(GtkButton *button, gpointer data)
{
    GtkWidget *widget = g_object_ref(data);
    const LearningModuleChoice *choice = g_object_get_data(G_OBJECT(button), "learning-module-choice");
    UmiStatus status = choice == NULL ? UMI_STATUS_INVALID_STATE :
        UmiLearningCatalogueGtk4OpenModule(widget, choice->directory,
            choice->pack, choice->product, choice->resource);
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner != NULL && !owner->state.retired && status != UMI_STATUS_OK)
        LearningCatalogueMessage(owner, status, "The learning module could not be opened. Check its installed folder or choose a library.");
    g_object_unref(widget);
}

UmiStatus UmiLearningCatalogueGtk4OfferModule(GtkWidget *widget, const char *title,
    const char *description, const char *directory, const char *pack_id,
    const char *product, const char *resource_id)
{
    LearningCatalogue *owner = LearningCatalogueOwner(widget);
    if (owner == NULL || title == NULL || description == NULL || directory == NULL ||
        pack_id == NULL || product == NULL || resource_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->state.retired) return UMI_STATUS_INVALID_STATE;
    if (owner->state.busy) return UMI_STATUS_BUSY;
    if (!umi_path_is_absolute(directory) || !g_utf8_validate(title, -1, NULL) ||
        !g_utf8_validate(description, -1, NULL) || !g_utf8_validate(directory, -1, NULL) ||
        pack_id[0] == '\0' || product[0] == '\0' || resource_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(title) > 240U || strlen(description) > 1024U || strlen(directory) >= UMI_PATH_CAPACITY ||
        strlen(pack_id) >= UMI_TEACHER_ID_CAPACITY || strlen(product) >= UMI_TEACHER_ID_CAPACITY ||
        strlen(resource_id) >= UMI_TEACHER_ID_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    char *key = g_strconcat("learning-module-", pack_id, NULL);
    if (g_object_get_data(G_OBJECT(widget), key) != NULL) { g_free(key); return UMI_STATUS_ALREADY_EXISTS; }
    if (owner->module_count >= UMI_LEARNING_SHELF_MAX_PACKS) { g_free(key); return UMI_STATUS_CAPACITY_EXCEEDED; }
    LearningModuleChoice *choice = g_try_new0(LearningModuleChoice, 1U);
    if (choice == NULL) { g_free(key); return UMI_STATUS_OUT_OF_MEMORY; }
    g_strlcpy(choice->directory, directory, sizeof(choice->directory));
    g_strlcpy(choice->pack, pack_id, sizeof(choice->pack));
    g_strlcpy(choice->product, product, sizeof(choice->product));
    g_strlcpy(choice->resource, resource_id, sizeof(choice->resource));
    /* Mark registration before widget notifications; a reentrant host cannot
     * insert a duplicate route. The root owns the controls and their copies. */
    g_object_ref(widget);
    owner->state.busy = 1;
    g_object_set_data(G_OBJECT(widget), key, GINT_TO_POINTER(1));
    ++owner->module_count;
    g_free(key);
    int attach_choices = owner->module_choices == NULL;
    if (attach_choices) {
        owner->module_choices = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *heading = gtk_label_new("Learning modules");
        gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
        gtk_box_append(GTK_BOX(owner->module_choices), heading);
        /* Attach only after the complete row exists; attachment may retire the host. */
    }
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    GtkWidget *button = gtk_button_new_with_label(title);
    GtkWidget *detail = gtk_label_new(description);
    gtk_label_set_wrap(GTK_LABEL(detail), TRUE);
    gtk_label_set_xalign(GTK_LABEL(detail), 0.0F);
    g_object_set_data_full(G_OBJECT(button), "learning-module-choice", choice, g_free);
    char *tag = g_strconcat("learning.module.", pack_id, NULL);
    g_object_set_data_full(G_OBJECT(button), "umicom-automation-id", tag, g_free);
    g_signal_connect_object(button, "clicked", G_CALLBACK(ModuleOpen), widget, 0);
    gtk_box_append(GTK_BOX(row), button);
    gtk_box_append(GTK_BOX(row), detail);
    gtk_box_append(GTK_BOX(owner->module_choices), row);
    if (attach_choices) gtk_box_prepend(GTK_BOX(owner->controls), owner->module_choices);
    owner->state.busy = 0;
    g_object_unref(widget);
    return UMI_STATUS_OK;
}
