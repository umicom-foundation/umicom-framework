/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout_data/session_store.c
 *
 * PURPOSE:
 *   Implement chunked serialisation and optimistic Data Server persistence for workbench sessions and open panel state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout_data/session_store.h"
#include "umicom/workbench_layout_data/value_codec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "internal.h"




typedef struct SessionWriter {
    char *text;
    size_t capacity;
    size_t required;
    bool overflow;
} SessionWriter;

static void writer_append(SessionWriter *writer, const char *text)
{
    const size_t length = strlen(text);
    if (writer->text != NULL && writer->required + length < writer->capacity) {
        (void)memcpy(writer->text + writer->required, text, length);
    } else if (writer->text != NULL) {
        writer->overflow = true;
    }
    writer->required += length;
}

static void writer_field(
    SessionWriter *writer,
    const char *name,
    const char *value)
{
    size_t required = 0U;
    char *escaped;
    if (umi_workbench_layout_data_value_escape(
            value != NULL ? value : "", NULL, 0U, &required) !=
        UMI_STATUS_OK) {
        writer->overflow = true;
        return;
    }
    escaped = (char *)calloc(required, sizeof(char));
    if (escaped == NULL) {
        writer->overflow = true;
        return;
    }
    if (umi_workbench_layout_data_value_escape(
            value != NULL ? value : "",
            escaped, required, NULL) == UMI_STATUS_OK) {
        writer_append(writer, name);
        writer_append(writer, "=");
        writer_append(writer, escaped);
        writer_append(writer, "\n");
    } else {
        writer->overflow = true;
    }
    free(escaped);
}

static void writer_u64(
    SessionWriter *writer,
    const char *name,
    uint64_t value)
{
    char text[32];
    (void)snprintf(text, sizeof(text), "%llu",
                   (unsigned long long)value);
    writer_field(writer, name, text);
}

static void writer_bool(
    SessionWriter *writer,
    const char *name,
    bool value)
{
    writer_field(writer, name, value ? "true" : "false");
}

static void writer_panel(
    SessionWriter *writer,
    size_t index,
    const UmiWorkbenchOpenPanel *panel)
{
    char prefix[48];
    char name[80];
    (void)snprintf(prefix, sizeof(prefix), "panel.%zu.", index);
#define WRITE_PANEL_FIELD(field) \
    do { \
        (void)snprintf(name, sizeof(name), "%s" #field, prefix); \
        writer_field(writer, name, panel->field); \
    } while (0)
    WRITE_PANEL_FIELD(instance_id);
    WRITE_PANEL_FIELD(panel_id);
    WRITE_PANEL_FIELD(owner_application_id);
    WRITE_PANEL_FIELD(node_id);
    WRITE_PANEL_FIELD(context_group_id);
    WRITE_PANEL_FIELD(state_reference);
#undef WRITE_PANEL_FIELD
    (void)snprintf(name, sizeof(name), "%svisible", prefix);
    writer_bool(writer, name, panel->visible);
    (void)snprintf(name, sizeof(name), "%sactive", prefix);
    writer_bool(writer, name, panel->active);
    (void)snprintf(name, sizeof(name), "%sdirty", prefix);
    writer_bool(writer, name, panel->dirty);
    (void)snprintf(name, sizeof(name), "%srevision", prefix);
    writer_u64(writer, name, panel->revision);
}

UmiStatus umi_workbench_layout_session_encode(
    const UmiWorkbenchLayoutSession *session,
    char **out_text,
    size_t *out_size)
{
    SessionWriter writer;
    size_t index;
    if (session == NULL || out_text == NULL ||
        session->structure_size < sizeof(*session)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_text = NULL;
    if (out_size != NULL) *out_size = 0U;
    (void)memset(&writer, 0, sizeof(writer));
    writer_field(&writer, "schema", "umicom.workbench-session/1");
    writer_field(&writer, "session_id", session->session_id);
    writer_field(&writer, "user_id", session->user_id);
    writer_field(&writer, "workspace_id", session->workspace_id);
    writer_field(&writer, "active_application_id",
                 session->active_application_id);
    writer_field(&writer, "active_layout_id",
                 session->active_layout_id);
    writer_field(&writer, "active_node_id",
                 session->active_node_id);
    writer_field(&writer, "active_panel_instance_id",
                 session->active_panel_instance_id);
    writer_u64(&writer, "panel_count", (uint64_t)session->panel_count);
    writer_u64(&writer, "started_at_ms", session->started_at_ms);
    writer_u64(&writer, "last_checkpoint_at_ms",
               session->last_checkpoint_at_ms);
    writer_u64(&writer, "revision", session->revision);
    writer_bool(&writer, "clean_shutdown", session->clean_shutdown);
    writer_bool(&writer, "recovery_available",
                session->recovery_available);
    for (index = 0U; index < session->panel_count; ++index) {
        writer_panel(&writer, index, &session->panels[index]);
    }
    if (writer.overflow) return UMI_STATUS_INTERNAL_ERROR;
    writer.capacity = writer.required + 1U;
    writer.text = (char *)calloc(writer.capacity, sizeof(char));
    if (writer.text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    writer.required = 0U;
    writer_field(&writer, "schema", "umicom.workbench-session/1");
    writer_field(&writer, "session_id", session->session_id);
    writer_field(&writer, "user_id", session->user_id);
    writer_field(&writer, "workspace_id", session->workspace_id);
    writer_field(&writer, "active_application_id",
                 session->active_application_id);
    writer_field(&writer, "active_layout_id",
                 session->active_layout_id);
    writer_field(&writer, "active_node_id",
                 session->active_node_id);
    writer_field(&writer, "active_panel_instance_id",
                 session->active_panel_instance_id);
    writer_u64(&writer, "panel_count", (uint64_t)session->panel_count);
    writer_u64(&writer, "started_at_ms", session->started_at_ms);
    writer_u64(&writer, "last_checkpoint_at_ms",
               session->last_checkpoint_at_ms);
    writer_u64(&writer, "revision", session->revision);
    writer_bool(&writer, "clean_shutdown", session->clean_shutdown);
    writer_bool(&writer, "recovery_available",
                session->recovery_available);
    for (index = 0U; index < session->panel_count; ++index) {
        writer_panel(&writer, index, &session->panels[index]);
    }
    if (writer.overflow) {
        free(writer.text);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    writer.text[writer.required] = '\0';
    *out_text = writer.text;
    if (out_size != NULL) *out_size = writer.required;
    return UMI_STATUS_OK;
}

static UmiStatus parse_fields(
    const char *text,
    UmiWorkbenchLayoutDataFieldSet *out_fields)
{
    return umi_workbench_layout_data_value_decode(text, out_fields);
}

static UmiStatus copy_field(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    const char *name,
    char *destination,
    size_t capacity,
    bool allow_empty)
{
    const char *value =
        umi_workbench_layout_data_field_set_get(fields, name);
    return value != NULL
        ? umi_workbench_layout_data_copy_text(
            destination, capacity, value, allow_empty)
        : UMI_STATUS_NOT_FOUND;
}

static UmiStatus panel_from_fields(
    const UmiWorkbenchLayoutDataFieldSet *fields,
    size_t index,
    UmiWorkbenchOpenPanel *panel)
{
    char prefix[48];
    char name[80];
    uint64_t revision = 0U;
    UmiStatus status;
    (void)memset(panel, 0, sizeof(*panel));
    panel->structure_size = sizeof(*panel);
    (void)snprintf(prefix, sizeof(prefix), "panel.%zu.", index);
#define READ_PANEL_FIELD(field, allow_empty) \
    do { \
        (void)snprintf(name, sizeof(name), "%s" #field, prefix); \
        status = copy_field(fields, name, panel->field, \
                            sizeof(panel->field), allow_empty); \
        if (status != UMI_STATUS_OK) return status; \
    } while (0)
    READ_PANEL_FIELD(instance_id, false);
    READ_PANEL_FIELD(panel_id, false);
    READ_PANEL_FIELD(owner_application_id, true);
    READ_PANEL_FIELD(node_id, true);
    READ_PANEL_FIELD(context_group_id, true);
    READ_PANEL_FIELD(state_reference, true);
#undef READ_PANEL_FIELD
    (void)snprintf(name, sizeof(name), "%svisible", prefix);
    status = umi_workbench_layout_data_field_set_get_bool(
        fields, name, &panel->visible);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(name, sizeof(name), "%sactive", prefix);
    status = umi_workbench_layout_data_field_set_get_bool(
        fields, name, &panel->active);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(name, sizeof(name), "%sdirty", prefix);
    status = umi_workbench_layout_data_field_set_get_bool(
        fields, name, &panel->dirty);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(name, sizeof(name), "%srevision", prefix);
    status = umi_workbench_layout_data_field_set_get_u64(
        fields, name, &revision);
    panel->revision = revision;
    return status;
}

UmiStatus umi_workbench_layout_session_decode(
    const char *text,
    UmiWorkbenchLayoutSession *out_session)
{
    UmiWorkbenchLayoutDataFieldSet fields;
    const char *schema;
    uint64_t panel_count = 0U;
    size_t index;
    UmiStatus status;
    if (text == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_session, 0, sizeof(*out_session));
    out_session->structure_size = sizeof(*out_session);
    status = parse_fields(text, &fields);
    schema = umi_workbench_layout_data_field_set_get(&fields, "schema");
    if (status == UMI_STATUS_OK &&
        (schema == NULL ||
         strcmp(schema, "umicom.workbench-session/1") != 0)) {
        status = UMI_STATUS_PARSE_ERROR;
    }
#define READ_SESSION_FIELD(field, allow_empty) \
    if (status == UMI_STATUS_OK) { \
        status = copy_field(&fields, #field, out_session->field, \
                            sizeof(out_session->field), allow_empty); \
    }
    READ_SESSION_FIELD(session_id, false)
    READ_SESSION_FIELD(user_id, true)
    READ_SESSION_FIELD(workspace_id, true)
    READ_SESSION_FIELD(active_application_id, true)
    READ_SESSION_FIELD(active_layout_id, true)
    READ_SESSION_FIELD(active_node_id, true)
    READ_SESSION_FIELD(active_panel_instance_id, true)
#undef READ_SESSION_FIELD
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_u64(
            &fields, "panel_count", &panel_count);
        if (panel_count > UMI_WORKBENCH_LAYOUT_MAX_OPEN_PANELS) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
#define READ_SESSION_U64(field) \
    if (status == UMI_STATUS_OK) { \
        status = umi_workbench_layout_data_field_set_get_u64( \
            &fields, #field, &out_session->field); \
    }
    READ_SESSION_U64(started_at_ms)
    READ_SESSION_U64(last_checkpoint_at_ms)
    READ_SESSION_U64(revision)
#undef READ_SESSION_U64
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_bool(
            &fields, "clean_shutdown", &out_session->clean_shutdown);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_data_field_set_get_bool(
            &fields, "recovery_available",
            &out_session->recovery_available);
    }
    for (index = 0U;
         status == UMI_STATUS_OK && index < (size_t)panel_count;
         ++index) {
        status = panel_from_fields(
            &fields, index, &out_session->panels[index]);
    }
    if (status == UMI_STATUS_OK) {
        out_session->panel_count = (size_t)panel_count;
        status = umi_workbench_layout_session_validate(out_session);
    }
    return status;
}

UmiStatus umi_workbench_layout_session_store_init(
    UmiWorkbenchLayoutSessionStore *store,
    UmiDataServer *server)
{
    if (store == NULL || server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(store, 0, sizeof(*store));
    store->structure_size = sizeof(*store);
    store->server = server;
    return umi_workbench_layout_chunk_store_init(
        &store->chunks,
        server,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_MANIFEST,
        UMI_WORKBENCH_LAYOUT_DATA_RECORD_SESSION_CHUNK);
}

static UmiStatus session_revision(
    const UmiWorkbenchLayoutSessionStore *store,
    const char *session_id,
    uint64_t *out_revision)
{
    UmiWorkbenchLayoutDataChunkManifest manifest;
    UmiStatus status;
    *out_revision = 0U;
    status = umi_workbench_layout_chunk_store_verify(
        &store->chunks, session_id, &manifest);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;
    *out_revision = manifest.revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_session_store_save(
    const UmiWorkbenchLayoutSessionStore *store,
    const UmiWorkbenchLayoutSession *session,
    uint64_t expected_revision,
    uint64_t *out_revision)
{
    UmiWorkbenchLayoutSession copy;
    char *text = NULL;
    uint64_t stored_revision = 0U;
    UmiStatus status;
    if (store == NULL || session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = session_revision(
        store, session->session_id, &stored_revision);
    if (status != UMI_STATUS_OK) return status;
    if (expected_revision != stored_revision) {
        return expected_revision == 0U && stored_revision > 0U
            ? UMI_STATUS_ALREADY_EXISTS
            : UMI_STATUS_INVALID_STATE;
    }
    copy = *session;
    copy.revision = stored_revision + 1U;
    status = umi_workbench_layout_session_encode(
        &copy, &text, NULL);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_chunk_store_save(
            &store->chunks, copy.session_id, text,
            copy.revision, copy.last_checkpoint_at_ms, NULL);
    }
    free(text);
    if (status == UMI_STATUS_OK && out_revision != NULL) {
        *out_revision = copy.revision;
    }
    return status;
}

UmiStatus umi_workbench_layout_session_store_load(
    const UmiWorkbenchLayoutSessionStore *store,
    const char *session_id,
    UmiWorkbenchLayoutSession *out_session)
{
    char *text = NULL;
    UmiStatus status;
    if (store == NULL || session_id == NULL ||
        out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_chunk_store_load(
        &store->chunks, session_id, &text, NULL);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_session_decode(
            text, out_session);
    }
    free(text);
    return status;
}

UmiStatus umi_workbench_layout_session_store_delete(
    const UmiWorkbenchLayoutSessionStore *store,
    const char *session_id,
    uint64_t expected_revision)
{
    uint64_t stored_revision = 0U;
    UmiStatus status;
    if (store == NULL || session_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = session_revision(store, session_id, &stored_revision);
    if (status != UMI_STATUS_OK) return status;
    if (stored_revision == 0U) return UMI_STATUS_NOT_FOUND;
    if (expected_revision != stored_revision) {
        return UMI_STATUS_INVALID_STATE;
    }
    return umi_workbench_layout_chunk_store_delete(
        &store->chunks, session_id);
}
