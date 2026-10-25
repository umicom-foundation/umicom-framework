/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_language_choice_gtk4.c
 *
 * PURPOSE:
 *   Check native language choice identity, invalid selections and caller-owned output semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/language_choice.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      failed = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1],
             *cases[] = {"create",    "select",          "unknown",      "unselected",
                         "arguments", "existing-output", "wrong-widget", "all-choices",
                         "model-lifetime", "retained-model"};
  int known = 0;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    known |= !strcmp(name, cases[i]);
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int failed = 0;
  GtkWidget *widget = NULL;
  CHECK(UmiGtk4LanguageChoiceCreate("c", &widget) == UMI_STATUS_OK);
  g_object_ref_sink(widget);
  const UmiDocumentLanguageChoice *choice = NULL;
  CHECK(UmiGtk4LanguageChoiceRead(widget, &choice) == UMI_STATUS_OK &&
        !strcmp(choice->language_id, "c"));
  if (!strcmp(name, "model-lifetime") || !strcmp(name, "retained-model")) {
    /* The dropdown owns its model. A caller may retain a separate reference,
     * but releasing the control must neither leak nor consume that reference. */
    GListModel *model = gtk_drop_down_get_model(GTK_DROP_DOWN(widget));
    gpointer observed = model;
    CHECK(model != NULL && g_list_model_get_n_items(model) == UmiDocumentLanguageChoiceCount());
    g_object_add_weak_pointer(G_OBJECT(model), &observed);
    int retained = !strcmp(name, "retained-model");
    if (retained)
      g_object_ref(model);
    g_clear_object(&widget);
    int lifetime_ok = retained ? observed == model : observed == NULL;
    if (retained) {
      lifetime_ok = lifetime_ok && g_list_model_get_n_items(model) == UmiDocumentLanguageChoiceCount();
      g_object_unref(model);
      lifetime_ok = lifetime_ok && observed == NULL;
    }
    /* Remove the stack address if a regression leaves the model alive. */
    if (observed != NULL)
      g_object_remove_weak_pointer(G_OBJECT(observed), &observed);
    CHECK(lifetime_ok);
  } else if (!strcmp(name, "select")) {
    CHECK(UmiGtk4LanguageChoiceSelect(widget, "sql") == UMI_STATUS_OK);
    CHECK(UmiGtk4LanguageChoiceRead(widget, &choice) == UMI_STATUS_OK &&
          !strcmp(choice->language_id, "sql"));
  } else if (!strcmp(name, "unknown")) {
    CHECK(UmiGtk4LanguageChoiceSelect(widget, "missing") == UMI_STATUS_NOT_FOUND);
    CHECK(UmiGtk4LanguageChoiceRead(widget, &choice) == UMI_STATUS_OK &&
          !strcmp(choice->language_id, "c"));
  } else if (!strcmp(name, "unselected")) {
    const UmiDocumentLanguageChoice *old = choice;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(widget), GTK_INVALID_LIST_POSITION);
    CHECK(UmiGtk4LanguageChoiceRead(widget, &choice) == UMI_STATUS_NOT_FOUND && choice == old);
  } else if (!strcmp(name, "arguments")) {
    CHECK(UmiGtk4LanguageChoiceCreate("c", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4LanguageChoiceRead(NULL, &choice) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4LanguageChoiceRead(widget, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4LanguageChoiceSelect(NULL, "c") == UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "existing-output")) {
    GtkWidget *old = widget;
    CHECK(UmiGtk4LanguageChoiceCreate("sql", &widget) == UMI_STATUS_INVALID_ARGUMENT &&
          widget == old);
  } else if (!strcmp(name, "wrong-widget")) {
    GtkWidget *label = gtk_label_new("other");
    g_object_ref_sink(label);
    UmiStatus status = UmiGtk4LanguageChoiceRead(label, &choice);
    g_object_unref(label);
    CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "all-choices"))
    for (size_t i = 0U; i < UmiDocumentLanguageChoiceCount(); ++i) {
      const UmiDocumentLanguageChoice *next = UmiDocumentLanguageChoiceAt(i);
      CHECK(UmiGtk4LanguageChoiceSelect(widget, next->language_id) == UMI_STATUS_OK);
      CHECK(UmiGtk4LanguageChoiceRead(widget, &choice) == UMI_STATUS_OK && choice == next);
    }
cleanup:
  if (widget)
    g_object_unref(widget);
  return failed;
}
