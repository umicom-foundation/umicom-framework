/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/statement_archive.c
 *
 * PURPOSE:
 *   Recalculate saved statement reviews through the existing exact banking services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/bank_feed/statement_archive.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/json_writer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiBankStatementArchive {
    UmiBankFeed *feed;
    UmiBankStatementArchiveSnapshot snapshot;
};
/* Admission uses the existing archive decoder as the source of provenance and
 * row validation. The round trip makes the owned snapshot independent of both
 * caller memory and any future importer representation. */
UmiStatus UmiBankStatementArchiveCreate(const UmiBankFeed *feed,
    const UmiBankStatementRequest *request, const UmiBankFeedArchiveInfo *info,
    UmiBankStatementArchive **out) {
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiBankStatementResult result;
    UmiStatus status = UmiBankStatementReview(feed, request, &result);
    if (status != UMI_STATUS_OK) return status;
    UmiBankStatementArchive *archive = calloc(1U, sizeof *archive);
    if (archive == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* A successful scope lookup ensures these strings match a bounded row. */
    memcpy(archive->snapshot.account, request->account, strlen(request->account) + 1U);
    memcpy(archive->snapshot.currency, request->currency, strlen(request->currency) + 1U);
    archive->snapshot.firstDate = request->firstDate;
    archive->snapshot.lastDate = request->lastDate;
    archive->snapshot.openingBalance = request->openingBalance;
    archive->snapshot.closingBalance = request->closingBalance;
    archive->snapshot.result = result;
    char *encoded = NULL;
    size_t length = 0U;
    status = UmiBankFeedArchiveEncode(feed, info, &encoded, &length);
    if (status == UMI_STATUS_OK)
        status = UmiBankFeedArchiveDecode(encoded, length, &archive->feed, &archive->snapshot.source);
    UmiBankFeedArchiveFree(encoded);
    if (status != UMI_STATUS_OK) {
        UmiBankStatementArchiveDestroy(archive);
        return status;
    }
    *out = archive;
    return UMI_STATUS_OK;
}
void UmiBankStatementArchiveDestroy(UmiBankStatementArchive *archive) {
    if (archive == NULL) return;
    UmiBankFeedDestroy(archive->feed);
    free(archive);
}
UmiStatus UmiBankStatementArchiveRead(const UmiBankStatementArchive *archive,
    UmiBankStatementArchiveSnapshot *out) {
    if (archive == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = archive->snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiBankStatementArchiveCloneFeed(const UmiBankStatementArchive *archive,
    UmiBankFeed **out) {
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    return archive != NULL ? UmiBankFeedClone(archive->feed, out) : UMI_STATUS_INVALID_ARGUMENT;
}
static void Field(UmiLanguageRuntimeJsonWriter *writer, const char *name, const char *value) {
    (void)umi_language_runtime_json_writer_raw(writer, ",");
    (void)umi_language_runtime_json_writer_string(writer, name);
    (void)umi_language_runtime_json_writer_raw(writer, ":");
    (void)umi_language_runtime_json_writer_string(writer, value);
}
UmiStatus UmiBankStatementArchiveEncode(const UmiBankStatementArchive *archive,
    char **outBytes, size_t *outLength) {
    if (outBytes != NULL) *outBytes = NULL;
    if (outLength != NULL) *outLength = 0U;
    if (archive == NULL || outBytes == NULL || outLength == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char *observations = NULL;
    size_t observationLength = 0U;
    UmiStatus status = UmiBankFeedArchiveEncode(archive->feed, &archive->snapshot.source,
        &observations, &observationLength);
    if (status != UMI_STATUS_OK) return status;
    const UmiBankStatementArchiveSnapshot *scope = &archive->snapshot;
    /* Date values are validated, but give integer formatting its full static
     * bound too so diagnostic builds need not infer cross-function ranges. */
#if 0
    char first[11], last[11], opening[UMI_DECIMAL_TEXT_MAX], closing[UMI_DECIMAL_TEXT_MAX];
#endif
    char first[32], last[32], opening[UMI_DECIMAL_TEXT_MAX], closing[UMI_DECIMAL_TEXT_MAX];
    (void)snprintf(first, sizeof first, "%04d-%02u-%02u", (int)scope->firstDate.year,
        (unsigned)scope->firstDate.month, (unsigned)scope->firstDate.day);
    (void)snprintf(last, sizeof last, "%04d-%02u-%02u", (int)scope->lastDate.year,
        (unsigned)scope->lastDate.month, (unsigned)scope->lastDate.day);
    status = UmiDecimalFormat(scope->openingBalance, opening, sizeof opening);
    if (status == UMI_STATUS_OK) status = UmiDecimalFormat(scope->closingBalance, closing, sizeof closing);
    char *bytes = status == UMI_STATUS_OK ? malloc((size_t)UMI_BANK_STATEMENT_ARCHIVE_MAX_BYTES + 1U) : NULL;
    if (status == UMI_STATUS_OK && bytes == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK) {
        UmiLanguageRuntimeJsonWriter writer;
        umi_language_runtime_json_writer_init(&writer, bytes, (size_t)UMI_BANK_STATEMENT_ARCHIVE_MAX_BYTES + 1U);
        (void)umi_language_runtime_json_writer_raw(&writer,
            "{\"format\":\"umicom.bank-statement-review\",\"version\":1");
        Field(&writer, "account", scope->account);
        Field(&writer, "currency", scope->currency);
        Field(&writer, "first_date", first);
        Field(&writer, "last_date", last);
        Field(&writer, "opening_balance", opening);
        Field(&writer, "closing_balance", closing);
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"observations\":");
        (void)umi_language_runtime_json_writer_raw(&writer, observations);
        (void)umi_language_runtime_json_writer_raw(&writer, "}\n");
        status = writer.status;
        if (status == UMI_STATUS_OK) { *outBytes = bytes; *outLength = writer.length; }
    }
    UmiBankFeedArchiveFree(observations);
    if (status != UMI_STATUS_OK) free(bytes);
    return status;
}
static UmiStatus Text(const UmiJsonTree *tree, const char *key, char *out, size_t capacity) {
    int node;
    UmiStatus status = UmiJsonTreeMember(tree, 0, key, &node);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        status = UMI_STATUS_PARSE_ERROR;
    return status == UMI_STATUS_OK ? UmiJsonTreeText(tree, node, out, capacity) : status;
}
static UmiStatus Decimal(const char *text, UmiDecimal *out) {
    const char *point = strchr(text, '.');
    size_t scale = point != NULL ? strlen(point + 1) : 0U;
    if (scale > 9U) return UMI_STATUS_PARSE_ERROR;
    UmiDecimal value;
    UmiStatus status = UmiDecimalParse(text, strlen(text), (uint8_t)scale, &value);
    char canonical[UMI_DECIMAL_TEXT_MAX];
    if (status == UMI_STATUS_OK) status = UmiDecimalFormat(value, canonical, sizeof canonical);
    if (status == UMI_STATUS_OK && strcmp(canonical, text) != 0) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) *out = value;
    return status;
}
/* Only the new envelope is decoded here. The nested source span is handed
 * intact to the established observation codec; no duplicate row parser or
 * trusted serialized comparison result is introduced. */
UmiStatus UmiBankStatementArchiveDecode(const void *bytes, size_t length,
    UmiBankStatementArchive **out) {
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (bytes == NULL || length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_BANK_STATEMENT_ARCHIVE_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiJsonTree *tree = NULL;
    const UmiJsonTreeLimits limits = {UMI_BANK_STATEMENT_ARCHIVE_MAX_BYTES, 65568U, 26U};
    UmiStatus status = UmiJsonTreeCreate(bytes, length, &limits, NULL, &tree);
    if (status != UMI_STATUS_OK) return status;
    char format[40], account[161], currency[4], first[11], last[11];
    char opening[UMI_DECIMAL_TEXT_MAX], closing[UMI_DECIMAL_TEXT_MAX];
    int versionNode = -1, observationNode = -1;
    int64_t version = 0;
    UmiBankStatementRequest request = {0};
    if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT || UmiJsonTreeCount(tree, 0) != 9U)
        status = UMI_STATUS_PARSE_ERROR;
#define READ_TEXT(key, field) do { if (status == UMI_STATUS_OK) status = Text(tree, key, field, sizeof field); } while (0)
    READ_TEXT("format", format);
    if (status == UMI_STATUS_OK && strcmp(format, "umicom.bank-statement-review") != 0) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiJsonTreeMember(tree, 0, "version", &versionNode);
    if (status == UMI_STATUS_OK) status = UmiJsonTreeInteger(tree, versionNode, &version);
    if (status == UMI_STATUS_OK && version != 1) status = UMI_STATUS_NOT_IMPLEMENTED;
    READ_TEXT("account", account); READ_TEXT("currency", currency);
    READ_TEXT("first_date", first); READ_TEXT("last_date", last);
    READ_TEXT("opening_balance", opening); READ_TEXT("closing_balance", closing);
#undef READ_TEXT
    if (status == UMI_STATUS_OK) status = UmiBankStatementDate(first, &request.firstDate);
    if (status == UMI_STATUS_OK) status = UmiBankStatementDate(last, &request.lastDate);
    if (status == UMI_STATUS_OK) status = Decimal(opening, &request.openingBalance);
    if (status == UMI_STATUS_OK) status = Decimal(closing, &request.closingBalance);
    const char *span = NULL;
    size_t spanLength = 0U;
    if (status == UMI_STATUS_OK) status = UmiJsonTreeMember(tree, 0, "observations", &observationNode);
    if (status == UMI_STATUS_OK) status = UmiJsonTreeSourceSpan(tree, observationNode, &span, &spanLength);
    UmiBankFeed *feed = NULL;
    UmiBankFeedArchiveInfo info;
    if (status == UMI_STATUS_OK) status = UmiBankFeedArchiveDecode(span, spanLength, &feed, &info);
    request.account = account; request.currency = currency;
    if (status == UMI_STATUS_OK) status = UmiBankStatementArchiveCreate(feed, &request, &info, out);
    UmiBankFeedDestroy(feed);
    UmiJsonTreeDestroy(tree);
    return status;
}
