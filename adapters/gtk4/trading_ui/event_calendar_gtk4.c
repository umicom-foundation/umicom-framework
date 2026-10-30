/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/trading_ui/event_calendar_gtk4.c
 *
 * PURPOSE:
 *   Present bounded calendar pages and native local-file import while retaining evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading_ui/gtk4/event_calendar.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/workstation/action_group.h"
#include <string.h>
#define PAGE_SIZE 100U
typedef struct CalendarUi {
  UmiEventCalendar *calendar;
  UmiEventCalendarFilter applied;
  GtkWidget *from, *through, *search, *country, *currency, *kind, *impact;
  GtkWidget *import, *cancel_button, *apply, *previous, *next, *message, *rows, *details;
  GtkFileChooserNative *chooser;
  GCancellable *cancel;
  size_t offset, total;
  bool detached, loading;
} CalendarUi;
typedef struct CalendarRead {
  GWeakRef root;
  GFile *file;
} CalendarRead;
static CalendarUi *State(GtkWidget *root) {
  return root != NULL ? g_object_get_data(G_OBJECT(root), "umicom-event-calendar") : NULL;
}
static void Free(gpointer data) {
  CalendarUi *s = data;
  if (s->cancel != NULL)
    g_cancellable_cancel(s->cancel);
  g_clear_object(&s->cancel);
  if (s->chooser != NULL) {
    gtk_native_dialog_destroy(GTK_NATIVE_DIALOG(s->chooser));
    g_object_unref(s->chooser);
  }
  UmiEventCalendarDestroy(s->calendar);
  g_free(s);
}
static void Tag(GtkWidget *w, const char *id) { (void)umi_gtk4_automation_tag_widget(w, id); }
static void Message(CalendarUi *s, const char *text) {
  gtk_label_set_text(GTK_LABEL(s->message), text);
}
static GtkWidget *Label(const char *text) {
  GtkWidget *w = gtk_label_new(text);
  gtk_label_set_xalign(GTK_LABEL(w), 0.0F);
  gtk_label_set_wrap(GTK_LABEL(w), TRUE);
  return w;
}
static const char *Value(const char *text) { return text[0] ? text : "Not supplied"; }
static void Selected(GtkListBox *list, GtkListBoxRow *row, gpointer root) {
  (void)list;
  CalendarUi *s = State(root);
  if (s == NULL || row == NULL)
    return;
  size_t index = (size_t)GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(row), "event-index")) - 1U;
  const UmiCalendarEvent *e = UmiEventCalendarAt(s->calendar, index);
  if (e == NULL)
    return;
  char *detail = g_strdup_printf(
      "%s\n%s | %s\nSource date: %s\nSource time: %s (%s; %s)\nStatus: %s\nCountry: %s | "
      "Currency: %s\nInstrument: %s | Venue: %s\nPeriod: %s\nImportance: %s\nEstimate: "
      "%s\nActual: %s\nPrevious: %s\nUnit: %s\nSource: %s\nSource observed at: %s\nData class: "
      "%s\nEvent ID: %s\n\nImported snapshot only. Coverage and accuracy are not verified. Dates "
      "and times retain the source timezone; no timezone conversion or trading action is "
      "performed.",
      e->title, e->kind, e->quality, e->date, Value(e->time), e->timezone, e->timing, e->status,
      Value(e->country), Value(e->currency), Value(e->symbol), Value(e->venue), Value(e->period),
      e->importance, Value(e->estimate), Value(e->actual), Value(e->previous), Value(e->unit),
      e->source, e->observed_at, e->quality, e->id);
  gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(s->details)), detail, -1);
  g_free(detail);
}
static void Render(CalendarUi *s) {
  size_t indexes[PAGE_SIZE], count = 0U, total = 0U;
  if (s->calendar != NULL)
    (void)UmiEventCalendarSelect(s->calendar, &s->applied, s->offset, indexes, PAGE_SIZE, &count,
                                 &total);
  GtkWidget *child;
  while ((child = gtk_widget_get_first_child(s->rows)) != NULL)
    gtk_list_box_remove(GTK_LIST_BOX(s->rows), child);
  gtk_text_buffer_set_text(
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(s->details)),
      "Select an event to review its source, reporting period and supplied values.", -1);
  s->total = total;
  for (size_t i = 0U; i < count; ++i) {
    const UmiCalendarEvent *e = UmiEventCalendarAt(s->calendar, indexes[i]);
    GtkWidget *row = gtk_list_box_row_new(), *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    char *heading =
        g_strdup_printf("%s  %s  |  %s  |  %s%s%s", e->date, e->time[0] ? e->time : e->timing,
                        e->kind, e->symbol, e->symbol[0] ? " / " : "", e->venue);
    gtk_box_append(GTK_BOX(box), Label(heading));
    g_free(heading);
    gtk_box_append(GTK_BOX(box), Label(e->title));
    char *values =
        g_strdup_printf("Actual: %s  |  Estimate: %s  |  Previous: %s  [%s]\n%s | %s | %s | %s",
                        Value(e->actual), Value(e->estimate), Value(e->previous), Value(e->unit),
                        e->timezone, e->status, e->importance, e->quality);
    GtkWidget *summary = Label(values);
    g_free(values);
    gtk_widget_add_css_class(summary, "dim-label");
    gtk_box_append(GTK_BOX(box), summary);
    gtk_widget_set_margin_top(box, 6);
    gtk_widget_set_margin_bottom(box, 6);
    gtk_widget_set_margin_start(box, 8);
    gtk_widget_set_margin_end(box, 8);
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), box);
    g_object_set_data(G_OBJECT(row), "event-index", GUINT_TO_POINTER((guint)indexes[i] + 1U));
    gtk_list_box_append(GTK_LIST_BOX(s->rows), row);
  }
  gtk_widget_set_sensitive(s->previous, !s->detached && s->offset > 0U);
  gtk_widget_set_sensitive(s->next, !s->detached && s->offset + count < total);
  char *message;
  if (s->calendar == NULL)
    message = g_strdup("No calendar loaded. Import an economic or earnings CSV snapshot. No "
                       "live feed is connected.");
  else if (total == 0U)
    message = g_strdup("No matching imported events. This does not prove that no events are "
                       "scheduled; check filters and source coverage.");
  else
    message = g_strdup_printf("Showing %zu-%zu of %zu matching imported events. Source dates and "
                              "times; no timezone conversion. Select a row for source details.",
                              s->offset + 1U, s->offset + count, total);
  Message(s, message);
  g_free(message);
}
static bool CopyEntry(GtkWidget *w, char *out, size_t capacity) {
  const char *value = gtk_editable_get_text(GTK_EDITABLE(w));
  size_t n = strlen(value);
  if (n >= capacity)
    return false;
  memcpy(out, value, n + 1U);
  return true;
}
static void Apply(GtkButton *button, gpointer root) {
  (void)button;
  CalendarUi *s = State(root);
  if (s == NULL || s->detached)
    return;
  UmiEventCalendarFilter f = {0};
  f.kind = (UmiEventKind)gtk_drop_down_get_selected(GTK_DROP_DOWN(s->kind));
  f.minimum_importance = gtk_drop_down_get_selected(GTK_DROP_DOWN(s->impact));
  if (!CopyEntry(s->from, f.from, sizeof f.from) ||
      !CopyEntry(s->through, f.through, sizeof f.through) ||
      !CopyEntry(s->search, f.search, sizeof f.search) ||
      !CopyEntry(s->country, f.country, sizeof f.country) ||
      !CopyEntry(s->currency, f.currency, sizeof f.currency) ||
      UmiEventCalendarFilterValidate(&f) != UMI_STATUS_OK) {
    Message(s, "Filters not applied. Use YYYY-MM-DD, an ordered date range, and uppercase "
               "country/currency codes. Previous rows remain visible.");
    return;
  }
  s->applied = f;
  s->offset = 0U;
  Render(s);
}
/* These commands edit the same visible filter controls as manual entry. Today
 * uses the computer's local calendar date, not a converted exchange instant. */
static void Today(GtkButton *button, gpointer root) {
  (void)button;
  CalendarUi *s = State(root);
  if (s == NULL || s->detached)
    return;
  GDateTime *now = g_date_time_new_now_local();
  if (now == NULL) {
    Message(s, "Local date is unavailable. Enter a source date in Event filters.");
    return;
  }
  char *date = g_date_time_format(now, "%Y-%m-%d");
  gtk_editable_set_text(GTK_EDITABLE(s->from), date);
  gtk_editable_set_text(GTK_EDITABLE(s->through), date);
  g_free(date);
  g_date_time_unref(now);
  Apply(NULL, root);
}
static void ClearFilters(GtkButton *button, gpointer root) {
  (void)button;
  CalendarUi *s = State(root);
  if (s == NULL || s->detached)
    return;
  GtkWidget *entries[] = {s->from, s->through, s->search, s->country, s->currency};
  for (size_t i = 0U; i < sizeof entries / sizeof entries[0]; ++i)
    gtk_editable_set_text(GTK_EDITABLE(entries[i]), "");
  gtk_drop_down_set_selected(GTK_DROP_DOWN(s->kind), 0U);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(s->impact), 0U);
  Apply(NULL, root);
}
static void SearchActivated(GtkEntry *entry, gpointer root) {
  (void)entry;
  Apply(NULL, root);
}
static void Page(GtkButton *button, gpointer root) {
  CalendarUi *s = State(root);
  if (s == NULL || s->detached)
    return;
  if (GTK_WIDGET(button) == s->next) {
    if (s->offset + PAGE_SIZE >= s->total)
      return;
    s->offset += PAGE_SIZE;
  } else {
    if (s->offset == 0U)
      return;
    s->offset = s->offset > PAGE_SIZE ? s->offset - PAGE_SIZE : 0U;
  }
  Render(s);
}
static UmiStatus Load(CalendarUi *s, const void *bytes, size_t length) {
  UmiEventCalendar *next = NULL;
  size_t record = 0U;
  UmiStatus status = UmiEventCalendarReadCsv(bytes, length, &next, &record);
  if (status != UMI_STATUS_OK) {
    char *m = g_strdup_printf("Import refused at CSV record %zu: %s. Previous calendar retained.",
                              record, umi_status_text(status));
    Message(s, m);
    g_free(m);
    return status;
  }
  UmiEventCalendarDestroy(s->calendar);
  s->calendar = next;
  s->offset = 0U;
  Render(s);
  return UMI_STATUS_OK;
}
UmiStatus UmiGtk4EventCalendarLoadCsv(GtkWidget *panel, const void *bytes, size_t length) {
  CalendarUi *s = State(panel);
  if (s == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (s->detached)
    return UMI_STATUS_INVALID_STATE;
  if (s->loading || s->chooser != NULL)
    return UMI_STATUS_BUSY;
  return Load(s, bytes, length);
}
static void ReadFree(gpointer data) {
  CalendarRead *r = data;
  g_weak_ref_clear(&r->root);
  g_object_unref(r->file);
  g_free(r);
}
static void ReadWorker(GTask *task, gpointer source, gpointer task_data, GCancellable *cancel) {
  (void)source;
  CalendarRead *r = task_data;
  GError *error = NULL;
  GFileInputStream *stream = g_file_read(r->file, cancel, &error);
  if (stream == NULL) {
    g_task_return_error(task, error);
    return;
  }
  GByteArray *bytes = g_byte_array_new();
  guint8 chunk[8192];
  for (;;) {
    gssize n = g_input_stream_read(G_INPUT_STREAM(stream), chunk, sizeof chunk, cancel, &error);
    if (n <= 0)
      break;
    if ((size_t)n > UMI_EVENT_CALENDAR_MAX_BYTES - (size_t)bytes->len) {
      g_set_error_literal(&error, G_IO_ERROR, G_IO_ERROR_NO_SPACE,
                          "Calendar exceeds the 4 MiB import limit");
      break;
    }
    g_byte_array_append(bytes, chunk, (guint)n);
  }
  g_object_unref(stream);
  if (error != NULL) {
    g_byte_array_unref(bytes);
    g_task_return_error(task, error);
  } else
    g_task_return_pointer(task, g_byte_array_free_to_bytes(bytes), (GDestroyNotify)g_bytes_unref);
}
static void ReadFinished(GObject *source, GAsyncResult *result, gpointer unused) {
  (void)source;
  (void)unused;
  GTask *task = G_TASK(result);
  CalendarRead *r = g_task_get_task_data(task);
  GtkWidget *root = g_weak_ref_get(&r->root);
  GError *error = NULL;
  GBytes *bytes = g_task_propagate_pointer(task, &error);
  if (root != NULL) {
    CalendarUi *s = State(root);
    if (s != NULL && !s->detached && s->cancel == g_task_get_cancellable(task)) {
      s->loading = false;
      g_clear_object(&s->cancel);
      gtk_widget_set_sensitive(s->import, TRUE);
      gtk_widget_set_sensitive(s->cancel_button, FALSE);
      if (bytes != NULL) {
        gsize length = 0U;
        const void *data = g_bytes_get_data(bytes, &length);
        (void)Load(s, data, (size_t)length);
      } else
        Message(s, "File could not be read within the import limits. Previous calendar "
                   "retained; choose a readable local CSV file.");
    }
    g_object_unref(root);
  }
  g_clear_pointer(&bytes, g_bytes_unref);
  g_clear_error(&error);
}
/* All import entry points share one active cancellable identity. Cancelling
 * retires that identity before a later result can publish on the GTK context. */
UmiStatus UmiGtk4EventCalendarReadFile(GtkWidget *panel, GFile *file) {
  CalendarUi *s = State(panel);
  if (s == NULL || !G_IS_FILE(file))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (s->detached)
    return UMI_STATUS_INVALID_STATE;
  if (s->loading || s->chooser != NULL)
    return UMI_STATUS_BUSY;
  if (!g_file_is_native(file))
    return UMI_STATUS_PERMISSION_DENIED;
  s->loading = true;
  s->cancel = g_cancellable_new();
  gtk_widget_set_sensitive(s->import, FALSE);
  gtk_widget_set_sensitive(s->cancel_button, TRUE);
  Message(s, "Reading calendar file. Existing rows remain visible until the complete import is "
             "validated.");
  CalendarRead *r = g_new0(CalendarRead, 1);
  g_weak_ref_init(&r->root, G_OBJECT(panel));
  r->file = g_object_ref(file);
  GTask *task = g_task_new(NULL, s->cancel, ReadFinished, NULL);
  g_task_set_task_data(task, r, ReadFree);
  g_task_run_in_thread(task, ReadWorker);
  g_object_unref(task);
  return UMI_STATUS_OK;
}
void UmiGtk4EventCalendarCancelRead(GtkWidget *panel) {
  CalendarUi *s = State(panel);
  if (s == NULL || s->detached || !s->loading)
    return;
  g_cancellable_cancel(s->cancel);
  g_clear_object(&s->cancel);
  s->loading = false;
  gtk_widget_set_sensitive(s->import, TRUE);
  gtk_widget_set_sensitive(s->cancel_button, FALSE);
  Message(s, "Import cancelled. Previous calendar retained.");
}
bool UmiGtk4EventCalendarIsReading(GtkWidget *panel) {
  CalendarUi *s = State(panel);
  return s != NULL && !s->detached && s->loading;
}
static void Cancel(GtkButton *button, gpointer root) {
  (void)button;
  UmiGtk4EventCalendarCancelRead(root);
}
static void Chosen(GtkNativeDialog *dialog, int response, gpointer root) {
  CalendarUi *s = State(root);
  if (s == NULL || s->chooser != GTK_FILE_CHOOSER_NATIVE(dialog))
    return;
  GFile *file =
      response == GTK_RESPONSE_ACCEPT ? gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog)) : NULL;
  s->chooser = NULL;
  gtk_native_dialog_destroy(dialog);
  g_object_unref(dialog);
  if (file == NULL)
    return;
  if (s->detached || !g_file_is_native(file)) {
    g_object_unref(file);
    return;
  }
  /* The chooser now delegates to the reusable asynchronous entry point so
   * cancellation and programmatic imports have identical ownership rules.
   * Its previous worker setup remains below for implementation review. */
#if 0
    s->loading = true;
    s->cancel = g_cancellable_new();
    gtk_widget_set_sensitive(s->import, FALSE);
    Message(s, "Reading calendar file. Existing rows remain visible until the complete import is "
               "validated.");
    CalendarRead *r = g_new0(CalendarRead, 1);
    g_weak_ref_init(&r->root, G_OBJECT(root));
    r->file = file;
    GTask *task = g_task_new(NULL, s->cancel, ReadFinished, NULL);
    g_task_set_task_data(task, r, ReadFree);
    g_task_run_in_thread(task, ReadWorker);
    g_object_unref(task);
#endif
  UmiStatus status = UmiGtk4EventCalendarReadFile(root, file);
  g_object_unref(file);
  if (status != UMI_STATUS_OK)
    Message(s, "Import could not start. Choose a local file after the current import completes.");
}
static void Import(GtkButton *button, gpointer root) {
  (void)button;
  CalendarUi *s = State(root);
  if (s == NULL || s->detached || s->loading || s->chooser != NULL)
    return;
  GtkRoot *native = gtk_widget_get_root(root);
  s->chooser = gtk_file_chooser_native_new("Import economic and earnings calendar",
                                           GTK_IS_WINDOW(native) ? GTK_WINDOW(native) : NULL,
                                           GTK_FILE_CHOOSER_ACTION_OPEN, "Import", "Cancel");
  GtkFileFilter *filter = gtk_file_filter_new();
  gtk_file_filter_set_name(filter, "Calendar CSV (*.csv)");
  gtk_file_filter_add_pattern(filter, "*.csv");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(s->chooser), filter);
  g_object_unref(filter);
  g_signal_connect_object(s->chooser, "response", G_CALLBACK(Chosen), G_OBJECT(root), 0);
  gtk_native_dialog_show(GTK_NATIVE_DIALOG(s->chooser));
}
static GtkWidget *Entry(GtkWidget *box, const char *caption, const char *id, int width) {
  GtkWidget *column = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_box_append(GTK_BOX(column), Label(caption));
  GtkWidget *entry = gtk_entry_new();
  gtk_editable_set_width_chars(GTK_EDITABLE(entry), width);
  gtk_widget_set_hexpand(entry, TRUE);
  gtk_accessible_update_property(GTK_ACCESSIBLE(entry), GTK_ACCESSIBLE_PROPERTY_LABEL, caption, -1);
  Tag(entry, id);
  gtk_box_append(GTK_BOX(column), entry);
  gtk_box_append(GTK_BOX(box), column);
  return entry;
}
GtkWidget *UmiGtk4EventCalendarCreate(void) {
  GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  CalendarUi *s = g_new0(CalendarUi, 1);
  g_object_set_data_full(G_OBJECT(root), "umicom-event-calendar", s, Free);
  Tag(root, "trading.calendar");
  gtk_box_append(GTK_BOX(root), Label("Economic & earnings calendar"));
  GtkWidget *filters = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  s->search = Entry(root, "Symbol, company, event or source", "trading.calendar.search", 16);
  GtkWidget *dates = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  s->from = Entry(dates, "From (YYYY-MM-DD)", "trading.calendar.from", 10);
  s->through = Entry(dates, "Through (YYYY-MM-DD)", "trading.calendar.through", 10);
  gtk_box_append(GTK_BOX(filters), dates);
  GtkWidget *codes = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  s->country = Entry(codes, "Country (US, GB...)", "trading.calendar.country", 3);
  s->currency = Entry(codes, "Currency (USD...)", "trading.calendar.currency", 4);
  gtk_box_append(GTK_BOX(filters), codes);
  const char *kinds[] = {"All event types", "Economic releases", "Company earnings", NULL};
  s->kind = gtk_drop_down_new_from_strings(kinds);
  Tag(s->kind, "trading.calendar.kind");
  gtk_accessible_update_property(GTK_ACCESSIBLE(s->kind), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                 "Calendar event type", -1);
  gtk_box_append(GTK_BOX(filters), s->kind);
  const char *impacts[] = {"Any importance", "Low or higher", "Medium or higher", "High", NULL};
  s->impact = gtk_drop_down_new_from_strings(impacts);
  Tag(s->impact, "trading.calendar.importance");
  gtk_accessible_update_property(GTK_ACCESSIBLE(s->impact), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                 "Minimum source importance", -1);
  gtk_box_append(GTK_BOX(filters), s->impact);
  GtkWidget *expander = gtk_expander_new("Event filters — Apply updates the view");
  gtk_expander_set_expanded(GTK_EXPANDER(expander), FALSE);
  gtk_expander_set_child(GTK_EXPANDER(expander), filters);
  gtk_box_append(GTK_BOX(root), expander);
  GtkWidget *shortcuts = UmiGtk4ActionGroupCreate();
  GtkWidget *today = gtk_button_new_with_label("Today (local date)");
  GtkWidget *clear = gtk_button_new_with_label("Clear filters");
  Tag(today, "trading.calendar.today");
  Tag(clear, "trading.calendar.clear");
  (void)UmiGtk4ActionGroupAppend(shortcuts, today);
  (void)UmiGtk4ActionGroupAppend(shortcuts, clear);
  gtk_box_append(GTK_BOX(root), shortcuts);
  g_signal_connect_object(today, "clicked", G_CALLBACK(Today), G_OBJECT(root), 0);
  g_signal_connect_object(clear, "clicked", G_CALLBACK(ClearFilters), G_OBJECT(root), 0);
  g_signal_connect_object(s->search, "activate", G_CALLBACK(SearchActivated), G_OBJECT(root), 0);
  /* Shared action groups reflow without replacing focused controls. Advanced
   * filters start collapsed so smaller docked views retain room for events. */
  GtkWidget *actions = UmiGtk4ActionGroupCreate();
  s->import = gtk_button_new_with_label("Import calendar CSV");
  s->apply = gtk_button_new_with_label("Apply filters");
  s->cancel_button = gtk_button_new_with_label("Cancel import");
  gtk_widget_set_sensitive(s->cancel_button, FALSE);
  (void)UmiGtk4ActionGroupAppend(actions, s->import);
  (void)UmiGtk4ActionGroupAppend(actions, s->apply);
  (void)UmiGtk4ActionGroupAppend(actions, s->cancel_button);
  gtk_box_append(GTK_BOX(root), actions);
  s->message = Label("");
  Tag(s->message, "trading.calendar.message");
  gtk_box_append(GTK_BOX(root), s->message);
  GtkWidget *split = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
  gtk_paned_set_resize_start_child(GTK_PANED(split), TRUE);
  gtk_paned_set_resize_end_child(GTK_PANED(split), TRUE);
  gtk_paned_set_shrink_start_child(GTK_PANED(split), TRUE);
  gtk_paned_set_shrink_end_child(GTK_PANED(split), TRUE);
  s->rows = gtk_list_box_new();
  gtk_list_box_set_selection_mode(GTK_LIST_BOX(s->rows), GTK_SELECTION_SINGLE);
  Tag(s->rows, "trading.calendar.rows");
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), s->rows);
  gtk_paned_set_start_child(GTK_PANED(split), scroll);
  s->details = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(s->details), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(s->details), GTK_WRAP_WORD_CHAR);
  Tag(s->details, "trading.calendar.details");
  GtkWidget *details_scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(details_scroll), s->details);
  gtk_paned_set_end_child(GTK_PANED(split), details_scroll);
  gtk_widget_set_vexpand(split, TRUE);
  gtk_box_append(GTK_BOX(root), split);
  GtkWidget *pages = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  s->previous = gtk_button_new_with_label("Previous page");
  s->next = gtk_button_new_with_label("Next page");
  gtk_box_append(GTK_BOX(pages), s->previous);
  gtk_box_append(GTK_BOX(pages), s->next);
  gtk_box_append(GTK_BOX(root), pages);
  Tag(s->import, "trading.calendar.import");
  Tag(s->apply, "trading.calendar.apply");
  Tag(s->cancel_button, "trading.calendar.cancel");
  Tag(s->previous, "trading.calendar.previous");
  Tag(s->next, "trading.calendar.next");
  g_signal_connect_object(s->apply, "clicked", G_CALLBACK(Apply), G_OBJECT(root), 0);
  g_signal_connect_object(s->cancel_button, "clicked", G_CALLBACK(Cancel), G_OBJECT(root), 0);
  g_signal_connect_object(s->import, "clicked", G_CALLBACK(Import), G_OBJECT(root), 0);
  g_signal_connect_object(s->previous, "clicked", G_CALLBACK(Page), G_OBJECT(root), 0);
  g_signal_connect_object(s->next, "clicked", G_CALLBACK(Page), G_OBJECT(root), 0);
  g_signal_connect_object(s->rows, "row-selected", G_CALLBACK(Selected), G_OBJECT(root), 0);
  Render(s);
  return root;
}
void UmiGtk4EventCalendarDetach(GtkWidget *panel) {
  CalendarUi *s = State(panel);
  if (s == NULL || s->detached)
    return;
  s->detached = true;
  if (s->cancel != NULL)
    g_cancellable_cancel(s->cancel);
  if (s->chooser != NULL) {
    GtkFileChooserNative *chooser = s->chooser;
    s->chooser = NULL;
    gtk_native_dialog_destroy(GTK_NATIVE_DIALOG(chooser));
    g_object_unref(chooser);
  }
  gtk_widget_set_sensitive(panel, FALSE);
  Message(s, "Workspace closed. Imported evidence remains visible; calendar actions are disabled.");
}
