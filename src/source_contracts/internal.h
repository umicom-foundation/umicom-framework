/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/internal.h
 *
 * PURPOSE:
 *   Share private implementation helpers for one native qualification engine.
 *
 * Author: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SOURCE_CONTRACTS_INTERNAL_H
#define UMICOM_SOURCE_CONTRACTS_INTERNAL_H
#include "umicom/source_contracts/source_contracts.h"
#include <stdio.h>

#define SC_MAX_FILE_BYTES (16U * 1024U * 1024U)
#define SC_MAX_FILES 100000U
#define SC_MAX_DEPTH 128U
#define SC_MAX_TOKEN_BYTES 4096U

typedef struct ScToken { size_t begin, end; unsigned char kind; } ScToken;
typedef struct ScText {
    char *data;
    size_t length;
    size_t line_offset;
    ScToken *tokens;
    size_t count;
} ScText;
typedef struct ScFiles { char **items; size_t count, capacity; } ScFiles;

char *ScCopy(const char *text, size_t length);
void ScFinding(UmiSourceContractReport *report, const char *path, size_t line,
    const char *rule, const char *message, bool input_error);
int ScLex(ScText *text, const char *data, bool cmake);
void ScTextFree(ScText *text);
bool ScTokenEquals(const ScText *text, size_t index, const char *value);
bool ScIdentifier(const ScText *text, size_t index);
size_t ScMatching(const ScText *text, size_t opening);
int ScBody(const ScText *source, const char *name, ScText *body);
bool ScContains(const ScText *source, const char *pattern);
size_t ScIndex(const ScText *source, const char *pattern, size_t from);
size_t ScCount(const ScText *source, const char *pattern);
char *ScCompactRange(const ScText *source, size_t first, size_t last);
size_t ScLine(const ScText *source, size_t token);
char *ScRead(const char *path);
char *ScJoin(const char *root, const char *path);
int ScCollect(const char *root, bool headers, ScFiles *files);
int ScCollectManifests(const char *root, ScFiles *files);
void ScFilesFree(ScFiles *files);
int ScMetadata(const char *source, size_t length, const char *path,
    UmiSourceContractReport *report);
int ScGuard(const char *source, const char *path, char **guard,
    UmiSourceContractReport *report);
int ScPortfolio(const char *root, UmiSourceContractReport *report);
int ScWorkbenchRules(const char *root, UmiSourceContractReport *report);
#endif
