/*-----------------------------------------------------------------------------
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 *
 * LICENCE: MIT
 *
 * Umicom Framework
 * File: adapters/gtk4/ibkr_rule_report_gtk4.c
 * PURPOSE: Import route reports off the UI thread and compare explicitly captured policy
 * identities. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/rule_report_gtk4.h"
#include "umicom/platform/input_file.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "window_removal_private.h"
#include <limits.h>
#include <string.h>
#define RULE_PANEL_KEY "umicom-ibkr-rule-report-panel"
typedef struct RulePanel {
  GtkWidget *box, *path, *load, *routes, *output, *comparison;
  UmiIbkrRuleReport *report;
  gboolean loading, publishing, expected, invalidated;
  uint64_t generation;
  uint32_t contract;
  UmiSide side;
  char exchange[64];
} RulePanel;
typedef struct RuleLoad {
  char *path;
  uint64_t generation;
  UmiStatus status;
  UmiIbkrRuleReport *report;
} RuleLoad;
/* A retained GTK window can still be a widget root after native destruction.
 * Consult Framework's toplevel registry check instead of treating that retained
 * pointer as permission to load or publish. */
static gboolean RulePanelActive(GtkWidget *box) {
  GtkRoot *root = gtk_widget_get_root(box);
  return GTK_IS_WINDOW(root) && UmiGtk4WindowRegistered(GTK_WINDOW(root));
}
static RulePanel *RulePanelOwner(GtkWidget *box) {
  RulePanel *ui = g_object_get_data(G_OBJECT(box), RULE_PANEL_KEY);
  return ui != NULL && RulePanelActive(box) ? ui : NULL;
}
static GtkWidget *RuleLabel(const char *text, const char *name) {
  GtkWidget *label = gtk_label_new(text);
  gtk_widget_set_name(label, name);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
  gtk_label_set_wrap(GTK_LABEL(label), TRUE);
  gtk_label_set_selectable(GTK_LABEL(label), TRUE);
  return label;
}
static void RulePanelFree(gpointer data) {
  RulePanel *ui = data;
  UmiIbkrRuleReportDestroy(ui->report);
  g_free(ui);
}
static void RuleLoadFree(gpointer data) {
  RuleLoad *load = data;
  UmiIbkrRuleReportDestroy(load->report);
  g_free(load->path);
  g_free(load);
}
/* No GtkWidget or connection is touched by the worker. The task's copied path
 * is checked by the canonical bounded regular-file reader before JSON import. */
static void RuleLoadWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel) {
  (void)source;
  (void)cancel;
  RuleLoad *load = data;
  unsigned char *bytes = NULL;
  size_t size = 0U;
  load->status = UmiInputFileRead(load->path, UMI_IBKR_RULE_REPORT_MAX_BYTES, &bytes, &size);
  if (load->status == UMI_STATUS_OK)
    load->status = UmiIbkrRuleReportCreate(bytes, size, &load->report);
  UmiInputFileFree(bytes);
  g_task_return_boolean(task, TRUE);
}
static void RulePanelPaint(RulePanel *ui) {
  if (ui->publishing || !RulePanelActive(ui->box))
    return;
  g_object_ref(ui->box);
  ui->publishing = TRUE;
  ui->invalidated = FALSE;
  guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->routes));
  char *description = NULL;
  if (ui->report != NULL)
    (void)UmiIbkrRuleReportDescribe(ui->report, index, &description);
  gtk_label_set_text(GTK_LABEL(ui->output),
                     description != NULL
                         ? description
                         : "Choose a saved route-rule JSON report. No broker session is opened.");
  UmiIbkrRuleReportFreeText(description);
  /* Publish comparison last. A notification on the description may retire an
   * expectation or close the window; read current state again after it returns. */
  if (RulePanelActive(ui->box)) {
    index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->routes));
    UmiIbkrRuleBinding binding;
    UmiStatus status = ui->expected && ui->report != NULL
                           ? UmiIbkrRuleReportCompare(ui->report, index, ui->contract, ui->side,
                                                      ui->exchange, &binding)
                           : UMI_STATUS_UNAVAILABLE;
    gtk_label_set_text(GTK_LABEL(ui->comparison),
                       status == UMI_STATUS_OK ? UmiIbkrRuleBindingText(binding)
                                               : "No captured policy comparison. Review the "
                                                 "full-quantity policy for the current quote.");
  }
  /* Nested application notifications can change the selected route or policy.
   * Never leave the outer publication's MATCH visible after that change. */
  if (ui->invalidated && RulePanelActive(ui->box)) {
    ui->expected = FALSE;
    gtk_label_set_text(GTK_LABEL(ui->comparison),
                       "Review changed during publication. Review the policy again.");
  }
  ui->publishing = FALSE;
  g_object_unref(ui->box);
}
static void RuleRouteChanged(GObject *object, GParamSpec *property, gpointer data) {
  (void)object;
  (void)property;
  RulePanel *ui = RulePanelOwner(GTK_WIDGET(data));
  if (ui != NULL) {
    ui->invalidated = TRUE;
    RulePanelPaint(ui);
  }
}
static void RuleLoadFinished(GObject *source, GAsyncResult *result, gpointer data) {
  (void)data;
  GTask *task = G_TASK(result);
  (void)g_task_propagate_boolean(task, NULL);
  RuleLoad *load = g_task_get_task_data(task);
  RulePanel *ui = g_object_get_data(source, RULE_PANEL_KEY);
  if (ui == NULL)
    return;
  ui->loading = FALSE;
  if (!RulePanelActive(ui->box) || ui->generation != load->generation)
    return;
  if (load->status != UMI_STATUS_OK) {
    gchar *message = g_strdup_printf("Report not imported: %s. Check the selected file and report "
                                     "format. No broker request was made.",
                                     umi_status_text(load->status));
    ui->publishing = TRUE;
    gtk_label_set_text(GTK_LABEL(ui->output), message);
    ui->publishing = FALSE;
    g_free(message);
    return;
  }
  UmiIbkrRuleReportDestroy(ui->report);
  ui->report = load->report;
  load->report = NULL;
  UmiIbkrRuleReportIdentity identity;
  (void)UmiIbkrRuleReportRead(ui->report, &identity);
  GtkStringList *names = gtk_string_list_new(NULL);
  for (size_t i = 0U; i < identity.routeCount; ++i) {
    UmiIbkrRuleRoute route;
    (void)UmiIbkrRuleReportRouteAt(ui->report, i, &route);
    gtk_string_list_append(names, route.exchange);
  }
  /* GtkDropDown retains the model. Publish only after the complete report and
   * route list are ready; route notifications cannot observe a partial import. */
  ui->publishing = TRUE;
  gtk_drop_down_set_model(GTK_DROP_DOWN(ui->routes), G_LIST_MODEL(names));
  g_object_unref(names);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->routes), 0U);
  ui->publishing = FALSE;
  RulePanelPaint(ui);
}
/* Clearing a report also clears the route selector. Retained names must not
 * suggest that a failed or cancelled import still owns valid route evidence. */
static void RulePanelResetChoices(RulePanel *ui) {
  if (ui->publishing)
    return;
  gboolean publishing = ui->publishing;
  ui->publishing = TRUE;
  GtkStringList *empty = gtk_string_list_new((const char *[]){"No report", NULL});
  gtk_drop_down_set_model(GTK_DROP_DOWN(ui->routes), G_LIST_MODEL(empty));
  g_object_unref(empty);
  ui->publishing = publishing;
}
static void RuleLoadClicked(GtkButton *button, gpointer data) {
  (void)button;
  RulePanel *ui = RulePanelOwner(GTK_WIDGET(data));
  if (ui == NULL || ui->loading || ui->publishing)
    return;
  g_object_ref(ui->box);
  /* Capture the chosen path before any notification can edit it. Retire the
   * old report so a failed replacement cannot masquerade as new evidence. */
  RuleLoad *load = g_new0(RuleLoad, 1);
  load->path = g_strdup(gtk_editable_get_text(GTK_EDITABLE(ui->path)));
  load->generation = ++ui->generation;
  ui->loading = TRUE;
  UmiIbkrRuleReportDestroy(ui->report);
  ui->report = NULL;
  RulePanelResetChoices(ui);
  if (!RulePanelActive(ui->box) || ui->generation != load->generation) {
    ui->loading = FALSE;
    RuleLoadFree(load);
    g_object_unref(ui->box);
    return;
  }
  const uint64_t generation = load->generation;
  GTask *task = g_task_new(ui->box, NULL, RuleLoadFinished, NULL);
  g_task_set_task_data(task, load, RuleLoadFree);
  g_task_run_in_thread(task, RuleLoadWorker);
  g_object_unref(task);
  ui->publishing = TRUE;
  gtk_label_set_text(GTK_LABEL(ui->comparison),
                     "Waiting for imported metadata; no support has been confirmed.");
  if (RulePanelActive(ui->box) && ui->loading && ui->generation == generation)
    gtk_label_set_text(GTK_LABEL(ui->output), "Reading the selected local report...");
  ui->publishing = FALSE;
  g_object_unref(ui->box);
}
static void RulePathChanged(GtkEditable *entry, gpointer data) {
  (void)entry;
  RulePanel *ui = g_object_get_data(G_OBJECT(data), RULE_PANEL_KEY);
  if (ui == NULL)
    return;
  ++ui->generation;
  ui->invalidated = TRUE;
  UmiIbkrRuleReportDestroy(ui->report);
  ui->report = NULL;
  RulePanelResetChoices(ui);
  RulePanelPaint(ui);
}
static void RuleRootChanged(GObject *object, GParamSpec *property, gpointer data) {
  (void)property;
  (void)data;
  RulePanel *ui = g_object_get_data(object, RULE_PANEL_KEY);
  if (ui != NULL && !RulePanelActive(ui->box)) {
    ++ui->generation;
    ui->expected = FALSE;
  }
}
static void RuleClearClicked(GtkButton *button, gpointer data) {
  (void)button;
  RulePanel *ui = RulePanelOwner(GTK_WIDGET(data));
  if (ui == NULL || ui->publishing)
    return;
  ++ui->generation;
  UmiIbkrRuleReportDestroy(ui->report);
  ui->report = NULL;
  RulePanelResetChoices(ui);
  ui->expected = FALSE;
  if (ui->publishing)
    ui->invalidated = TRUE;
  RulePanelPaint(ui);
}
GtkWidget *UmiIbkrRuleReportGtk4Create(void) {
  RulePanel *ui = g_new0(RulePanel, 1);
  ui->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  g_object_set_data_full(G_OBJECT(ui->box), RULE_PANEL_KEY, ui, RulePanelFree);
  gtk_widget_set_name(ui->box, "ibkr-rule-report");
  gtk_box_append(GTK_BOX(ui->box), RuleLabel("Import a saved Client Portal route-rule report. This "
                                             "is separate from TWS and does not log in, "
                                             "refresh capabilities or enable trading.",
                                             "ibkr-rule-report-help"));
  ui->path = gtk_entry_new();
  gtk_widget_set_name(ui->path, "ibkr-rule-report-path");
  gtk_entry_set_max_length(GTK_ENTRY(ui->path), 4096);
  gtk_entry_set_placeholder_text(GTK_ENTRY(ui->path), "Choose the saved JSON report");
  gtk_box_append(GTK_BOX(ui->box), ui->path);
  GtkWidget *browse = UmiPathPickerGtk4Create(GTK_ENTRY(ui->path), UMI_PATH_PICKER_EXISTING_FILE,
                                              "Choose an IBKR route-rule report", NULL);
  gtk_box_append(GTK_BOX(ui->box), browse);
  ui->load = gtk_button_new_with_label("Import rule report");
  gtk_widget_set_name(ui->load, "ibkr-rule-report-load");
  gtk_box_append(GTK_BOX(ui->box), ui->load);
  ui->routes = gtk_drop_down_new_from_strings((const char *[]){"No report", NULL});
  gtk_widget_set_name(ui->routes, "ibkr-rule-report-routes");
  gtk_box_append(GTK_BOX(ui->box), ui->routes);
  ui->output = RuleLabel("Choose a saved route-rule JSON report. No broker session is opened.",
                         "ibkr-rule-report-output");
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 180);
  gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(scroll), 360);
  gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(scroll), TRUE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), ui->output);
  gtk_box_append(GTK_BOX(ui->box), scroll);
  ui->comparison = RuleLabel("Review a full-quantity policy to compare identities.",
                             "ibkr-rule-report-comparison");
  gtk_box_append(GTK_BOX(ui->box), ui->comparison);
  GtkWidget *clear = gtk_button_new_with_label("Clear imported report");
  gtk_widget_set_name(clear, "ibkr-rule-report-clear");
  gtk_box_append(GTK_BOX(ui->box), clear);
  g_signal_connect_object(ui->load, "clicked", G_CALLBACK(RuleLoadClicked), ui->box, 0);
  g_signal_connect_object(clear, "clicked", G_CALLBACK(RuleClearClicked), ui->box, 0);
  g_signal_connect_object(ui->path, "changed", G_CALLBACK(RulePathChanged), ui->box, 0);
  g_signal_connect_object(ui->routes, "notify::selected", G_CALLBACK(RuleRouteChanged), ui->box, 0);
  g_signal_connect(ui->box, "notify::root", G_CALLBACK(RuleRootChanged), NULL);
  return ui->box;
}
UmiStatus UmiIbkrRuleReportGtk4SetExpectation(GtkWidget *panel, uint32_t contractId, UmiSide side,
                                              const char *exchange) {
  if (panel == NULL || exchange == NULL || contractId == 0U || contractId > INT_MAX ||
      (side != UMI_SIDE_BUY && side != UMI_SIDE_SELL))
    return UMI_STATUS_INVALID_ARGUMENT;
  RulePanel *ui = RulePanelOwner(panel);
  if (ui == NULL)
    return UMI_STATUS_INVALID_STATE;
  size_t length = 0U;
  while (length < sizeof ui->exchange && exchange[length] != '\0') {
    if ((unsigned char)exchange[length] < 33U || (unsigned char)exchange[length] > 126U)
      return UMI_STATUS_INVALID_ARGUMENT;
    ++length;
  }
  if (length == 0U || length == sizeof ui->exchange)
    return UMI_STATUS_INVALID_ARGUMENT;
  ui->contract = contractId;
  ui->side = side;
  memcpy(ui->exchange, exchange, length + 1U);
  ui->expected = TRUE;
  if (ui->publishing)
    ui->invalidated = TRUE;
  RulePanelPaint(ui);
  return UMI_STATUS_OK;
}
void UmiIbkrRuleReportGtk4ClearExpectation(GtkWidget *panel) {
  if (panel == NULL)
    return;
  RulePanel *ui = g_object_get_data(G_OBJECT(panel), RULE_PANEL_KEY);
  if (ui == NULL)
    return;
  ui->expected = FALSE;
  if (ui->publishing)
    ui->invalidated = TRUE;
  RulePanelPaint(ui);
}
