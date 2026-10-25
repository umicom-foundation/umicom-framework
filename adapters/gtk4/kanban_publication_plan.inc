/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_publication_plan.inc
 *
 * PURPOSE:
 *   Bridge reviewed plan fields into the live board without retaining its borrowed model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The persistence control owns only a weak route back to the live board panel.
 * Worker I/O never retains or touches the borrowed board. */
static UmiStatus CapturePublicPlan(gpointer context, UmiKanbanPublicationPlan **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  GtkWidget *root = NULL;
  Panel *panel = Acquire(context, &root);
  if (panel == NULL)
    return UMI_STATUS_UNAVAILABLE;
  UmiStatus status =
      gtk_widget_get_mapped(root)
          ? UmiKanbanPublicationPlanCreate(panel->board, panel->revision,
                                           gtk_editable_get_text(GTK_EDITABLE(panel->site_title)),
                                           panel->publication, panel->publication_count, out)
          : UMI_STATUS_UNAVAILABLE;
  g_object_unref(root);
  return status;
}
static UmiStatus AdoptPublicPlan(gpointer context, const UmiKanbanPublicationPlan *saved) {
  GtkWidget *root = NULL;
  Panel *panel = Acquire(context, &root);
  if (panel == NULL)
    return UMI_STATUS_UNAVAILABLE;
  UmiKanbanPublicationPlan *reviewed = NULL;
  UmiStatus status =
      gtk_widget_get_mapped(root)
          ? UmiKanbanPublicationPlanReview(saved, panel->board, panel->revision, &reviewed)
          : UMI_STATUS_UNAVAILABLE;
  if (status == UMI_STATUS_OK) {
    UmiKanbanPublicationPlanInfo info;
    status = UmiKanbanPublicationPlanRead(reviewed, &info);
    if (status == UMI_STATUS_OK) {
      /* Retain presentation objects across observable native updates;
       * changed public fields invalidate the previously generated HTML. */
      GtkWidget *title = g_object_ref(panel->site_title);
      GtkTextBuffer *html = g_object_ref(panel->html);
      panel->applying = true;
      for (size_t i = 0U; i < info.card_count; ++i)
        panel->publication[i] = *UmiKanbanPublicationPlanAt(reviewed, i);
      panel->publication_count = info.card_count;
      gtk_editable_set_text(GTK_EDITABLE(title), info.title);
      /* Keep the former buffer-only invalidation for review. Shared freshness
       * also invalidates export admission for the previous reviewed fields. */
#if 0
      gtk_text_buffer_set_text(html, "", 0);
#endif
      InvalidatePublic(panel);
      panel->applying = false;
      g_object_unref(html);
      g_object_unref(title);
    }
  }
  UmiKanbanPublicationPlanDestroy(reviewed);
  g_object_unref(root);
  return status;
}
static void FreePlanContext(gpointer data) { FreeBinding(data, NULL); }
static GtkWidget *CreatePublicPlanStore(Panel *panel) {
  Binding *binding = g_new0(Binding, 1);
  g_weak_ref_init(&binding->root, G_OBJECT(panel->root));
  GtkWidget *widget =
      UmiKanbanPublicPlanGtk4Create(CapturePublicPlan, AdoptPublicPlan, binding, FreePlanContext);
  if (widget == NULL)
    FreePlanContext(binding);
  return widget;
}
