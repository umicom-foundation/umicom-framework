/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native transfer review owns display text and never grants storage authority. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_record.h"
#include "umicom/ui/gtk4/form_bundle_review.h"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static GtkWidget *find(GtkWidget *root, const char *name) {
  if (strcmp(gtk_widget_get_name(root), name) == 0)
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *found = find(child, name);
    if (found)
      return found;
  }
  return NULL;
}
static int readonly(GtkWidget *root) {
  if (GTK_IS_EDITABLE(root) || GTK_IS_TEXT_VIEW(root))
    return 0;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child))
    if (!readonly(child))
      return 0;
  return 1;
}
int main(int argc, char **argv) {
  const char *cases[] = {"history",  "unchecked",    "confirmed", "withdrawn", "retired",
                         "unrooted", "owned",        "readonly",  "markup",    "utf8",
                         "secret",   "wrong-widget", "clock"};
  if (argc != 2)
    return 2;
  bool selected = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      selected = true;
  if (!selected)
    return 2;
  if (!gtk_init_check())
    return 77;
  const char *name = argv[1];
  int result = 0;
  FormFixture *f = form_fixture_new(), *g = form_fixture_new();
  UmiDataServer *source = NULL;
  UmiRecordBundle *bundle = NULL;
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiStructuredFormBundleReview *review = NULL;
  GtkWidget *root = NULL, *window = NULL, *other = NULL;
  CHECK(f && g);
  if (strcmp(name, "secret") == 0)
    f->fields[2].kind = UMI_APPLICATION_EXPERIENCE_FIELD_SECRET;
  g->schema.version = 2;
  g->rules[0].unit_id = "mass.lb";
  OK(umi_structured_form_init(&g->schema, &g->before));
  const UmiStructuredFormSchema *schemas[] = {&f->schema, &g->schema};
  OK(umi_structured_form_catalogue_create(schemas, 2, &catalogue));
  OK(umi_data_server_create_memory(&source));
  UmiRecordAmendment metadata = {.entity_id = "review.parcel",
                                 .source = "Synthetic source",
                                 .actor = "<b>Original author</b>",
                                 .reason = "Original",
                                 .recorded_at_nanoseconds = 100};
  if (strcmp(name, "utf8") == 0) {
    metadata.actor[0] = (char)0xff;
    metadata.actor[1] = '\0';
  }
  UmiFormAnswer answer = form_mass(7);
  UmiRecordRevision saved;
  OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &answer));
  OK(umi_structured_form_record_save(source, &metadata, &f->schema, &f->before, 0, &saved));
  metadata.recorded_at_nanoseconds = strcmp(name, "clock") == 0 ? UINT64_MAX : 200;
  memcpy(metadata.actor, "Second author", 14);
  answer = form_mass(9);
  memcpy(answer.unit_id, "mass.lb", 8);
  OK(umi_structured_form_set(&g->schema, &g->before, g->before.revision, "parcel.mass", &answer));
  OK(umi_structured_form_record_save(source, &metadata, &g->schema, &g->before, 1, &saved));
  OK(umi_record_bundle_capture(source, metadata.entity_id, &bundle));
  OK(umi_structured_form_bundle_review(bundle, catalogue, "example.parcel", "review.", NULL, NULL,
                                       &review));
  UmiStatus status = UmiFormBundleReviewGtk4New(review, &root);
  if (strcmp(name, "utf8") == 0 || strcmp(name, "secret") == 0) {
    CHECK(status ==
          (strcmp(name, "utf8") == 0 ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_PERMISSION_DENIED));
    CHECK(root == NULL);
    goto cleanup;
  }
  OK(status);
  g_object_ref_sink(root);
  UmiStructuredFormBundleSummary summary, out;
  OK(UmiFormBundleReviewGtk4Summary(root, &summary));
  memset(&out, 0xa5, sizeof(out));
  UmiStructuredFormBundleSummary before = out;
  GtkWidget *confirm = find(root, "form-bundle-confirm"),
            *stack = find(root, "form-bundle-history");
  CHECK(GTK_IS_CHECK_BUTTON(confirm) && GTK_IS_STACK(stack));
  CHECK(strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack)), "revision-2") == 0);
  GtkWidget *old = gtk_stack_get_child_by_name(GTK_STACK(stack), "revision-1");
  GtkWidget *latest = gtk_stack_get_child_by_name(GTK_STACK(stack), "revision-2");
  CHECK(old && latest && gtk_stack_get_child_by_name(GTK_STACK(stack), "revision-3") == NULL);
  CHECK(strstr(gtk_label_get_text(GTK_LABEL(find(old, "form-inspect.parcel.mass.value"))),
               "mass.kg") != NULL);
  CHECK(strstr(gtk_label_get_text(GTK_LABEL(find(latest, "form-inspect.parcel.mass.value"))),
               "mass.lb") != NULL);
  CHECK(readonly(root));
  CHECK(strstr(gtk_label_get_text(GTK_LABEL(find(old, "form-bundle-times"))),
               "1970-01-01 00:00:00.000000100 UTC") != NULL);
  CHECK(strstr(gtk_label_get_text(GTK_LABEL(find(old, "form-bundle-times"))),
               "Observed: Unknown") != NULL);
  if (strcmp(name, "clock") == 0) {
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(find(latest, "form-bundle-times"))),
                 ".709551615 UTC") != NULL);
    goto cleanup;
  }
  CHECK(UmiFormBundleReviewGtk4Confirmed(root, &out) == UMI_STATUS_INVALID_STATE &&
        memcmp(&out, &before, sizeof(out)) == 0);
  if (strcmp(name, "unchecked") == 0 || strcmp(name, "history") == 0 ||
      strcmp(name, "readonly") == 0)
    goto cleanup;
  if (strcmp(name, "markup") == 0) {
    GtkWidget *author = find(old, "form-bundle-author");
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(author)), "<b>Original author</b>") == 0 &&
          !gtk_label_get_use_markup(GTK_LABEL(author)));
    goto cleanup;
  }
  if (strcmp(name, "owned") == 0) {
    umi_structured_form_bundle_review_destroy(review);
    review = NULL;
    umi_record_bundle_destroy(bundle);
    bundle = NULL;
    umi_structured_form_catalogue_destroy(catalogue);
    catalogue = NULL;
    umi_data_server_destroy(source);
    source = NULL;
    free(f);
    f = NULL;
    free(g);
    g = NULL;
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "revision-1");
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(find(old, "form-inspect.parcel.mass.value"))),
                 "mass.kg") != NULL);
  }
  if (strcmp(name, "wrong-widget") == 0) {
    other = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    g_object_ref_sink(other);
    CHECK(UmiFormBundleReviewGtk4Confirmed(other, &out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFormBundleReviewGtk4Summary(other, &out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&out, &before, sizeof(out)) == 0);
    goto cleanup;
  }
  gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
  OK(UmiFormBundleReviewGtk4Confirmed(root, &out));
  CHECK(strcmp(out.bundle_sha256, summary.bundle_sha256) == 0 && out.revision_count == 2);
  if (strcmp(name, "withdrawn") == 0)
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), FALSE);
  if (strcmp(name, "retired") == 0) {
    UmiFormBundleReviewGtk4Retire(root);
    UmiFormBundleReviewGtk4Retire(root);
    CHECK(!gtk_widget_get_sensitive(root));
  }
  if (strcmp(name, "unrooted") == 0) {
    window = gtk_window_new();
    g_object_ref_sink(window);
    gtk_window_set_child(GTK_WINDOW(window), root);
    gtk_window_set_child(GTK_WINDOW(window), NULL);
  }
  if (strcmp(name, "withdrawn") == 0 || strcmp(name, "retired") == 0 ||
      strcmp(name, "unrooted") == 0) {
    out = before;
    CHECK(UmiFormBundleReviewGtk4Confirmed(root, &out) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&out, &before, sizeof(out)) == 0);
  }
cleanup:
  if (window) {
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
  }
  if (other)
    g_object_unref(other);
  if (root)
    g_object_unref(root);
  umi_structured_form_bundle_review_destroy(review);
  umi_record_bundle_destroy(bundle);
  umi_structured_form_catalogue_destroy(catalogue);
  umi_data_server_destroy(source);
  free(g);
  free(f);
  return result;
}
