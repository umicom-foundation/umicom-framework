/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_contracts/internal.h
 *
 * PURPOSE:
 *   * Share private implementation helpers for one native qualification engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SOURCE_CONTRACTS_INTERNAL_H
#define UMICOM_SOURCE_CONTRACTS_INTERNAL_H
#include "umicom/source_contracts/source_contracts.h"
#include <stdio.h>

/* These helpers remain private to Source Contracts, but their definitions
 * have external linkage across its translation units. Setup Centre uses some
 * of the same short names with different contracts. Preserve the established
 * implementation spellings through local aliases while emitting component-
 * qualified linker symbols. New cross-file helpers must join this namespace;
 * public UmiSourceContract APIs and independently owned Setup Centre helpers
 * are unchanged. Never expose this private header as an SDK contract. */
#define ScCopy UmiSourceContractInternalCopy
#define ScFinding UmiSourceContractInternalFinding
#define ScLex UmiSourceContractInternalLex
#define ScLiterals UmiSourceContractInternalLiterals
#define ScTextFree UmiSourceContractInternalTextFree
#define ScTokenEquals UmiSourceContractInternalTokenEquals
#define ScIdentifier UmiSourceContractInternalIdentifier
#define ScMatching UmiSourceContractInternalMatching
#define ScBody UmiSourceContractInternalBody
#define ScContains UmiSourceContractInternalContains
#define ScIndex UmiSourceContractInternalIndex
#define ScCount UmiSourceContractInternalCount
#define ScCompactRange UmiSourceContractInternalCompactRange
#define ScLine UmiSourceContractInternalLine
#define ScRead UmiSourceContractInternalRead
#define ScReadBytes UmiSourceContractInternalReadBytes
#define ScCollectGuides UmiSourceContractInternalCollectGuides
#define ScJoin UmiSourceContractInternalJoin
#define ScCollect UmiSourceContractInternalCollect
#define ScCollectManifests UmiSourceContractInternalCollectManifests
#define ScFilesFree UmiSourceContractInternalFilesFree
#define ScMetadata UmiSourceContractInternalMetadata
#define ScGuard UmiSourceContractInternalGuard
#define ScPortfolio UmiSourceContractInternalPortfolio
#define ScWorkbenchRules UmiSourceContractInternalWorkbenchRules

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
/* Narrow lexical admission shared by source/header audits; no compilation. */
int ScLiterals(const char *source, const char *path, UmiSourceContractReport *report);
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
/* Binary reads retain an extra sentinel byte; length describes original data. */
unsigned char *ScReadBytes(const char *path, size_t *length);
int ScCollectGuides(const char *root, ScFiles *files);
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
