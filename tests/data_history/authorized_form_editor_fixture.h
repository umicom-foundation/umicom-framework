/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/authorized_form_editor_fixture.h
 *
 * PURPOSE:
 *   Native tests use fictional data, a controlled atomic clock and real host workers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native tests use fictional data, a controlled atomic clock and real host workers. */
#ifndef UMICOM_AUTHORIZED_FORM_EDITOR_FIXTURE_H
#define UMICOM_AUTHORIZED_FORM_EDITOR_FIXTURE_H
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "record_host_fixture.h"
#include "umicom/ui/gtk4/authorized_form_editor.h"
#include "umicom/ui/gtk4/structured_form.h"
#include <stdatomic.h>
typedef struct NativeAccessFixture {
  FormFixture *values;
  UmiRecordHost *host;
  GtkWidget *editor;
  UmiClock clock;
  atomic_uint_fast64_t now;
  UmiSessionRecord session;
  UmiRecordAccessRequest request;
  UmiAuthorizedRecordRevision seeded;
  UmiAuthorizedFormRecord *loaded;
} NativeAccessFixture;
static inline uint64_t native_clock(const UmiClock *clock) {
  NativeAccessFixture *f = clock->state;
  return atomic_load_explicit(&f->now, memory_order_relaxed);
}
static inline GtkWidget *native_find(GtkWidget *widget, const char *name) {
  if (!widget)
    return NULL;
  if (!strcmp(gtk_widget_get_name(widget), name))
    return widget;
  for (GtkWidget *child = gtk_widget_get_first_child(widget); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *found = native_find(child, name);
    if (found)
      return found;
  }
  return NULL;
}
static inline bool native_idle(GtkWidget *editor) {
  gint64 until = g_get_monotonic_time() + 15 * G_USEC_PER_SEC;
  while (UmiAuthorizedFormEditorGtk4Busy(editor) && g_get_monotonic_time() < until) {
    g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  return !UmiAuthorizedFormEditorGtk4Busy(editor);
}
static inline UmiStatus native_wait(NativeAccessFixture *f) {
  if (!native_idle(f->editor))
    return UMI_STATUS_BUSY;
  UmiAuthorizedFormEditorGtk4State state;
  UmiStatus s = UmiAuthorizedFormEditorGtk4ReadState(f->editor, &state);
  return s == UMI_STATUS_OK ? state.last_status : s;
}
static inline UmiStatus native_open(NativeAccessFixture *f) {
  UmiStatus s = UmiAuthorizedFormEditorGtk4Open(f->editor, "example.stored");
  return s == UMI_STATUS_OK ? native_wait(f) : s;
}
static inline void native_mass(NativeAccessFixture *f, const char *text) {
  GtkWidget *state = native_find(f->editor, "parcel.mass.state");
  GtkWidget *value = native_find(f->editor, "parcel.mass.value");
  gtk_drop_down_set_selected(GTK_DROP_DOWN(state), (guint)UMI_FORM_KNOWN);
  gtk_editable_set_text(GTK_EDITABLE(value), text);
}
static inline UmiStatus native_read(NativeAccessFixture *f, const char *entity) {
  UmiRecordAccessRequest request = f->request;
  strcpy(request.entity_id, entity);
  request.operation = UMI_RECORD_ACCESS_READ_HEAD;
  return umi_record_host_form_load_current(f->host, &request, 0, &f->values->schema, f->loaded);
}
static inline UmiStatus native_new(const char *backend, NativeAccessFixture **out) {
  NativeAccessFixture *f = calloc(1, sizeof *f);
  if (!f)
    return UMI_STATUS_OUT_OF_MEMORY;
  *out = f;
  atomic_init(&f->now, 100);
  f->clock = (UmiClock){.monotonic_nanoseconds = native_clock, .state = f};
  f->values = form_fixture_new();
  f->loaded = malloc(sizeof *f->loaded);
  if (!f->values || !f->loaded)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiRecordHostOptions options = {.backend = !strcmp(backend, "sqlite") ? UMI_RECORD_HOST_SQLITE
                                                                        : UMI_RECORD_HOST_MEMORY,
                                  .database_path = !strcmp(backend, "sqlite") ? ":memory:" : NULL,
                                  .source = "Fictional native host",
                                  .entity_prefix = "example."};
  UmiStatus s = umi_record_host_create_with_clock(&options, &f->clock, &f->host);
  UmiIdentityRecord identity = {
      .identity_id = "example.editor", .display_name = "Fictional editor", .enabled = 1};
  if (s == UMI_STATUS_OK)
    s = umi_record_host_register_identity(f->host, &identity);
  uint64_t revision;
  if (s == UMI_STATUS_OK)
    s = host_policy(f->host, UMI_POLICY_ALLOW, &revision);
  if (s == UMI_STATUS_OK)
    s = umi_record_host_issue_current_session(f->host, identity.identity_id, 1000, &f->session);
  f->request = (UmiRecordAccessRequest){
      .entity_id = "example.stored", .purpose = "review", .operation = UMI_RECORD_ACCESS_CREATE};
  strcpy(f->request.session_id, f->session.session_id);
  atomic_store_explicit(&f->now, 101, memory_order_relaxed);
  UmiFormAnswer answer = form_mass(3.5);
  if (s == UMI_STATUS_OK)
    s = umi_structured_form_set(&f->values->schema, &f->values->before, 1, "parcel.mass", &answer);
  if (s == UMI_STATUS_OK)
    s = umi_record_host_form_save_current(f->host, &f->request, &f->values->schema,
                                          &f->values->before, "Seed fictional form", 0, 0,
                                          &f->seeded);
  UmiAuthorizedFormEditorGtk4Options editor = {f->host, &f->values->schema, f->session.session_id,
                                               "review"};
  if (s == UMI_STATUS_OK)
    s = UmiAuthorizedFormEditorGtk4New(&editor, &f->editor);
  if (s == UMI_STATUS_OK)
    g_object_ref_sink(f->editor);
  return s;
}
static inline bool native_free(NativeAccessFixture *f) {
  if (!f)
    return true;
  /* A timed-out worker still borrows schema and clock state. Keep the fixture
   * alive until the failing test exits rather than freeing its dependencies. */
  if (f->editor && !native_idle(f->editor))
    return false;
  if (f->editor) {
    UmiAuthorizedFormEditorGtk4Retire(f->editor);
    g_object_unref(f->editor);
  }
  umi_record_host_release(f->host);
  free(f->loaded);
  free(f->values);
  free(f);
  return true;
}
#endif
