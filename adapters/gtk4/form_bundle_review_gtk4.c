/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Review is display-only: copied labels cannot change after the source closes,
 * and the host owns the separate authorization and storage operation. */
#include "record_time_internal.h"
#include "umicom/ui/gtk4/form_bundle_review.h"
#include "umicom/ui/gtk4/structured_form_inspect.h"
#include <inttypes.h>
typedef struct UmiBundleReviewWidget {
  GtkBox parent;
  UmiStructuredFormBundleSummary summary;
  GtkWidget *confirm;
  gboolean retired;
} UmiBundleReviewWidget;
typedef struct UmiBundleReviewWidgetClass {
  GtkBoxClass parent_class;
} UmiBundleReviewWidgetClass;
GType umi_bundle_review_widget_get_type(void);
G_DEFINE_TYPE(UmiBundleReviewWidget, umi_bundle_review_widget, GTK_TYPE_BOX)
static void bundle_unroot(GtkWidget *widget) {
  ((UmiBundleReviewWidget *)widget)->retired = TRUE;
  GTK_WIDGET_CLASS(umi_bundle_review_widget_parent_class)->unroot(widget);
}
static void bundle_dispose(GObject *object) {
  ((UmiBundleReviewWidget *)object)->retired = TRUE;
  G_OBJECT_CLASS(umi_bundle_review_widget_parent_class)->dispose(object);
}
static void umi_bundle_review_widget_class_init(UmiBundleReviewWidgetClass *klass) {
  GTK_WIDGET_CLASS(klass)->unroot = bundle_unroot;
  G_OBJECT_CLASS(klass)->dispose = bundle_dispose;
}
static void umi_bundle_review_widget_init(UmiBundleReviewWidget *self) {
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 12);
}
static UmiBundleReviewWidget *instance(GtkWidget *widget) {
  return widget != NULL && G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_bundle_review_widget_get_type())
             ? (UmiBundleReviewWidget *)widget
             : NULL;
}
static void text(GtkWidget *box, const char *name, const char *value) {
  GtkWidget *label = gtk_label_new(value);
  gtk_widget_set_name(label, name);
  gtk_label_set_xalign(GTK_LABEL(label), 0);
  gtk_label_set_wrap(GTK_LABEL(label), TRUE);
  gtk_label_set_selectable(GTK_LABEL(label), TRUE);
  gtk_box_append(GTK_BOX(box), label);
}
/* Use an explicit UTC calendar representation while retaining full nanosecond
 * precision. Zero observation time remains unknown, never the Unix epoch. */
/* Shared formatting now serves history and receipts with the same UTC and
 * unknown-time semantics. Preserve the earlier local implementation. */
#if 0
static char *calendar_time(uint64_t nanoseconds) {
  if (nanoseconds == 0)
    return g_strdup("Unknown");
  GDateTime *date = g_date_time_new_from_unix_utc((gint64)(nanoseconds / 1000000000U));
  if (date == NULL)
    return g_strdup("Unsupported timestamp");
  char *seconds = g_date_time_format(date, "%Y-%m-%d %H:%M:%S");
  char *result = g_strdup_printf("%s.%09" PRIu64 " UTC", seconds, nanoseconds % 1000000000U);
  g_free(seconds);
  g_date_time_unref(date);
  return result;
}
#endif
static char *calendar_time(uint64_t nanoseconds) { return umi_gtk_record_time(nanoseconds); }
static gboolean metadata_utf8(const UmiRecordRevision *record) {
  return g_utf8_validate(record->amendment.entity_id, -1, NULL) &&
         g_utf8_validate(record->amendment.source, -1, NULL) &&
         g_utf8_validate(record->amendment.actor, -1, NULL) &&
         g_utf8_validate(record->amendment.reason, -1, NULL);
}
UmiStatus UmiFormBundleReviewGtk4New(const UmiStructuredFormBundleReview *review, GtkWidget **out) {
  if (review == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStructuredFormBundleSummary summary;
  UmiStatus status = umi_structured_form_bundle_summary(review, &summary);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormHistorical *historical = g_try_new(UmiStructuredFormHistorical, 1);
  if (historical == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  GtkWidget *root = g_object_new(umi_bundle_review_widget_get_type(), NULL);
  UmiBundleReviewWidget *self = (UmiBundleReviewWidget *)root;
  self->summary = summary;
  GtkWidget *stack = gtk_stack_new();
  gtk_widget_set_name(stack, "form-bundle-history");
  GtkWidget *switcher = gtk_stack_switcher_new();
  gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), GTK_STACK(stack));
  gtk_box_append(GTK_BOX(root), switcher);
  gtk_box_append(GTK_BOX(root), stack);
  for (size_t i = 0; i < summary.revision_count && status == UMI_STATUS_OK; ++i) {
    status = umi_structured_form_bundle_inspect(review, i, historical);
    if (status == UMI_STATUS_OK && !metadata_utf8(&historical->record))
      status = UMI_STATUS_PARSE_ERROR;
    GtkWidget *inspect = NULL;
    if (status == UMI_STATUS_OK)
      status = UmiStructuredFormInspectGtk4New(historical->resolved.schema,
                                               &historical->resolved.form, &inspect);
    if (status != UMI_STATUS_OK)
      break;
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    char *name = g_strdup_printf("revision-%zu", i + 1);
    char *title = g_strdup_printf("Revision %zu", i + 1);
    /* Calendar labels replace raw integer-only display. Retain that formatting
     * for review of the original precision and unknown-time semantics. */
#if 0
    char *times = g_strdup_printf("Recorded UTC nanoseconds: %" PRIu64 " | Observed UTC nanoseconds: %" PRIu64 " (0 means unknown)",
        historical->record.amendment.recorded_at_nanoseconds, historical->record.amendment.observed_at_nanoseconds);
#endif
    char *recorded = calendar_time(historical->record.amendment.recorded_at_nanoseconds);
    char *observed = calendar_time(historical->record.amendment.observed_at_nanoseconds);
    char *times = g_strdup_printf("Recorded: %s | Observed: %s", recorded, observed);
    g_free(recorded);
    g_free(observed);
    text(page, "form-bundle-author", historical->record.amendment.actor);
    text(page, "form-bundle-source", historical->record.amendment.source);
    text(page, "form-bundle-reason", historical->record.amendment.reason);
    text(page, "form-bundle-times", times);
    gtk_box_append(GTK_BOX(page), inspect);
    gtk_stack_add_titled(GTK_STACK(stack), page, name, title);
    g_free(times);
    g_free(title);
    g_free(name);
  }
  g_free(historical);
  if (status != UMI_STATUS_OK) {
    g_object_ref_sink(root);
    g_object_unref(root);
    return status;
  }
  char *identity =
      g_strdup_printf("%s | %s | %zu original revisions", summary.latest.amendment.entity_id,
                      summary.form_id, summary.revision_count);
  text(root, "form-bundle-identity", identity);
  g_free(identity);
  text(root, "form-bundle-digest", summary.bundle_sha256);
  text(root, "form-bundle-boundary",
       "Original author labels are not authenticated identities. Unanswered fields remain drafts. "
       "Import does not overwrite existing records.");
  self->confirm = gtk_check_button_new_with_label(
      "I intend to import the original record and history shown here.");
  gtk_widget_set_name(self->confirm, "form-bundle-confirm");
  gtk_box_append(GTK_BOX(root), self->confirm);
  char *latest = g_strdup_printf("revision-%zu", summary.revision_count);
  gtk_stack_set_visible_child_name(GTK_STACK(stack), latest);
  g_free(latest);
  *out = root;
  return UMI_STATUS_OK;
}
UmiStatus UmiFormBundleReviewGtk4Summary(GtkWidget *widget, UmiStructuredFormBundleSummary *out) {
  UmiBundleReviewWidget *self = instance(widget);
  if (self == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired)
    return UMI_STATUS_INVALID_STATE;
  *out = self->summary;
  return UMI_STATUS_OK;
}
UmiStatus UmiFormBundleReviewGtk4Confirmed(GtkWidget *widget, UmiStructuredFormBundleSummary *out) {
  UmiBundleReviewWidget *self = instance(widget);
  if (self == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired || !gtk_check_button_get_active(GTK_CHECK_BUTTON(self->confirm)))
    return UMI_STATUS_INVALID_STATE;
  *out = self->summary;
  return UMI_STATUS_OK;
}
void UmiFormBundleReviewGtk4Retire(GtkWidget *widget) {
  UmiBundleReviewWidget *self = instance(widget);
  if (self == NULL || self->retired)
    return;
  g_object_ref(widget);
  self->retired = TRUE;
  gtk_widget_set_sensitive(widget, FALSE);
  g_object_unref(widget);
}
