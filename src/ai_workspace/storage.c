/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/storage.c
 *
 * PURPOSE:
 *   Persist AI collections and reviewed jobs transactionally through the Data Server only.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workspace_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AW_TRY(expression) do { UmiStatus awStatus = (expression); if (awStatus != UMI_STATUS_OK) return awStatus; } while (0)

static UmiStatus AwKey(UmiAiWorkspace *workspace, const char *suffix, char *key, size_t capacity)
{
    size_t left = strlen(workspace->prefix), right = strlen(suffix);
    if (left >= capacity || right >= capacity - left) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(key, workspace->prefix, left); memcpy(key + left, suffix, right + 1U);
    return UMI_STATUS_OK;
}
static UmiStatus AwRead(UmiAiWorkspace *workspace, const char *suffix, AwWire *wire)
{
    char key[192];
    AwWireInit(wire); AW_TRY(AwKey(workspace, suffix, key, sizeof(key)));
    UmiStatus status = umi_data_server_get(workspace->data, key, wire->text, sizeof(wire->text));
    return status == UMI_STATUS_CAPACITY_EXCEEDED ? UMI_STATUS_PARSE_ERROR : status;
}
static UmiStatus AwWrite(UmiAiWorkspace *workspace, const char *suffix, AwWire *wire)
{
    char key[192];
    if (wire->status != UMI_STATUS_OK) return wire->status;
    AW_TRY(AwKey(workspace, suffix, key, sizeof(key)));
    return umi_data_server_set(workspace->data, key, wire->text);
}
static UmiStatus AwReadRequired(UmiAiWorkspace *workspace, const char *suffix, AwWire *wire)
{
    UmiStatus status = AwRead(workspace, suffix, wire);
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
}
static UmiStatus AwField(UmiAiWorkspace *workspace, const char *suffix, char *text, size_t capacity)
{
    AwWire wire; AW_TRY(AwReadRequired(workspace, suffix, &wire));
    AW_TRY(AwWireReadText(&wire, text, capacity));
    return AwWireEnd(&wire) ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
static UmiStatus AwWriteField(UmiAiWorkspace *workspace, const char *suffix, const char *text)
{
    AwWire wire; AwWireInit(&wire); AwWireText(&wire, text);
    return AwWrite(workspace, suffix, &wire);
}

/* Collect keys before deletion: invoking another Data Server method from its
 * visitor would re-enter the server lock. The bounded copy also preserves all
 * records outside this workspace's namespace. */
typedef struct AwKeys { const char *prefix; char keys[1024][192]; size_t count; } AwKeys;
static UmiStatus AwCollectKey(const char *key, const char *value, void *context)
{
    AwKeys *keys = context; (void)value;
    if (strncmp(key, keys->prefix, strlen(keys->prefix)) != 0) return UMI_STATUS_OK;
    if (keys->count >= 1024U || strlen(key) >= sizeof(keys->keys[0])) return UMI_STATUS_PARSE_ERROR;
    memcpy(keys->keys[keys->count++], key, strlen(key) + 1U);
    return UMI_STATUS_OK;
}
static size_t AwRecordCount(const AwState *state)
{
    size_t count = 1U + state->collectionCount + state->sourceCount * 2U;
    for (size_t j = 0U; j < state->jobCount; ++j) count += 5U + state->jobs[j].evidenceCount * 2U;
    return count;
}
static UmiStatus AwCountNamespace(UmiAiWorkspace *workspace, size_t expected)
{
    AwKeys *keys = calloc(1U, sizeof(*keys)); UmiStatus status;
    if (keys == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    keys->prefix = workspace->prefix;
    status = umi_data_server_visit(workspace->data, AwCollectKey, keys);
    if (status == UMI_STATUS_OK && keys->count != expected) status = UMI_STATUS_PARSE_ERROR;
    free(keys); return status;
}
static void AwSourceMetaWrite(AwWire *wire, const UmiAiWorkspaceSource *source)
{
    AwWireInit(wire); AwWireText(wire, source->id); AwWireText(wire, source->collectionId);
    AwWireText(wire, source->title); AwWireNumber(wire, source->firstLine);
    AwWireNumber(wire, source->lastLine); AwWireNumber(wire, source->revision);
}
static UmiStatus AwSourceMetaRead(AwWire *wire, UmiAiWorkspaceSource *source)
{
    uint64_t number;
    AW_TRY(AwWireReadText(wire, source->id, sizeof(source->id)));
    AW_TRY(AwWireReadText(wire, source->collectionId, sizeof(source->collectionId)));
    AW_TRY(AwWireReadText(wire, source->title, sizeof(source->title)));
    AW_TRY(AwWireReadNumber(wire, &number)); if (number > UINT32_MAX) return UMI_STATUS_PARSE_ERROR;
    source->firstLine = (uint32_t)number;
    AW_TRY(AwWireReadNumber(wire, &number)); if (number > UINT32_MAX) return UMI_STATUS_PARSE_ERROR;
    source->lastLine = (uint32_t)number;
    AW_TRY(AwWireReadNumber(wire, &source->revision));
    return AwWireEnd(wire) ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
static UmiStatus AwSuffix(char *out, size_t capacity, const char *base, const char *tail)
{
    size_t left = strlen(base), right = strlen(tail);
    if (left >= capacity || right >= capacity - left) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, base, left); memcpy(out + left, tail, right + 1U); return UMI_STATUS_OK;
}
static UmiStatus AwSourceWrite(UmiAiWorkspace *workspace, const char *base, const UmiAiWorkspaceSource *source)
{
    char suffix[96]; AwWire wire;
    AW_TRY(AwSuffix(suffix, sizeof(suffix), base, "/m"));
    AwSourceMetaWrite(&wire, source); AW_TRY(AwWrite(workspace, suffix, &wire));
    AW_TRY(AwSuffix(suffix, sizeof(suffix), base, "/t"));
    return AwWriteField(workspace, suffix, source->text);
}
static UmiStatus AwSourceRead(UmiAiWorkspace *workspace, const char *base, UmiAiWorkspaceSource *source)
{
    char suffix[96]; AwWire wire;
    AW_TRY(AwSuffix(suffix, sizeof(suffix), base, "/m"));
    AW_TRY(AwReadRequired(workspace, suffix, &wire)); AW_TRY(AwSourceMetaRead(&wire, source));
    AW_TRY(AwSuffix(suffix, sizeof(suffix), base, "/t"));
    return AwField(workspace, suffix, source->text, sizeof(source->text));
}
static void AwJobMetaWrite(AwWire *wire, const UmiAiWorkspaceJob *job)
{
    AwWireInit(wire);
    AwWireText(wire, job->id); AwWireText(wire, job->providerId); AwWireText(wire, job->modelId);
    AwWireText(wire, job->collectionId); AwWireText(wire, job->requestedBy); AwWireText(wire, job->reviewedBy);
    AwWireText(wire, job->permission); AwWireNumber(wire, (uint64_t)job->kind);
    AwWireNumber(wire, (uint64_t)job->state); AwWireNumber(wire, (uint64_t)job->providerKind);
    AwWireNumber(wire, job->maxOutputTokens); AwWireNumber(wire, job->corpusRevision);
    AwWireNumber(wire, job->evidenceCount); AwWireNumber(wire, (uint64_t)job->status);
    AwWireText(wire, job->response.request_id); AwWireText(wire, job->response.provider_id);
    AwWireText(wire, job->response.model_id); AwWireNumber(wire, (uint64_t)job->response.finish_reason);
    AwWireNumber(wire, job->response.usage.input_tokens); AwWireNumber(wire, job->response.usage.output_tokens);
    AwWireNumber(wire, job->response.usage.total_tokens);
}
static UmiStatus AwJobMetaRead(AwWire *wire, UmiAiWorkspaceJob *job)
{
    uint64_t number;
    AW_TRY(AwWireReadText(wire, job->id, sizeof(job->id)));
    AW_TRY(AwWireReadText(wire, job->providerId, sizeof(job->providerId)));
    AW_TRY(AwWireReadText(wire, job->modelId, sizeof(job->modelId)));
    AW_TRY(AwWireReadText(wire, job->collectionId, sizeof(job->collectionId)));
    AW_TRY(AwWireReadText(wire, job->requestedBy, sizeof(job->requestedBy)));
    AW_TRY(AwWireReadText(wire, job->reviewedBy, sizeof(job->reviewedBy)));
    AW_TRY(AwWireReadText(wire, job->permission, sizeof(job->permission)));
    AW_TRY(AwWireReadNumber(wire, &number)); if (number < 1U || number > 3U) return UMI_STATUS_PARSE_ERROR;
    job->kind = (UmiAiWorkspaceJobKind)number;
    AW_TRY(AwWireReadNumber(wire, &number)); if (number < 1U || number > 8U) return UMI_STATUS_PARSE_ERROR;
    job->state = (UmiAiWorkspaceJobState)number;
    AW_TRY(AwWireReadNumber(wire, &number)); if (number > 4U) return UMI_STATUS_PARSE_ERROR;
    job->providerKind = (UmiAiProviderKind)number;
    AW_TRY(AwWireReadNumber(wire, &number)); if (number > 4096U) return UMI_STATUS_PARSE_ERROR;
    job->maxOutputTokens = (uint32_t)number;
    AW_TRY(AwWireReadNumber(wire, &job->corpusRevision));
    AW_TRY(AwWireReadNumber(wire, &number)); if (number > UMI_AI_WORKSPACE_MAX_EVIDENCE) return UMI_STATUS_PARSE_ERROR;
    job->evidenceCount = (size_t)number;
    AW_TRY(AwWireReadNumber(wire, &number)); if (number > (uint64_t)UMI_STATUS_BUSY) return UMI_STATUS_PARSE_ERROR;
    job->status = (UmiStatus)number;
    AW_TRY(AwWireReadText(wire, job->response.request_id, sizeof(job->response.request_id)));
    AW_TRY(AwWireReadText(wire, job->response.provider_id, sizeof(job->response.provider_id)));
    AW_TRY(AwWireReadText(wire, job->response.model_id, sizeof(job->response.model_id)));
    AW_TRY(AwWireReadNumber(wire, &number)); if (number > 4U) return UMI_STATUS_PARSE_ERROR;
    job->response.finish_reason = (UmiAiFinishReason)number;
    AW_TRY(AwWireReadNumber(wire, &job->response.usage.input_tokens));
    AW_TRY(AwWireReadNumber(wire, &job->response.usage.output_tokens));
    AW_TRY(AwWireReadNumber(wire, &job->response.usage.total_tokens));
    return AwWireEnd(wire) ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
static UmiStatus AwStateWrite(UmiAiWorkspace *workspace, const AwState *state)
{
    AwWire wire; char suffix[96];
    AwWireInit(&wire); AwWireNumber(&wire, 1U); AwWireNumber(&wire, state->revision);
    AwWireNumber(&wire, state->corpusRevision); AwWireNumber(&wire, state->collectionCount);
    AwWireNumber(&wire, state->sourceCount); AwWireNumber(&wire, state->jobCount);
    AW_TRY(AwWrite(workspace, "meta", &wire));
    for (size_t i = 0U; i < state->collectionCount; ++i) {
        AwWireInit(&wire); AwWireText(&wire, state->collections[i].id); AwWireText(&wire, state->collections[i].title);
        (void)snprintf(suffix, sizeof(suffix), "c/%zu", i); AW_TRY(AwWrite(workspace, suffix, &wire));
    }
    for (size_t i = 0U; i < state->sourceCount; ++i) {
        (void)snprintf(suffix, sizeof(suffix), "s/%zu", i); AW_TRY(AwSourceWrite(workspace, suffix, &state->sources[i]));
    }
    for (size_t j = 0U; j < state->jobCount; ++j) {
        const UmiAiWorkspaceJob *job = &state->jobs[j];
        AwJobMetaWrite(&wire, job); (void)snprintf(suffix, sizeof(suffix), "j/%zu/m", j);
        AW_TRY(AwWrite(workspace, suffix, &wire));
        (void)snprintf(suffix, sizeof(suffix), "j/%zu/p", j); AW_TRY(AwWriteField(workspace, suffix, job->prompt));
        for (size_t e = 0U; e < job->evidenceCount; ++e) {
            (void)snprintf(suffix, sizeof(suffix), "j/%zu/e/%zu", j, e);
            AW_TRY(AwSourceWrite(workspace, suffix, &job->evidence[e].source));
        }
        /* Split only at UTF-8 boundaries. A storage record remains below the
         * Data Server's existing 4096-byte memory-backend value limit. */
        size_t offset = 0U, length = strlen(job->response.text);
        for (size_t part = 0U; part < 3U; ++part) {
            char text[1537]; size_t amount = length - offset;
            if (amount > 1536U) amount = 1536U;
            if (offset + amount < length)
                while (amount > 0U && ((unsigned char)job->response.text[offset + amount] & 0xC0U) == 0x80U) --amount;
            memcpy(text, job->response.text + offset, amount); text[amount] = '\0'; offset += amount;
            (void)snprintf(suffix, sizeof(suffix), "j/%zu/o/%zu", j, part);
            AW_TRY(AwWriteField(workspace, suffix, text));
        }
        if (offset != length) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
static UmiStatus AwStateRead(UmiAiWorkspace *workspace, AwState *state)
{
    AwWire wire; char suffix[96]; uint64_t number;
    UmiStatus status = AwRead(workspace, "meta", &wire);
    memset(state, 0, sizeof(*state));
    if (status == UMI_STATUS_NOT_FOUND) return AwCountNamespace(workspace, 0U);
    if (status != UMI_STATUS_OK) return status;
    AW_TRY(AwWireReadNumber(&wire, &number)); if (number != 1U) return UMI_STATUS_PARSE_ERROR;
    AW_TRY(AwWireReadNumber(&wire, &state->revision)); if (state->revision == 0U) return UMI_STATUS_PARSE_ERROR;
    AW_TRY(AwWireReadNumber(&wire, &state->corpusRevision));
    AW_TRY(AwWireReadNumber(&wire, &number)); if (number > UMI_AI_WORKSPACE_MAX_COLLECTIONS) return UMI_STATUS_PARSE_ERROR;
    state->collectionCount = (size_t)number;
    AW_TRY(AwWireReadNumber(&wire, &number)); if (number > UMI_AI_WORKSPACE_MAX_SOURCES) return UMI_STATUS_PARSE_ERROR;
    state->sourceCount = (size_t)number;
    AW_TRY(AwWireReadNumber(&wire, &number)); if (number > UMI_AI_WORKSPACE_MAX_JOBS) return UMI_STATUS_PARSE_ERROR;
    state->jobCount = (size_t)number;
    if (!AwWireEnd(&wire)) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < state->collectionCount; ++i) {
        (void)snprintf(suffix, sizeof(suffix), "c/%zu", i); AW_TRY(AwReadRequired(workspace, suffix, &wire));
        AW_TRY(AwWireReadText(&wire, state->collections[i].id, sizeof(state->collections[i].id)));
        AW_TRY(AwWireReadText(&wire, state->collections[i].title, sizeof(state->collections[i].title)));
        if (!AwWireEnd(&wire)) return UMI_STATUS_PARSE_ERROR;
    }
    for (size_t i = 0U; i < state->sourceCount; ++i) {
        (void)snprintf(suffix, sizeof(suffix), "s/%zu", i); AW_TRY(AwSourceRead(workspace, suffix, &state->sources[i]));
    }
    for (size_t j = 0U; j < state->jobCount; ++j) {
        UmiAiWorkspaceJob *job = &state->jobs[j];
        (void)snprintf(suffix, sizeof(suffix), "j/%zu/m", j); AW_TRY(AwReadRequired(workspace, suffix, &wire));
        AW_TRY(AwJobMetaRead(&wire, job));
        (void)snprintf(suffix, sizeof(suffix), "j/%zu/p", j); AW_TRY(AwField(workspace, suffix, job->prompt, sizeof(job->prompt)));
        for (size_t e = 0U; e < job->evidenceCount; ++e) {
            (void)snprintf(suffix, sizeof(suffix), "j/%zu/e/%zu", j, e);
            AW_TRY(AwSourceRead(workspace, suffix, &job->evidence[e].source));
        }
        size_t offset = 0U;
        for (size_t part = 0U; part < 3U; ++part) {
            char text[1537]; size_t length;
            (void)snprintf(suffix, sizeof(suffix), "j/%zu/o/%zu", j, part);
            AW_TRY(AwField(workspace, suffix, text, sizeof(text))); length = strlen(text);
            if (length >= sizeof(job->response.text) - offset) return UMI_STATUS_PARSE_ERROR;
            memcpy(job->response.text + offset, text, length + 1U); offset += length;
        }
    }
    AW_TRY(AwCountNamespace(workspace, AwRecordCount(state)));
    return AwStateValidate(state);
}
UmiStatus AwLoad(UmiAiWorkspace *workspace, AwState *outState)
{
    UmiStatus status, rollback;
    if (umi_data_server_in_transaction(workspace->data)) return UMI_STATUS_BUSY;
    status = umi_data_server_begin(workspace->data); if (status != UMI_STATUS_OK) return status;
    status = AwStateRead(workspace, outState);
    rollback = umi_data_server_rollback(workspace->data);
    if (rollback != UMI_STATUS_OK) { workspace->recoveryRequired = true; return rollback; }
    if (status != UMI_STATUS_OK) return status;
    /* Saved approvals never survive a new runtime binding. Interrupted work
     * is explicitly uncertain; reading the journal never repeats a callback. */
    for (size_t j = 0U; j < outState->jobCount; ++j) {
        UmiAiWorkspaceJob *job = &outState->jobs[j];
        if (job->state == UMI_AI_WORKSPACE_RUNNING) {
            job->state = UMI_AI_WORKSPACE_INTERRUPTED; job->status = UMI_STATUS_UNAVAILABLE;
        } else if (job->state == UMI_AI_WORKSPACE_APPROVED) {
            job->state = UMI_AI_WORKSPACE_REVIEW; job->reviewedBy[0] = '\0';
        }
    }
    return UMI_STATUS_OK;
}
UmiStatus AwSave(UmiAiWorkspace *workspace, AwState *next)
{
    AwWire wire; uint64_t schema = 0U, revision = 0U; UmiStatus status, rollback;
    AwKeys *keys;
    if (umi_data_server_in_transaction(workspace->data)) return UMI_STATUS_BUSY;
    if (workspace->state->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    next->revision = workspace->state->revision + 1U;
    status = AwStateValidate(next); if (status != UMI_STATUS_OK) return status;
    keys = calloc(1U, sizeof(*keys)); if (keys == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    keys->prefix = workspace->prefix;
    status = umi_data_server_begin(workspace->data);
    if (status != UMI_STATUS_OK) { free(keys); return status; }
    status = AwRead(workspace, "meta", &wire);
    if (status == UMI_STATUS_NOT_FOUND && workspace->state->revision == 0U) status = UMI_STATUS_OK;
    else if (status == UMI_STATUS_OK) {
        status = AwWireReadNumber(&wire, &schema);
        if (status == UMI_STATUS_OK) status = AwWireReadNumber(&wire, &revision);
        if (status == UMI_STATUS_OK && schema != 1U) status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK && revision != workspace->state->revision) status = UMI_STATUS_BUSY;
    } else if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK) {
        AwState *stored = calloc(1U, sizeof(*stored));
        if (stored == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        else { status = AwStateRead(workspace, stored); free(stored); }
    }
    if (status == UMI_STATUS_OK) status = umi_data_server_visit(workspace->data, AwCollectKey, keys);
    if (status == UMI_STATUS_OK && workspace->state->revision == 0U && keys->count != 0U) status = UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < keys->count; ++i)
        status = umi_data_server_delete(workspace->data, keys->keys[i]);
    if (status == UMI_STATUS_OK) status = AwStateWrite(workspace, next);
    free(keys);
    if (status == UMI_STATUS_OK) {
        status = umi_data_server_commit(workspace->data);
        if (status == UMI_STATUS_OK) return status;
    }
    rollback = umi_data_server_rollback(workspace->data);
    if (rollback != UMI_STATUS_OK) workspace->recoveryRequired = true;
    return status;
}
