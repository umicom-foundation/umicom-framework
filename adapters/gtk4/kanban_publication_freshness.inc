/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_publication_freshness.inc
 *
 * PURPOSE:
 *   Invalidate derived public HTML and admit export only for the reviewed board and fields.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Public previews are derived state. Their admission token belongs to the
 * same panel that owns the reviewed fields; the generic exporter never borrows
 * a board or guesses whether an old HTML buffer still represents it. */
static void InvalidatePublic(Panel *panel) {
  panel->html_current = false;
  if (panel->public_generation != UINT64_MAX)
    ++panel->public_generation;
  if (panel->html == NULL || panel->html_invalidating || panel->html_publishing)
    return;
  panel->html_invalidating = true;
  GtkTextBuffer *html = g_object_ref(panel->html);
  if (gtk_text_buffer_get_char_count(html) != 0)
    gtk_text_buffer_set_text(html, "", 0);
  panel->html_invalidating = false;
  g_object_unref(html);
}
static void PublicTitleChanged(GtkEditable *editable, gpointer data) {
  (void)editable;
  Binding *binding = data;
  GtkWidget *root = g_weak_ref_get(&binding->root);
  if (root == NULL)
    return;
  Panel *panel = g_object_get_data(G_OBJECT(root), PANEL_KEY);
  /* Track native field changes even during a guarded render/adoption. This
   * generation prevents a setter observer from authorizing an older preview. */
  if (panel != NULL && panel->board != NULL)
    InvalidatePublic(panel);
  g_object_unref(root);
}
static UmiStatus PublicExportAdmission(void *context) {
  GtkWidget *root = NULL;
  Panel *panel = Acquire(context, &root);
  if (panel == NULL)
    return UMI_STATUS_UNAVAILABLE;
  UmiStatus status = panel->html_current && panel->public_generation != UINT64_MAX &&
                             panel->html_generation == panel->public_generation &&
                             panel->html_revision == umi_kanban_board_revision(panel->board)
                         ? UMI_STATUS_OK
                         : UMI_STATUS_INVALID_STATE;
  g_object_unref(root);
  return status;
}
static void PublicExportContextFree(gpointer data) { FreeBinding(data, NULL); }
static GtkWidget *CreateReviewedExport(Panel *panel) {
  Binding *binding = g_new0(Binding, 1);
  g_weak_ref_init(&binding->root, G_OBJECT(panel->root));
  GtkWidget *control = UmiTextExportGtk4CreateGuardedBuffer(
      panel->html, "index.html", PublicExportAdmission, binding, PublicExportContextFree);
  if (control == NULL)
    PublicExportContextFree(binding);
  return control;
}
