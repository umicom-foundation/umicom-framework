/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_indentation_preferences_gtk4.c
 *
 * PURPOSE:
 *   Check native indentation draft controls without publishing settings or editing a document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/indentation_preferences.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      failed = 1;                                                                                  \
      goto done;                                                                                   \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1], *cases[] = {"initial", "spaces", "tabs",     "typed",  "empty",
                                          "zero",    "large",  "fraction", "suffix", "arguments"};
  int known = 0;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(name, cases[i]))
      known = 1;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int failed = 0;
  GtkWidget *widget = NULL;
  UmiEditorIndentation initial = {4, 1}, out = {8, 0};
  CHECK(UmiGtk4IndentationPreferencesCreate(&initial, &widget) == UMI_STATUS_OK);
  g_object_ref_sink(widget);
  GtkWidget *width = UmiGtk4IndentationPreferencesWidth(widget),
            *spaces = UmiGtk4IndentationPreferencesSpaces(widget);
  CHECK(GTK_IS_SPIN_BUTTON(width) && GTK_IS_CHECK_BUTTON(spaces));
  if (!strcmp(name, "arguments")) {
    GtkWidget *bad = NULL;
    CHECK(UmiGtk4IndentationPreferencesCreate(NULL, &bad) == UMI_STATUS_INVALID_ARGUMENT &&
          bad == NULL);
    CHECK(UmiGtk4IndentationPreferencesCreate(&initial, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4IndentationPreferencesRead(NULL, &out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4IndentationPreferencesRead(widget, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiGtk4IndentationPreferencesWidth(NULL) == NULL);
    CHECK(UmiGtk4IndentationPreferencesSpaces(NULL) == NULL);
    goto done;
  }
  if (!strcmp(name, "spaces"))
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(width), 2.0);
  if (!strcmp(name, "tabs"))
    gtk_check_button_set_active(GTK_CHECK_BUTTON(spaces), FALSE);
  const char *text = !strcmp(name, "typed")      ? "16"
                     : !strcmp(name, "empty")    ? ""
                     : !strcmp(name, "zero")     ? "0"
                     : !strcmp(name, "large")    ? "17"
                     : !strcmp(name, "fraction") ? "1.5"
                     : !strcmp(name, "suffix")   ? "2x"
                                                 : NULL;
  /* Allow arbitrary draft text here to exercise rejection independently of
   * GTK keyboard filtering. Production controls keep their normal policy. */
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(width), FALSE);
  if (text != NULL)
    gtk_editable_set_text(GTK_EDITABLE(width), text);
  int invalid = !strcmp(name, "empty") || !strcmp(name, "zero") || !strcmp(name, "large") ||
                !strcmp(name, "fraction") || !strcmp(name, "suffix");
  CHECK(UmiGtk4IndentationPreferencesRead(widget, &out) ==
        (invalid ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK));
  if (invalid)
    CHECK(out.tab_width == 8U && out.insert_spaces == 0);
  else
    CHECK(out.tab_width == (!strcmp(name, "typed")    ? 16U
                            : !strcmp(name, "spaces") ? 2U
                                                      : 4U) &&
          out.insert_spaces == (strcmp(name, "tabs") != 0));
done:
  g_clear_object(&widget);
  return failed;
}
