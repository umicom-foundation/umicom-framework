/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/session_recovery.c
 *
 * PURPOSE:
 *   Implement checksummed recovery checkpoints and a portable journal format
 *   above the canonical Framework desktop persistence codec.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/session_recovery.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct UmiDesktopRecoveryCheckpoint {
    UmiDesktopRecoveryCheckpointSnapshot snapshot;
    char text[UMI_DESKTOP_PERSISTENCE_TEXT_CAPACITY];
} UmiDesktopRecoveryCheckpoint;

struct UmiDesktopSessionRecovery {
    UmiDesktopRuntime *runtime;
    UmiDesktopRecoveryCheckpoint
        checkpoints[UMI_DESKTOP_RECOVERY_CHECKPOINT_MAX];
    char session_id[UMI_DESKTOP_ID_CAPACITY];
    char application_id[UMI_DESKTOP_ID_CAPACITY];
    size_t checkpoint_count;
    UmiDesktopSessionRecoveryState state;
    bool clean_shutdown;
    uint64_t last_sequence;
    uint64_t revision;
};

static bool safe_text(const char *text)
{
    return text != NULL && text[0] != '\0' && strchr(text, '|') == NULL &&
        strchr(text, '\n') == NULL && strchr(text, '\r') == NULL;
}

static UmiStatus copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || !safe_text(source))
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static uint64_t checksum_text(const char *text, size_t length)
{
    uint64_t value = UINT64_C(1469598103934665603);
    size_t index;
    for (index = 0U; index < length; ++index) {
        value ^= (unsigned char)text[index];
        value *= UINT64_C(1099511628211);
    }
    return value;
}

static size_t find_checkpoint(
    const UmiDesktopSessionRecovery *recovery,
    const char *checkpoint_id)
{
    size_t index;
    if (recovery == NULL || checkpoint_id == NULL) return SIZE_MAX;
    for (index = 0U; index < recovery->checkpoint_count; ++index) {
        if (strcmp(recovery->checkpoints[index].snapshot.checkpoint_id,
                   checkpoint_id) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_desktop_session_recovery_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopSessionRecovery **out_recovery)
{
    UmiDesktopSessionRecovery *recovery;
    if (runtime == NULL || out_recovery == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_recovery = NULL;
    recovery = (UmiDesktopSessionRecovery *)calloc(1U, sizeof(*recovery));
    if (recovery == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    recovery->runtime = runtime;
    recovery->state = UMI_DESKTOP_SESSION_RECOVERY_IDLE;
    recovery->clean_shutdown = true;
    recovery->revision = 1U;
    *out_recovery = recovery;
    return UMI_STATUS_OK;
}

void umi_desktop_session_recovery_destroy(
    UmiDesktopSessionRecovery *recovery)
{
    free(recovery);
}

UmiStatus umi_desktop_session_recovery_begin(
    UmiDesktopSessionRecovery *recovery,
    const char *session_id,
    const char *application_id)
{
    UmiStatus status;
    if (recovery == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (recovery->state == UMI_DESKTOP_SESSION_RECOVERY_RUNNING)
        return UMI_STATUS_INVALID_STATE;
    if (recovery->state == UMI_DESKTOP_SESSION_RECOVERY_AVAILABLE)
        return UMI_STATUS_BUSY;
    status = copy_text(recovery->session_id,
                       sizeof(recovery->session_id), session_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(recovery->application_id,
                           sizeof(recovery->application_id), application_id);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(recovery->checkpoints, 0, sizeof(recovery->checkpoints));
    recovery->checkpoint_count = 0U;
    recovery->clean_shutdown = false;
    recovery->state = UMI_DESKTOP_SESSION_RECOVERY_RUNNING;
    recovery->revision += 1U;
    return umi_desktop_session_recovery_checkpoint(
        recovery, "session-start", "Session startup baseline");
}

UmiStatus umi_desktop_session_recovery_checkpoint(
    UmiDesktopSessionRecovery *recovery,
    const char *checkpoint_id,
    const char *reason)
{
    UmiDesktopRecoveryCheckpoint *checkpoint;
    size_t length = 0U;
    UmiStatus status;
    if (recovery == NULL || !safe_text(checkpoint_id) || !safe_text(reason))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (recovery->state != UMI_DESKTOP_SESSION_RECOVERY_RUNNING &&
        recovery->state != UMI_DESKTOP_SESSION_RECOVERY_RECOVERED)
        return UMI_STATUS_INVALID_STATE;
    if (find_checkpoint(recovery, checkpoint_id) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    if (recovery->checkpoint_count == UMI_DESKTOP_RECOVERY_CHECKPOINT_MAX) {
        (void)memmove(&recovery->checkpoints[0], &recovery->checkpoints[1],
                      (UMI_DESKTOP_RECOVERY_CHECKPOINT_MAX - 1U) *
                          sizeof(recovery->checkpoints[0]));
        recovery->checkpoint_count -= 1U;
    }
    checkpoint = &recovery->checkpoints[recovery->checkpoint_count];
    (void)memset(checkpoint, 0, sizeof(*checkpoint));
    status = umi_desktop_persistence_encode(
        recovery->runtime, checkpoint->text, sizeof(checkpoint->text),
        &length);
    if (status == UMI_STATUS_OK)
        status = copy_text(checkpoint->snapshot.checkpoint_id,
                           sizeof(checkpoint->snapshot.checkpoint_id),
                           checkpoint_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(checkpoint->snapshot.reason,
                           sizeof(checkpoint->snapshot.reason), reason);
    if (status != UMI_STATUS_OK) {
        (void)memset(checkpoint, 0, sizeof(*checkpoint));
        return status;
    }
    checkpoint->snapshot.structure_size =
        (uint32_t)sizeof(checkpoint->snapshot);
    checkpoint->snapshot.sequence = recovery->last_sequence + 1U;
    checkpoint->snapshot.encoded_length = length;
    checkpoint->snapshot.checksum = checksum_text(checkpoint->text, length);
    checkpoint->snapshot.revision = 1U;
    recovery->last_sequence = checkpoint->snapshot.sequence;
    recovery->checkpoint_count += 1U;
    recovery->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_session_recovery_mark_clean(
    UmiDesktopSessionRecovery *recovery)
{
    UmiStatus status;
    if (recovery == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (recovery->state != UMI_DESKTOP_SESSION_RECOVERY_RUNNING &&
        recovery->state != UMI_DESKTOP_SESSION_RECOVERY_RECOVERED)
        return UMI_STATUS_INVALID_STATE;
    status = umi_desktop_session_recovery_checkpoint(
        recovery, "clean-shutdown", "Clean session shutdown");
    if (status != UMI_STATUS_OK && status != UMI_STATUS_ALREADY_EXISTS)
        return status;
    recovery->clean_shutdown = true;
    recovery->state = UMI_DESKTOP_SESSION_RECOVERY_CLEAN;
    recovery->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_session_recovery_restore(
    UmiDesktopSessionRecovery *recovery,
    const char *checkpoint_id)
{
    size_t index;
    UmiDesktopRecoveryCheckpoint *checkpoint;
    UmiStatus status;
    if (recovery == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (recovery->checkpoint_count == 0U) return UMI_STATUS_NOT_FOUND;
    index = checkpoint_id != NULL && checkpoint_id[0] != '\0'
        ? find_checkpoint(recovery, checkpoint_id)
        : recovery->checkpoint_count - 1U;
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    checkpoint = &recovery->checkpoints[index];
    if (checksum_text(checkpoint->text,
                      checkpoint->snapshot.encoded_length) !=
        checkpoint->snapshot.checksum)
        return UMI_STATUS_PARSE_ERROR;
    status = umi_desktop_persistence_decode_transactional(
        recovery->runtime, checkpoint->text);
    if (status == UMI_STATUS_OK) {
        checkpoint->snapshot.revision += 1U;
        recovery->state = UMI_DESKTOP_SESSION_RECOVERY_RECOVERED;
        recovery->clean_shutdown = false;
        recovery->revision += 1U;
    }
    return status;
}

UmiStatus umi_desktop_session_recovery_export_latest(
    const UmiDesktopSessionRecovery *recovery,
    char *out_text,
    size_t capacity,
    size_t *out_length)
{
    const UmiDesktopRecoveryCheckpoint *checkpoint;
    int written;
    size_t header_length;
    size_t total_length;
    if (recovery == NULL || out_text == NULL || capacity == 0U ||
        out_length == NULL || recovery->checkpoint_count == 0U ||
        recovery->session_id[0] == '\0' ||
        recovery->application_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    checkpoint = &recovery->checkpoints[recovery->checkpoint_count - 1U];
    written = snprintf(
        out_text, capacity,
        "UMICOM-DESKTOP-RECOVERY|1\n"
        "SESSION|%s|%s|%d|%" PRIu64 "|%s|%s|%zu|%" PRIx64 "\n",
        recovery->session_id, recovery->application_id,
        recovery->clean_shutdown ? 1 : 0, checkpoint->snapshot.sequence,
        checkpoint->snapshot.checkpoint_id, checkpoint->snapshot.reason,
        checkpoint->snapshot.encoded_length, checkpoint->snapshot.checksum);
    if (written < 0) return UMI_STATUS_IO_ERROR;
    header_length = (size_t)written;
    total_length = header_length + checkpoint->snapshot.encoded_length +
        strlen("RECOVERY-END\n");
    if (header_length >= capacity || total_length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_text + header_length, checkpoint->text,
                 checkpoint->snapshot.encoded_length);
    (void)memcpy(out_text + header_length + checkpoint->snapshot.encoded_length,
                 "RECOVERY-END\n", strlen("RECOVERY-END\n") + 1U);
    *out_length = total_length;
    return UMI_STATUS_OK;
}

static size_t split_fields(char *line, char **fields, size_t capacity)
{
    size_t count = 0U;
    char *cursor = line;
    if (line == NULL || fields == NULL || capacity == 0U) return 0U;
    fields[count++] = cursor;
    while (*cursor != '\0') {
        if (*cursor == '|') {
            *cursor = '\0';
            if (count >= capacity) return capacity + 1U;
            fields[count++] = cursor + 1;
        }
        cursor += 1;
    }
    return count;
}

static UmiStatus parse_uint64_value(
    const char *text,
    int base,
    uint64_t *out_value)
{
    char *end = NULL;
    unsigned long long value;
    if (text == NULL || out_value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    errno = 0;
    value = strtoull(text, &end, base);
    if (errno != 0 || end == text || *end != '\0')
        return UMI_STATUS_PARSE_ERROR;
    *out_value = (uint64_t)value;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_session_recovery_import(
    UmiDesktopSessionRecovery *recovery,
    const char *text)
{
    static const char header[] = "UMICOM-DESKTOP-RECOVERY|1\n";
    static const char footer[] = "RECOVERY-END\n";
    const char *metadata_start;
    const char *metadata_end;
    const char *payload;
    char metadata[1024U];
    char *fields[9U];
    size_t metadata_length;
    size_t count;
    uint64_t clean;
    uint64_t sequence;
    uint64_t encoded_length;
    uint64_t checksum;
    UmiDesktopRecoveryCheckpoint imported;
    UmiStatus status;
    if (recovery == NULL || text == NULL ||
        strncmp(text, header, strlen(header)) != 0)
        return UMI_STATUS_PARSE_ERROR;
    metadata_start = text + strlen(header);
    metadata_end = strchr(metadata_start, '\n');
    if (metadata_end == NULL) return UMI_STATUS_PARSE_ERROR;
    metadata_length = (size_t)(metadata_end - metadata_start);
    if (metadata_length == 0U || metadata_length >= sizeof(metadata))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(metadata, metadata_start, metadata_length);
    metadata[metadata_length] = '\0';
    count = split_fields(metadata, fields,
                         sizeof(fields) / sizeof(fields[0]));
    if (count != 9U || strcmp(fields[0], "SESSION") != 0)
        return UMI_STATUS_PARSE_ERROR;
    status = parse_uint64_value(fields[3], 10, &clean);
    if (status == UMI_STATUS_OK)
        status = parse_uint64_value(fields[4], 10, &sequence);
    if (status == UMI_STATUS_OK)
        status = parse_uint64_value(fields[7], 10, &encoded_length);
    if (status == UMI_STATUS_OK)
        status = parse_uint64_value(fields[8], 16, &checksum);
    if (status != UMI_STATUS_OK || clean > 1U || encoded_length == 0U ||
        encoded_length >= UMI_DESKTOP_PERSISTENCE_TEXT_CAPACITY)
        return UMI_STATUS_PARSE_ERROR;
    payload = metadata_end + 1;
    if (strlen(payload) != (size_t)encoded_length + strlen(footer) ||
        memcmp(payload + encoded_length, footer, strlen(footer)) != 0 ||
        checksum_text(payload, (size_t)encoded_length) != checksum)
        return UMI_STATUS_PARSE_ERROR;
    (void)memset(&imported, 0, sizeof(imported));
    imported.snapshot.structure_size =
        (uint32_t)sizeof(imported.snapshot);
    status = copy_text(recovery->session_id,
                       sizeof(recovery->session_id), fields[1]);
    if (status == UMI_STATUS_OK)
        status = copy_text(recovery->application_id,
                           sizeof(recovery->application_id), fields[2]);
    if (status == UMI_STATUS_OK)
        status = copy_text(imported.snapshot.checkpoint_id,
                           sizeof(imported.snapshot.checkpoint_id), fields[5]);
    if (status == UMI_STATUS_OK)
        status = copy_text(imported.snapshot.reason,
                           sizeof(imported.snapshot.reason), fields[6]);
    if (status != UMI_STATUS_OK) return status;
    imported.snapshot.sequence = sequence;
    imported.snapshot.encoded_length = (size_t)encoded_length;
    imported.snapshot.checksum = checksum;
    imported.snapshot.revision = 1U;
    (void)memcpy(imported.text, payload, (size_t)encoded_length);
    imported.text[encoded_length] = '\0';
    (void)memset(recovery->checkpoints, 0, sizeof(recovery->checkpoints));
    recovery->checkpoints[0] = imported;
    recovery->checkpoint_count = 1U;
    recovery->last_sequence = sequence;
    recovery->clean_shutdown = clean == 1U;
    recovery->state = recovery->clean_shutdown
        ? UMI_DESKTOP_SESSION_RECOVERY_CLEAN
        : UMI_DESKTOP_SESSION_RECOVERY_AVAILABLE;
    recovery->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_session_recovery_checkpoint_at(
    const UmiDesktopSessionRecovery *recovery,
    size_t index,
    UmiDesktopRecoveryCheckpointSnapshot *out_checkpoint)
{
    if (recovery == NULL || out_checkpoint == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= recovery->checkpoint_count) return UMI_STATUS_NOT_FOUND;
    *out_checkpoint = recovery->checkpoints[index].snapshot;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_session_recovery_snapshot(
    const UmiDesktopSessionRecovery *recovery,
    UmiDesktopSessionRecoverySnapshot *out_snapshot)
{
    if (recovery == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    (void)memcpy(out_snapshot->session_id, recovery->session_id,
                 sizeof(out_snapshot->session_id));
    (void)memcpy(out_snapshot->application_id, recovery->application_id,
                 sizeof(out_snapshot->application_id));
    if (recovery->checkpoint_count > 0U) {
        (void)memcpy(
            out_snapshot->latest_checkpoint_id,
            recovery->checkpoints[recovery->checkpoint_count - 1U]
                .snapshot.checkpoint_id,
            sizeof(out_snapshot->latest_checkpoint_id));
    }
    out_snapshot->checkpoint_count = recovery->checkpoint_count;
    out_snapshot->state = recovery->state;
    out_snapshot->clean_shutdown = recovery->clean_shutdown;
    out_snapshot->recovery_available =
        recovery->state == UMI_DESKTOP_SESSION_RECOVERY_AVAILABLE;
    out_snapshot->last_sequence = recovery->last_sequence;
    out_snapshot->revision = recovery->revision;
    return UMI_STATUS_OK;
}
