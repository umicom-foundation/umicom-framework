/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/archive.c
 * PURPOSE: Persist validated bank observations using shared JSON, amount and timestamp services.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/bank_feed/archive.h"
#include "feed_private.h"
#include "umicom/bank_feed/statement.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/json_writer.h"
#include <stdlib.h>
#include <string.h>

static bool ArchiveInfoValid(const UmiBankFeedArchiveInfo *info) {
    if ((unsigned)info->sourceFormat > (unsigned)UMI_BANK_FEED_SOURCE_MAPPED_CSV) return false;
    const char *end = memchr(info->sourceSha256, '\0', sizeof info->sourceSha256);
    if (end == NULL) return false;
    size_t length = (size_t)(end - info->sourceSha256);
    if (length != 0U && length != 64U) return false;
    for (size_t i = 0U; i < length; ++i) {
        char c = info->sourceSha256[i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return false;
    }
    return true;
}
/* Share claim validation with native constructors without imposing the archive
 * byte budget on unrelated statement arithmetic. */
UmiStatus UmiBankFeedArchiveInfoValidate(const UmiBankFeedArchiveInfo *info) {
    return info != NULL && ArchiveInfoValid(info) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* The common JSON writer owns escaping and its sticky capacity failure. This
 * adapter supplies only fixed keys and already validated, owned feed values. */
static void TextField(UmiLanguageRuntimeJsonWriter *writer, const char *key, const char *value) {
    (void)umi_language_runtime_json_writer_string(writer, key);
    (void)umi_language_runtime_json_writer_raw(writer, ":");
    (void)umi_language_runtime_json_writer_string(writer, value);
}
static void FlagField(UmiLanguageRuntimeJsonWriter *writer, const char *key, bool value) {
    (void)umi_language_runtime_json_writer_string(writer, key);
    (void)umi_language_runtime_json_writer_raw(writer, ":");
    (void)umi_language_runtime_json_writer_bool(writer, value);
}
void UmiBankFeedArchiveFree(char *bytes) { free(bytes); }
UmiStatus UmiBankFeedArchiveEncode(const UmiBankFeed *feed,
    const UmiBankFeedArchiveInfo *info, char **outBytes, size_t *outLength) {
    if (outBytes != NULL) *outBytes = NULL;
    if (outLength != NULL) *outLength = 0U;
    if (feed == NULL || outBytes == NULL || outLength == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiBankFeedArchiveInfo source = {0};
    if (info != NULL) source = *info;
    if (!ArchiveInfoValid(&source)) return UMI_STATUS_INVALID_ARGUMENT;
    char *bytes = malloc((size_t)UMI_BANK_FEED_MAX_BYTES + 1U);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, bytes, (size_t)UMI_BANK_FEED_MAX_BYTES + 1U);
    (void)umi_language_runtime_json_writer_raw(&writer,
        "{\"format\":\"umicom.bank-observations\",\"version\":1,\"source_format\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, (uint64_t)source.sourceFormat);
    (void)umi_language_runtime_json_writer_raw(&writer, ",");
    TextField(&writer, "source_sha256", source.sourceSha256);
    (void)umi_language_runtime_json_writer_raw(&writer, ",");
    FlagField(&writer, "previous_page", feed->summary.hasPreviousPage);
    (void)umi_language_runtime_json_writer_raw(&writer, ",");
    FlagField(&writer, "next_page", feed->summary.hasNextPage);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"transactions\":[");
    for (size_t i = 0U; writer.status == UMI_STATUS_OK && i < feed->summary.rows; ++i) {
        const UmiBankFeedRow *row = &feed->rows[i];
        if (i != 0U) (void)umi_language_runtime_json_writer_raw(&writer, ",");
        (void)umi_language_runtime_json_writer_raw(&writer, "{");
        const char *keys[] = {"account", "transaction", "reference", "information", "booking_time", "currency", "amount"};
        const char *values[] = {row->account, row->transaction, row->reference, row->information,
                               row->bookingTime, row->currency, row->amountText};
        for (size_t j = 0U; j < 7U; ++j) {
            if (j != 0U) (void)umi_language_runtime_json_writer_raw(&writer, ",");
            TextField(&writer, keys[j], values[j]);
        }
        (void)umi_language_runtime_json_writer_raw(&writer, ",");
        FlagField(&writer, "credit", row->credit);
        (void)umi_language_runtime_json_writer_raw(&writer, ",");
        FlagField(&writer, "booked", row->booked);
        (void)umi_language_runtime_json_writer_raw(&writer, ",");
        FlagField(&writer, "mutable", row->mutableRecord);
        (void)umi_language_runtime_json_writer_raw(&writer, "}");
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "]}\n");
    if (writer.status == UMI_STATUS_OK) {
        *outBytes = bytes; *outLength = writer.length;
    } else free(bytes);
    return writer.status;
}
static UmiStatus Member(const UmiJsonTree *tree, int object, const char *key, int *node) {
    return UmiJsonTreeMember(tree, object, key, node);
}
static UmiStatus Text(const UmiJsonTree *tree, int object, const char *key,
    char *out, size_t capacity, size_t characters, bool multiline, bool required) {
    int node;
    UmiStatus status = Member(tree, object, key, &node);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiJsonTreeText(tree, node, out, capacity);
    if (status != UMI_STATUS_OK) return status;
    size_t count = 0U;
    /* The tree validates UTF-8. Information may retain CSV line breaks; other
     * fields follow bank identifiers' control-free character limits. */
    for (const unsigned char *p = (const unsigned char *)out; *p != 0U; ++p) {
        if ((*p < 32U || *p == 127U) && !(multiline && (*p == '\t' || *p == '\r' || *p == '\n')))
            return UMI_STATUS_PARSE_ERROR;
        if ((*p & 0xc0U) != 0x80U) ++count;
    }
    if (count > characters) return UMI_STATUS_CAPACITY_EXCEEDED;
    return required && count == 0U ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
}
static UmiStatus Flag(const UmiJsonTree *tree, int object, const char *key, bool *out) {
    int node, value = 0;
    UmiStatus status = Member(tree, object, key, &node);
    if (status == UMI_STATUS_OK) status = UmiJsonTreeBoolean(tree, node, &value);
    if (status == UMI_STATUS_OK) *out = value != 0;
    return status;
}
static UmiStatus Integer(const UmiJsonTree *tree, const char *key, int64_t *out) {
    int node;
    UmiStatus status = Member(tree, 0, key, &node);
    return status == UMI_STATUS_OK ? UmiJsonTreeInteger(tree, node, out) : status;
}
static UmiStatus Row(const UmiJsonTree *tree, int node, UmiBankFeedRow *row) {
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
        UmiJsonTreeCount(tree, node) != 10U) return UMI_STATUS_PARSE_ERROR;
#define ARCHIVE_TEXT(key, field, count, multiline, required) do { \
    UmiStatus status = Text(tree, node, key, row->field, sizeof row->field, count, multiline, required); \
    if (status != UMI_STATUS_OK) return status; \
} while (0)
    ARCHIVE_TEXT("account", account, 40U, false, true);
    ARCHIVE_TEXT("transaction", transaction, 210U, false, false);
    ARCHIVE_TEXT("reference", reference, 210U, false, false);
    ARCHIVE_TEXT("information", information, 500U, true, false);
    ARCHIVE_TEXT("booking_time", bookingTime, 39U, false, true);
    ARCHIVE_TEXT("currency", currency, 3U, false, true);
    ARCHIVE_TEXT("amount", amountText, 23U, false, true);
#undef ARCHIVE_TEXT
    UmiStatus status = Flag(tree, node, "credit", &row->credit);
    if (status == UMI_STATUS_OK) status = Flag(tree, node, "booked", &row->booked);
    if (status == UMI_STATUS_OK) status = Flag(tree, node, "mutable", &row->mutableRecord);
    if (status != UMI_STATUS_OK) return status;
    UmiFinancialDate date;
    const bool dateValid = strlen(row->bookingTime) == 10U
        ? UmiBankStatementDate(row->bookingTime, &date) == UMI_STATUS_OK
        : BankFeedTimestamp(row->bookingTime);
    if ((!row->booked && !row->mutableRecord) || !dateValid)
        return UMI_STATUS_PARSE_ERROR;
    return BankFeedAmount(row);
}
UmiStatus UmiBankFeedArchiveDecode(const void *bytes, size_t length,
    UmiBankFeed **out, UmiBankFeedArchiveInfo *outInfo) {
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (bytes == NULL || length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_BANK_FEED_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiJsonTree *tree = NULL;
    const UmiJsonTreeLimits limits = {UMI_BANK_FEED_MAX_BYTES, 65536U, 24U};
    UmiStatus status = UmiJsonTreeCreate(bytes, length, &limits, NULL, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiBankFeedArchiveInfo info = {0};
    char format[40] = {0};
    int64_t version = 0, sourceFormat = 0;
    int array = -1;
    bool previous = false, next = false;
    if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
        UmiJsonTreeCount(tree, 0) != 7U) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = Text(tree, 0, "format", format, sizeof format, 39U, false, true);
    if (status == UMI_STATUS_OK && strcmp(format, "umicom.bank-observations") != 0) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = Integer(tree, "version", &version);
    if (status == UMI_STATUS_OK && version != 1) status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status == UMI_STATUS_OK) status = Integer(tree, "source_format", &sourceFormat);
    if (status == UMI_STATUS_OK && (sourceFormat < 0 || sourceFormat > 3)) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) {
        info.sourceFormat = (UmiBankFeedSourceFormat)sourceFormat;
        status = Text(tree, 0, "source_sha256", info.sourceSha256, sizeof info.sourceSha256, 64U, false, false);
    }
    if (status == UMI_STATUS_OK && !ArchiveInfoValid(&info)) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = Flag(tree, 0, "previous_page", &previous);
    if (status == UMI_STATUS_OK) status = Flag(tree, 0, "next_page", &next);
    if (status == UMI_STATUS_OK) status = Member(tree, 0, "transactions", &array);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, array) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        status = UMI_STATUS_PARSE_ERROR;
    size_t count = status == UMI_STATUS_OK ? UmiJsonTreeCount(tree, array) : 0U;
    if (count > UMI_BANK_FEED_MAX_ROWS) status = UMI_STATUS_CAPACITY_EXCEEDED;
    UmiBankFeed *feed = NULL;
    if (status == UMI_STATUS_OK) {
        feed = calloc(1U, sizeof *feed);
        if (feed == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK && count != 0U) {
        feed->rows = calloc(count, sizeof *feed->rows);
        if (feed->rows == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    for (int node = status == UMI_STATUS_OK ? UmiJsonTreeFirst(tree, array) : -1;
         status == UMI_STATUS_OK && node >= 0; node = UmiJsonTreeNext(tree, node)) {
        UmiBankFeedRow *row = &feed->rows[feed->summary.rows];
        status = Row(tree, node, row);
        if (status != UMI_STATUS_OK) break;
        if (row->transaction[0] == '\0') ++feed->summary.missingIdentifiers;
        else for (size_t i = 0U; i < feed->summary.rows; ++i)
            if (!strcmp(row->account, feed->rows[i].account) &&
                !strcmp(row->transaction, feed->rows[i].transaction)) {
                status = UMI_STATUS_ALREADY_EXISTS; break;
            }
        ++feed->summary.rows;
    }
    UmiJsonTreeDestroy(tree);
    /* Publish the feed and provenance together only after the final row passes.
     * Parsing never converts an archive into evidence of a trusted provider. */
    if (status == UMI_STATUS_OK) {
        feed->summary.hasPreviousPage = previous; feed->summary.hasNextPage = next;
        if (outInfo != NULL) *outInfo = info;
        *out = feed;
    } else UmiBankFeedDestroy(feed);
    return status;
}
