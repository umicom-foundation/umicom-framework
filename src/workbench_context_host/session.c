/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/session.c
 *
 * PURPOSE:
 *   Implement context-host assignment capture, restore and portable text encoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/session.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void umi_workbench_context_host_session_init(
    UmiWorkbenchContextHostSession *session)
{
    if (session == NULL) return;
    memset(session, 0, sizeof(*session));
    session->structure_size = (uint32_t)sizeof(*session);
}

UmiStatus umi_workbench_context_host_session_capture(
    const UmiWorkbenchContextHost *host,
    UmiWorkbenchContextHostSession *out_session)
{
    size_t index;
    if (host == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_context_host_session_init(out_session);
    (void)umi_workbench_context_host_copy_text(
        out_session->host_id,
        sizeof(out_session->host_id),
        host->host_id);
    (void)umi_workbench_context_host_copy_text(
        out_session->active_group_id,
        sizeof(out_session->active_group_id),
        host->active_group_id);

    if (host->endpoints.count >
        UMI_WORKBENCH_CONTEXT_HOST_MAX_SESSION_ASSIGNMENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < host->endpoints.count; ++index) {
        const UmiWorkbenchContextHostEndpoint *endpoint =
            &host->endpoints.items[index];
        UmiWorkbenchContextHostSessionAssignment *assignment =
            &out_session->assignments[out_session->assignment_count++];
        (void)umi_workbench_context_host_copy_text(
            assignment->endpoint_id,
            sizeof(assignment->endpoint_id),
            endpoint->endpoint_id);
        (void)umi_workbench_context_host_copy_text(
            assignment->group_id,
            sizeof(assignment->group_id),
            endpoint->group_id);
        assignment->mode = endpoint->mode;
    }

    out_session->source_revision = host->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_session_apply(
    UmiWorkbenchContextHost *host,
    const UmiWorkbenchContextHostSession *session)
{
    size_t index;
    UmiStatus status;
    if (host == NULL || session == NULL ||
        session->structure_size != sizeof(*session)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (session->host_id[0] != '\0' &&
        strcmp(session->host_id, host->host_id) != 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < session->assignment_count; ++index) {
        const UmiWorkbenchContextHostSessionAssignment *assignment =
            &session->assignments[index];
        if (assignment->group_id[0] == '\0') continue;
        status = umi_workbench_context_host_assign_endpoint_group(
            host,
            assignment->endpoint_id,
            assignment->group_id,
            assignment->mode);
        if (status != UMI_STATUS_OK &&
            status != UMI_STATUS_NOT_FOUND) {
            return status;
        }
    }

    if (session->active_group_id[0] != '\0') {
        status = umi_workbench_context_host_set_active_group(
            host,
            session->active_group_id);
        if (status != UMI_STATUS_OK &&
            status != UMI_STATUS_NOT_FOUND) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

static bool safe_field(const char *text)
{
    if (text == NULL) return false;
    while (*text != '\0') {
        if (*text == '|' || *text == '\n' || *text == '\r') {
            return false;
        }
        ++text;
    }
    return true;
}

UmiStatus umi_workbench_context_host_session_encode(
    const UmiWorkbenchContextHostSession *session,
    char *out_text,
    size_t capacity)
{
    size_t used = 0U;
    size_t index;
    int written;

    if (session == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!safe_field(session->host_id) ||
        !safe_field(session->active_group_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    written = snprintf(
        out_text,
        capacity,
        "UMICOM-WORKBENCH-CONTEXT-HOST|1\n"
        "HOST|%s\n"
        "ACTIVE|%s\n",
        session->host_id,
        session->active_group_id);
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    used = (size_t)written;

    for (index = 0U; index < session->assignment_count; ++index) {
        const UmiWorkbenchContextHostSessionAssignment *assignment =
            &session->assignments[index];
        if (!safe_field(assignment->endpoint_id) ||
            !safe_field(assignment->group_id)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        written = snprintf(
            out_text + used,
            capacity - used,
            "ASSIGN|%s|%s|%u\n",
            assignment->endpoint_id,
            assignment->group_id,
            (unsigned)assignment->mode);
        if (written < 0 || (size_t)written >= capacity - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
    }
    return UMI_STATUS_OK;
}

static UmiStatus copy_segment(
    char *destination,
    size_t capacity,
    const char *begin,
    const char *end)
{
    size_t length;
    if (destination == NULL || begin == NULL ||
        end == NULL || end < begin) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = (size_t)(end - begin);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, begin, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_session_decode(
    const char *text,
    UmiWorkbenchContextHostSession *out_session)
{
    const char *cursor;
    size_t line_index = 0U;

    if (text == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_context_host_session_init(out_session);
    cursor = text;

    while (*cursor != '\0') {
        const char *end = strchr(cursor, '\n');
        const char *line_end =
            end != NULL ? end : cursor + strlen(cursor);

        if (line_index == 0U) {
            static const char header[] =
                "UMICOM-WORKBENCH-CONTEXT-HOST|1";
            if ((size_t)(line_end - cursor) != sizeof(header) - 1U ||
                strncmp(cursor, header, sizeof(header) - 1U) != 0) {
                return UMI_STATUS_PARSE_ERROR;
            }
        } else if (strncmp(cursor, "HOST|", 5U) == 0) {
            UmiStatus status = copy_segment(
                out_session->host_id,
                sizeof(out_session->host_id),
                cursor + 5U,
                line_end);
            if (status != UMI_STATUS_OK) return status;
        } else if (strncmp(cursor, "ACTIVE|", 7U) == 0) {
            UmiStatus status = copy_segment(
                out_session->active_group_id,
                sizeof(out_session->active_group_id),
                cursor + 7U,
                line_end);
            if (status != UMI_STATUS_OK) return status;
        } else if (strncmp(cursor, "ASSIGN|", 7U) == 0) {
            const char *assignment_begin = cursor + 7U;
            const char *first_separator =
                strchr(assignment_begin, '|');
            const char *second_separator;
            UmiWorkbenchContextHostSessionAssignment *assignment;
            char mode_text[16U];
            char *tail = NULL;
            unsigned long mode;

            if (first_separator == NULL ||
                first_separator >= line_end) {
                return UMI_STATUS_PARSE_ERROR;
            }
            second_separator = strchr(first_separator + 1U, '|');
            if (second_separator == NULL ||
                second_separator >= line_end) {
                return UMI_STATUS_PARSE_ERROR;
            }
            if (out_session->assignment_count >=
                UMI_WORKBENCH_CONTEXT_HOST_MAX_SESSION_ASSIGNMENTS) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }

            assignment = &out_session->assignments[
                out_session->assignment_count];
            if (copy_segment(
                    assignment->endpoint_id,
                    sizeof(assignment->endpoint_id),
                    assignment_begin,
                    first_separator) != UMI_STATUS_OK ||
                copy_segment(
                    assignment->group_id,
                    sizeof(assignment->group_id),
                    first_separator + 1U,
                    second_separator) != UMI_STATUS_OK ||
                copy_segment(
                    mode_text,
                    sizeof(mode_text),
                    second_separator + 1U,
                    line_end) != UMI_STATUS_OK) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }

            mode = strtoul(mode_text, &tail, 10);
            if (tail == mode_text || *tail != '\0' || mode > 3UL) {
                return UMI_STATUS_PARSE_ERROR;
            }
            assignment->mode =
                (UmiWorkbenchContextLinkMode)mode;
            ++out_session->assignment_count;
        }

        ++line_index;
        if (end == NULL) break;
        cursor = end + 1U;
    }

    return line_index > 0U
        ? UMI_STATUS_OK
        : UMI_STATUS_PARSE_ERROR;
}
