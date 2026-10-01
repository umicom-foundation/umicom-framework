/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/checkpoint_store.c
 * PURPOSE: Publish chart checkpoints and last-good recovery copies in one Data Server transaction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "checkpoint_private.h"
#include "umicom/native_launcher/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHART_MAGIC "UMICOM-CHART-CHECKPOINT 1\n"
#define CHART_KEY 192U
typedef struct ChartKeys { char primary[128], backup[128]; } ChartKeys;
typedef struct ChartStored {
    ChartCheckpointMetadata metadata;
    UmiChartDocument *document;
    char digest[65];
} ChartStored;
typedef struct ChartRemoval {
    const char *prefix;
    size_t count;
    char keys[UMI_CHART_DRAWING_CAPACITY + 1U][CHART_KEY];
} ChartRemoval;

static void ReportInit(UmiDataServer *server, UmiChartCheckpointReport *report)
{
    memset(report, 0, sizeof(*report)); report->primary_status = UMI_STATUS_NOT_FOUND;
    const char *path = server != NULL ? umi_data_server_path(server) : NULL;
    report->durable = server != NULL && umi_data_server_backend(server) == UMI_DATA_BACKEND_SQLITE &&
        path != NULL && path[0] != '\0' && strcmp(path, ":memory:") != 0;
}
static UmiStatus Keys(const char *scope, const char *pane, ChartKeys *keys)
{
    if (!UmiChartCheckpointIdentityValid(scope) || !UmiChartCheckpointIdentityValid(pane)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiNativeSha256 hash; unsigned char bytes[32]; char digest[65]; UmiNativeSha256Init(&hash);
    UmiStatus status = UmiNativeSha256Update(&hash, scope, strlen(scope) + 1U);
    if (status == UMI_STATUS_OK) status = UmiNativeSha256Update(&hash, pane, strlen(pane) + 1U);
    if (status == UMI_STATUS_OK) status = UmiNativeSha256Final(&hash, bytes);
    if (status != UMI_STATUS_OK) return status;
    UmiNativeSha256Hex(bytes, digest);
    (void)snprintf(keys->primary, sizeof(keys->primary), "umicom.chart.v1.%s.primary.", digest);
    (void)snprintf(keys->backup, sizeof(keys->backup), "umicom.chart.v1.%s.last-good.", digest);
    return UMI_STATUS_OK;
}
static void RecordKey(char key[CHART_KEY], const char *prefix, size_t index)
{ (void)snprintf(key, CHART_KEY, "%sdrawing.%zu", prefix, index); }
static void ManifestKey(char key[CHART_KEY], const char *prefix)
{ (void)snprintf(key, CHART_KEY, "%smanifest", prefix); }
static UmiStatus HashText(UmiNativeSha256 *hash, const char *text)
{ return UmiNativeSha256Update(hash, text, strlen(text) + 1U); }
static UmiStatus Finish(UmiDataServer *server, UmiStatus status)
{
    if (status == UMI_STATUS_OK) status = umi_data_server_commit(server);
    /* Begin succeeded before Finish is called. Always release its ownership,
     * even if SQLite already aborted the transaction: Data Server retains the
     * lost-transaction owner until rollback acknowledges that failure. */
    if (status != UMI_STATUS_OK) {
        UmiStatus rolled = umi_data_server_rollback(server);
        if (rolled != UMI_STATUS_OK) return rolled;
    }
    return status;
}
static int Recoverable(UmiStatus status)
{ return status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_PARSE_ERROR || status == UMI_STATUS_CAPACITY_EXCEEDED; }

/* Read metadata, every expected drawing and a digest before publishing an
 * owned document. Scope checks also reject a copied manifest under another key. */
static UmiStatus ReadSide(UmiDataServer *server, const char *prefix, const char *scope,
    const char *pane, UmiWorkbenchLayoutDataFieldSet *fields, ChartStored *out)
{
    memset(out, 0, sizeof(*out));
    char key[CHART_KEY], value[UMI_CHART_CHECKPOINT_VALUE];
    ManifestKey(key, prefix);
    UmiStatus status = umi_data_server_get(server, key, value, sizeof(value));
    if (status != UMI_STATUS_OK) return status;
    const size_t magic = sizeof(CHART_MAGIC) - 1U;
    if (strlen(value) <= magic + 65U || memcmp(value, CHART_MAGIC, magic) != 0 || value[magic + 64U] != '\n')
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < 64U; ++i)
        if (strchr("0123456789abcdef", value[magic + i]) == NULL) return UMI_STATUS_PARSE_ERROR;
    memcpy(out->digest, value + magic, 64U); out->digest[64] = '\0';
    const char *metadata = value + magic + 65U;
    status = UmiChartCheckpointDecodeMetadata(fields, metadata, &out->metadata);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(out->metadata.scope, scope) != 0 || strcmp(out->metadata.summary.pane_id, pane) != 0)
        return UMI_STATUS_PARSE_ERROR;
    UmiNativeSha256 hash; UmiNativeSha256Init(&hash); status = HashText(&hash, metadata);
    UmiChartDocument *document = calloc(1U, sizeof(*document));
    if (document == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    document->summary = out->metadata.summary;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < document->summary.drawing_count; ++i) {
        RecordKey(key, prefix, i); status = umi_data_server_get(server, key, value, sizeof(value));
        if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK) status = HashText(&hash, value);
        if (status == UMI_STATUS_OK) status = UmiChartCheckpointDecodeDrawing(fields, value, &document->drawings[i]);
    }
    unsigned char bytes[32]; char digest[65];
    if (status == UMI_STATUS_OK) status = UmiNativeSha256Final(&hash, bytes);
    if (status == UMI_STATUS_OK) {
        UmiNativeSha256Hex(bytes, digest);
        if (strcmp(digest, out->digest) != 0) status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK && UmiChartDocumentValidateOwned(document) != UMI_STATUS_OK) status = UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK) { UmiChartDocumentDestroy(document); return status; }
    out->document = document; return UMI_STATUS_OK;
}
/* Collect under the Data Server read lock, then delete after visiting. Never
 * recursively call into the server from its visitor callback. Only this exact
 * scope/pane/side prefix is owned; other charts and layout namespaces survive. */
static UmiStatus CollectOwned(const char *key, const char *value, void *data)
{
    ChartRemoval *remove = data; (void)value;
    if (strncmp(key, remove->prefix, strlen(remove->prefix)) != 0) return UMI_STATUS_OK;
    if (remove->count >= UMI_CHART_DRAWING_CAPACITY + 1U || strlen(key) >= CHART_KEY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(remove->keys[remove->count++], key); return UMI_STATUS_OK;
}
static UmiStatus ClearSide(UmiDataServer *server, const char *prefix)
{
    ChartRemoval *remove = calloc(1U, sizeof(*remove));
    if (remove == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    remove->prefix = prefix;
    UmiStatus status = umi_data_server_visit(server, CollectOwned, remove);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < remove->count; ++i)
        status = umi_data_server_delete(server, remove->keys[i]);
    free(remove); return status;
}
static UmiStatus WriteSide(UmiDataServer *server, const char *prefix,
    const ChartCheckpointMetadata *metadata, const UmiChartDocument *document,
    UmiWorkbenchLayoutDataFieldSet *fields, char outDigest[65])
{
    char key[CHART_KEY], value[UMI_CHART_CHECKPOINT_VALUE], meta[UMI_CHART_CHECKPOINT_VALUE];
    UmiStatus status = UmiChartCheckpointEncodeMetadata(fields, metadata, meta);
    if (status != UMI_STATUS_OK) return status;
    UmiNativeSha256 hash; UmiNativeSha256Init(&hash);
    status = HashText(&hash, meta);
    if (status == UMI_STATUS_OK) status = ClearSide(server, prefix);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < document->summary.drawing_count; ++i) {
        status = UmiChartCheckpointEncodeDrawing(fields, &document->drawings[i], value);
        if (status == UMI_STATUS_OK) status = HashText(&hash, value);
        RecordKey(key, prefix, i);
        if (status == UMI_STATUS_OK) status = umi_data_server_set(server, key, value);
    }
    unsigned char bytes[32];
    if (status == UMI_STATUS_OK) status = UmiNativeSha256Final(&hash, bytes);
    if (status == UMI_STATUS_OK) {
        UmiNativeSha256Hex(bytes, outDigest);
        int written = snprintf(value, sizeof(value), CHART_MAGIC "%s\n%s", outDigest, meta);
        if (written < 0 || (size_t)written >= sizeof(value)) status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ManifestKey(key, prefix);
    if (status == UMI_STATUS_OK) status = umi_data_server_set(server, key, value);
    return status;
}
static void Evidence(const ChartStored *stored, UmiChartCheckpointReport *report)
{
    report->checkpoint_revision = stored->metadata.storageRevision;
    report->source_revision = stored->metadata.summary.source_revision;
    report->saved_at_ms = stored->metadata.savedAtMs;
    report->drawing_count = stored->metadata.summary.drawing_count;
    memcpy(report->digest, stored->digest, sizeof(report->digest));
}

UmiStatus UmiChartCheckpointSave(UmiDataServer *server, const char *scope,
    const UmiChartDocument *document, uint64_t expectedRevision, uint64_t savedAtMs,
    UmiChartCheckpointReport *outReport)
{
    UmiChartCheckpointReport report; ReportInit(server, &report);
    ChartKeys keys; ChartStored previous = {0}, backup = {0};
    UmiWorkbenchLayoutDataFieldSet *fields = NULL;
    UmiStatus status = UmiChartDocumentValidateOwned(document);
    if (status == UMI_STATUS_OK) status = Keys(scope, document->summary.pane_id, &keys);
    if (status == UMI_STATUS_OK && server == NULL) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK && umi_data_server_in_transaction(server)) status = UMI_STATUS_BUSY;
    if (status != UMI_STATUS_OK) goto done;
    fields = calloc(1U, sizeof(*fields));
    if (fields == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) goto done;
    status = ReadSide(server, keys.primary, scope, document->summary.pane_id, fields, &previous);
    report.primary_status = status;
    if (status == UMI_STATUS_OK) {
        report.storage_revision = previous.metadata.storageRevision;
        report.storage_revision_known = true; Evidence(&previous, &report);
    } else if (status == UMI_STATUS_NOT_FOUND) {
        /* A missing primary with surviving backup is not a fresh namespace:
         * restarting its CAS counter could accept an old writer's token. */
        UmiStatus prior = ReadSide(server, keys.backup, scope, document->summary.pane_id, fields, &backup);
        if (prior == UMI_STATUS_NOT_FOUND) { status = UMI_STATUS_OK; report.storage_revision_known = true; }
        else status = prior == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : prior;
    }
    if (status == UMI_STATUS_OK && report.storage_revision != expectedRevision) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && report.storage_revision == UINT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
    char backupDigest[65], digest[65] = {0};
    if (status == UMI_STATUS_OK && previous.document != NULL)
        status = WriteSide(server, keys.backup, &previous.metadata, previous.document, fields, backupDigest);
    ChartCheckpointMetadata metadata = {0};
    if (status == UMI_STATUS_OK) {
        strcpy(metadata.scope, scope); metadata.summary = document->summary;
        metadata.storageRevision = report.storage_revision + 1U; metadata.savedAtMs = savedAtMs;
        status = WriteSide(server, keys.primary, &metadata, document, fields, digest);
    }
    status = Finish(server, status);
    if (status == UMI_STATUS_OK) {
        report.storage_revision = metadata.storageRevision; report.storage_revision_known = true;
        report.checkpoint_revision = metadata.storageRevision;
        report.source_revision = document->summary.source_revision; report.saved_at_ms = savedAtMs;
        report.drawing_count = document->summary.drawing_count; report.primary_status = UMI_STATUS_OK;
        memcpy(report.digest, digest, sizeof(report.digest));
    }
done:
    free(fields); UmiChartDocumentDestroy(previous.document); UmiChartDocumentDestroy(backup.document);
    if (outReport != NULL) *outReport = report;
    return status;
}

UmiStatus UmiChartCheckpointLoad(UmiDataServer *server, const char *scope,
    const char *paneId, UmiChartDocument **outDocument, UmiChartCheckpointReport *outReport)
{
    UmiChartCheckpointReport report; ReportInit(server, &report);
    ChartKeys keys; ChartStored primary = {0}, backup = {0}; ChartStored *chosen = NULL;
    UmiWorkbenchLayoutDataFieldSet *fields = NULL;
    UmiStatus status = Keys(scope, paneId, &keys);
    if (outDocument != NULL) *outDocument = NULL;
    if (status == UMI_STATUS_OK && (server == NULL || outDocument == NULL)) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK && umi_data_server_in_transaction(server)) status = UMI_STATUS_BUSY;
    if (status != UMI_STATUS_OK) goto done;
    fields = calloc(1U, sizeof(*fields));
    if (fields == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) goto done;
    status = ReadSide(server, keys.primary, scope, paneId, fields, &primary);
    report.primary_status = status;
    if (status == UMI_STATUS_OK) {
        report.storage_revision = primary.metadata.storageRevision; report.storage_revision_known = true; chosen = &primary;
    } else if (Recoverable(status)) {
        UmiStatus recovered = ReadSide(server, keys.backup, scope, paneId, fields, &backup);
        if (recovered == UMI_STATUS_OK) {
            status = UMI_STATUS_OK; report.recovered_last_good = true; chosen = &backup;
        } else if (recovered == UMI_STATUS_NOT_FOUND && report.primary_status == UMI_STATUS_NOT_FOUND) {
            report.storage_revision_known = true; status = UMI_STATUS_NOT_FOUND;
        } else if (!Recoverable(recovered) || report.primary_status == UMI_STATUS_NOT_FOUND) status = recovered;
    }
    status = Finish(server, status);
    if (status == UMI_STATUS_OK && chosen != NULL) {
        Evidence(chosen, &report); *outDocument = chosen->document; chosen->document = NULL;
    }
done:
    free(fields); UmiChartDocumentDestroy(primary.document); UmiChartDocumentDestroy(backup.document);
    if (outReport != NULL) *outReport = report;
    return status;
}
