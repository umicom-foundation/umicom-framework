/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/chart_persistence.c
 * PURPOSE: Keep storage conflicts and reviewed chart replacement in a reusable owner-thread service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading/chart_persistence.h"
#include <stdlib.h>
#include <string.h>

typedef struct ChartToken { char instrument[128]; uint64_t revision; bool known; } ChartToken;
struct UmiTradingChartPersistence {
    UmiTradingWorkspace *workspace;
    UmiDataServer *server;
    char scope[128];
    ChartToken tokens[UMI_TRADING_MAX_WATCHLIST];
    size_t tokenCount;
    uint64_t previewSequence;
    UmiChartDocument *preview;
    UmiTradingChartPreview evidence;
};
static int Identity(const char *text)
{
    if (text == NULL || text[0] == '\0') return 0;
    for (size_t i = 1U; i < 128U; ++i) if (text[i] == '\0') return 1;
    return 0;
}
static void ClearPreview(UmiTradingChartPersistence *service)
{
    UmiChartDocumentDestroy(service->preview); service->preview = NULL;
    memset(&service->evidence, 0, sizeof(service->evidence));
}
static UmiStatus Token(UmiTradingChartPersistence *service, const char *instrument, ChartToken **out)
{
    if (service == NULL || !Identity(instrument)) return UMI_STATUS_INVALID_ARGUMENT;
    if (service->server == NULL) return UMI_STATUS_UNAVAILABLE;
    for (size_t i = 0U; i < service->tokenCount; ++i)
        if (strcmp(service->tokens[i].instrument, instrument) == 0) { *out = &service->tokens[i]; return UMI_STATUS_OK; }
    if (service->tokenCount == UMI_TRADING_MAX_WATCHLIST) return UMI_STATUS_CAPACITY_EXCEEDED;
    ChartToken *token = &service->tokens[service->tokenCount++];
    strcpy(token->instrument, instrument); *out = token; return UMI_STATUS_OK;
}
UmiStatus UmiTradingChartPersistenceCreate(UmiTradingWorkspace *workspace, UmiTradingChartPersistence **outService)
{
    if (outService != NULL) *outService = NULL;
    if (workspace == NULL || outService == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTradingChartPersistence *service = calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->workspace = workspace; *outService = service; return UMI_STATUS_OK;
}
void UmiTradingChartPersistenceDestroy(UmiTradingChartPersistence *service)
{ if (service != NULL) { ClearPreview(service); free(service); } }
UmiStatus UmiTradingChartPersistenceBind(UmiTradingChartPersistence *service, UmiDataServer *server, const char *scope)
{
    if (service == NULL || (server != NULL && !Identity(scope))) return UMI_STATUS_INVALID_ARGUMENT;
    ClearPreview(service); memset(service->tokens, 0, sizeof(service->tokens)); service->tokenCount = 0U;
    service->server = server; service->scope[0] = '\0';
    if (server != NULL) strcpy(service->scope, scope);
    return UMI_STATUS_OK;
}
UmiStatus UmiTradingChartPersistencePreview(UmiTradingChartPersistence *service,
    const char *instrumentId, UmiTradingChartPreview *outPreview)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ClearPreview(service);
    if (outPreview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (service->previewSequence == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiChartDocument *live = NULL, *saved = NULL;
    UmiTradingChartPreview evidence = {0}; ChartToken *token = NULL;
    /* Capture first so unknown instruments cannot exhaust the bounded tokens. */
    UmiStatus status = UmiTradingWorkspaceCaptureChart(service->workspace, instrumentId, &live);
    if (status == UMI_STATUS_OK) status = Token(service, instrumentId, &token);
    if (status == UMI_STATUS_OK) status = UmiChartDocumentGetSummary(live, &evidence.current);
    if (status == UMI_STATUS_OK) {
        status = UmiChartCheckpointLoad(service->server, service->scope, instrumentId, &saved, &evidence.storage);
        token->known = evidence.storage.storage_revision_known;
        token->revision = evidence.storage.storage_revision;
    }
    if (status == UMI_STATUS_OK) status = UmiTradingChartDocumentValidate(saved);
    if (status == UMI_STATUS_OK) status = UmiChartDocumentGetSummary(saved, &evidence.saved);
    if (status == UMI_STATUS_OK) {
        evidence.preview_id = ++service->previewSequence;
        service->preview = saved; saved = NULL; service->evidence = evidence; *outPreview = evidence;
    } else if (token != NULL && status != UMI_STATUS_NOT_FOUND) token->known = false;
    UmiChartDocumentDestroy(live); UmiChartDocumentDestroy(saved); return status;
}
UmiStatus UmiTradingChartPersistencePreviewDrawing(const UmiTradingChartPersistence *service,
    uint64_t previewId, size_t index, UmiChartDrawingSnapshot *outDrawing)
{
    if (service == NULL || outDrawing == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (service->preview == NULL || previewId != service->evidence.preview_id) return UMI_STATUS_INVALID_STATE;
    return UmiChartDocumentDrawingAt(service->preview, index, outDrawing);
}
UmiStatus UmiTradingChartPersistenceSave(UmiTradingChartPersistence *service,
    const char *instrumentId, uint64_t savedAtMs, UmiChartCheckpointReport *outReport)
{
    UmiChartCheckpointReport report = {0}; UmiChartDocument *live = NULL, *stored = NULL; ChartToken *token = NULL;
    if (outReport != NULL) *outReport = report;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiTradingWorkspaceCaptureChart(service->workspace, instrumentId, &live);
    if (status == UMI_STATUS_OK) status = UmiTradingChartDocumentValidate(live);
    if (status == UMI_STATUS_OK) status = Token(service, instrumentId, &token);
    if (status == UMI_STATUS_OK && !token->known) {
        status = UmiChartCheckpointLoad(service->server, service->scope, instrumentId, &stored, &report);
        if (status == UMI_STATUS_NOT_FOUND && report.storage_revision_known) {
            token->known = true; token->revision = 0U; status = UMI_STATUS_OK;
        } else if (status == UMI_STATUS_OK) status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK) {
        status = UmiChartCheckpointSave(service->server, service->scope, live, token->revision, savedAtMs, &report);
        if (status == UMI_STATUS_OK) {
            token->revision = report.storage_revision; token->known = true; ClearPreview(service);
        }
    }
    UmiChartDocumentDestroy(live); UmiChartDocumentDestroy(stored);
    if (outReport != NULL) *outReport = report;
    return status;
}
static int SameStorage(const UmiChartCheckpointReport *left, const UmiChartCheckpointReport *right)
{
    return left->storage_revision_known == right->storage_revision_known &&
        left->storage_revision == right->storage_revision && left->checkpoint_revision == right->checkpoint_revision &&
        left->recovered_last_good == right->recovered_last_good &&
        left->primary_status == right->primary_status && strcmp(left->digest, right->digest) == 0;
}
UmiStatus UmiTradingChartPersistenceRestore(UmiTradingChartPersistence *service, const char *instrumentId, uint64_t previewId)
{
    if (service == NULL || !Identity(instrumentId)) return UMI_STATUS_INVALID_ARGUMENT;
    if (service->server == NULL) return UMI_STATUS_UNAVAILABLE;
    if (service->preview == NULL || previewId != service->evidence.preview_id || strcmp(service->evidence.saved.pane_id, instrumentId) != 0)
        return UMI_STATUS_INVALID_STATE;
    UmiChartDocument *current = NULL; UmiChartCheckpointReport report;
    UmiStatus status = UmiChartCheckpointLoad(service->server, service->scope, instrumentId, &current, &report);
    if (status == UMI_STATUS_OK && !SameStorage(&service->evidence.storage, &report)) status = UMI_STATUS_INVALID_STATE;
    UmiChartDocumentDestroy(current);
    if (status == UMI_STATUS_OK)
        status = UmiTradingWorkspaceRestoreChart(service->workspace, service->preview,
            service->evidence.current.source_revision, &service->evidence.current.navigation);
    /* A failed confirmation must be reviewed again, not left as a stale action. */
    ClearPreview(service); return status;
}
