/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/workspace_internal.h
 *
 * PURPOSE:
 *   Keep mutable AI workspace state private to the Framework service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKSPACE_INTERNAL_H
#define UMICOM_AI_WORKSPACE_INTERNAL_H
#include "umicom/ai_workspace/workspace.h"

#define AW_MAX_ALLOWED_TOOLS 8U
#define AW_WIRE_CAPACITY 4096U

typedef struct AwState {
    uint64_t revision;
    uint64_t corpusRevision;
    size_t collectionCount;
    size_t sourceCount;
    size_t jobCount;
    UmiAiWorkspaceCollection collections[UMI_AI_WORKSPACE_MAX_COLLECTIONS];
    UmiAiWorkspaceSource sources[UMI_AI_WORKSPACE_MAX_SOURCES];
    UmiAiWorkspaceJob jobs[UMI_AI_WORKSPACE_MAX_JOBS];
} AwState;

typedef struct AwEmbedding {
    char modelId[UMI_AI_ID_CAPACITY];
    uint64_t sourceRevision;
    UmiAiEmbedding value;
} AwEmbedding;

struct UmiAiWorkspace {
    UmiDataServer *data;
    UmiAiRuntime *runtime;
    char prefix[96];
    AwState *state;
    AwEmbedding embeddings[UMI_AI_WORKSPACE_MAX_SOURCES];
    char allowedTools[AW_MAX_ALLOWED_TOOLS][UMI_AI_ID_CAPACITY];
    size_t allowedToolCount;
    bool busy;
    bool recoveryRequired;
};

/* The wire format is length-bounded text, never a dump of native structures. */
typedef struct AwWire {
    char text[AW_WIRE_CAPACITY];
    size_t offset;
    UmiStatus status;
} AwWire;
void AwWireInit(AwWire *wire);
void AwWireText(AwWire *wire, const char *text);
void AwWireNumber(AwWire *wire, uint64_t value);
UmiStatus AwWireReadText(AwWire *wire, char *out, size_t capacity);
UmiStatus AwWireReadNumber(AwWire *wire, uint64_t *out);
bool AwWireEnd(const AwWire *wire);
UmiStatus AwLoad(UmiAiWorkspace *workspace, AwState *outState);
UmiStatus AwSave(UmiAiWorkspace *workspace, AwState *next);
UmiStatus AwStateValidate(const AwState *state);
UmiStatus AwReady(const UmiAiWorkspace *workspace);
UmiStatus AwTextCopy(char *out, size_t capacity, const char *text, bool allowEmpty);
bool AwTextValid(const char *text, size_t capacity, bool allowEmpty);
bool AwIdValid(const char *id, size_t capacity);
bool AwEmbeddingValid(const UmiAiEmbedding *embedding);
size_t AwCollectionIndex(const AwState *state, const char *id);
size_t AwSourceIndex(const AwState *state, const char *id);
size_t AwJobIndex(const AwState *state, const char *id);
UmiStatus AwPublish(UmiAiWorkspace *workspace, AwState *next, bool corpusChanged);
UmiStatus AwResponseValidate(const UmiAiWorkspaceJob *job, const UmiAiResponse *response);
UmiStatus AwCitationsValidate(const char *text, size_t evidenceCount);
#endif
