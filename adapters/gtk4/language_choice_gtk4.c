/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/language_choice_gtk4.c
 *
 * PURPOSE:
 *   Bind the document language catalogue to GTK without duplicating language identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/language_choice.h"
#include "umicom/ui/gtk4/drop_down.h"
#include <string.h>
static int LanguageChoiceWidget(GtkWidget *widget) {
  return widget != NULL && GTK_IS_DROP_DOWN(widget) &&
         g_object_get_data(G_OBJECT(widget), "umicom-language-choice") != NULL;
}
UmiStatus UmiGtk4LanguageChoiceRead(GtkWidget *widget, const UmiDocumentLanguageChoice **out) {
  if (!LanguageChoiceWidget(widget) || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  const UmiDocumentLanguageChoice *choice =
      UmiDocumentLanguageChoiceAt(gtk_drop_down_get_selected(GTK_DROP_DOWN(widget)));
  if (choice == NULL)
    return UMI_STATUS_NOT_FOUND;
  *out = choice;
  return UMI_STATUS_OK;
}
UmiStatus UmiGtk4LanguageChoiceSelect(GtkWidget *widget, const char *id) {
  if (!LanguageChoiceWidget(widget))
    return UMI_STATUS_INVALID_ARGUMENT;
  const UmiDocumentLanguageChoice *choice = NULL;
  UmiStatus status = UmiDocumentLanguageChoiceFind(id, &choice);
  if (status != UMI_STATUS_OK)
    return status;
  for (size_t i = 0U; i < UmiDocumentLanguageChoiceCount(); ++i)
    if (UmiDocumentLanguageChoiceAt(i) == choice) {
      gtk_drop_down_set_selected(GTK_DROP_DOWN(widget), (guint)i);
      return UMI_STATUS_OK;
    }
  return UMI_STATUS_NOT_FOUND;
}
UmiStatus UmiGtk4LanguageChoiceCreate(const char *id, GtkWidget **out) {
  if (out == NULL || *out != NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  const UmiDocumentLanguageChoice *choice = NULL;
  UmiStatus status = UmiDocumentLanguageChoiceFind(id, &choice);
  if (status != UMI_STATUS_OK)
    return status;
  GtkStringList *model = gtk_string_list_new(NULL);
  guint selected = 0U;
  for (size_t i = 0U; i < UmiDocumentLanguageChoiceCount(); ++i) {
    const UmiDocumentLanguageChoice *item = UmiDocumentLanguageChoiceAt(i);
    gtk_string_list_append(model, item->display_name);
    if (item == choice)
      selected = (guint)i;
  }
  /* GtkDropDown takes ownership of the supplied model. The choice catalogue
   * remains static; only its display strings are copied into GTK. */
  /* Centralise GTK model transfer in the shared ownership helper. This retains
   * the same catalogue and selected row; the earlier call stays for review. */
#if 0
  GtkWidget *widget = gtk_drop_down_new(G_LIST_MODEL(model), NULL);
#endif
  GtkWidget *widget = umi_ui_gtk4_drop_down_new_take_string_list(model);
  g_object_set_data(G_OBJECT(widget), "umicom-language-choice", GINT_TO_POINTER(1));
  gtk_widget_set_name(widget, "document.language.choice");
  gtk_drop_down_set_selected(GTK_DROP_DOWN(widget), selected);
  *out = widget;
  return UMI_STATUS_OK;
}
