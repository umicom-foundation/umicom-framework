/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/healthcare/access_archive.c
 * PURPOSE: Use a strict versioned format and transactional domain restore for synthetic exercises.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/healthcare/access_archive.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/json_writer.h"
#include <stdlib.h>
#include <string.h>

typedef UmiLanguageRuntimeJsonWriter Writer;

/* Use the existing escaping writer; schemas never concatenate input as JSON. */
static void text_field(Writer *writer, const char *key, const char *value)
{
    (void)umi_language_runtime_json_writer_string(writer, key);
    (void)umi_language_runtime_json_writer_raw(writer, ":");
    (void)umi_language_runtime_json_writer_string(writer, value);
}
static void number_field(Writer *writer, const char *key, uint64_t value)
{
    (void)umi_language_runtime_json_writer_raw(writer, ",");
    (void)umi_language_runtime_json_writer_string(writer, key);
    (void)umi_language_runtime_json_writer_raw(writer, ":");
    (void)umi_language_runtime_json_writer_uint64(writer, value);
}
static void boolean_field(Writer *writer, const char *key, int value)
{
    (void)umi_language_runtime_json_writer_raw(writer, ",");
    (void)umi_language_runtime_json_writer_string(writer, key);
    (void)umi_language_runtime_json_writer_raw(writer, ":");
    (void)umi_language_runtime_json_writer_bool(writer, value);
}
static void encode_request(Writer *writer, const UmiHealthcareAccessRecord *record)
{
    (void)umi_language_runtime_json_writer_raw(writer, "{");
    text_field(writer, "request", record->requestId);
    (void)umi_language_runtime_json_writer_raw(writer, ",");
    text_field(writer, "patient", record->patientId);
    (void)umi_language_runtime_json_writer_raw(writer, ",");
    text_field(writer, "item", record->itemId);
    (void)umi_language_runtime_json_writer_raw(writer, ",");
    text_field(writer, "provider", record->providerId);
    (void)umi_language_runtime_json_writer_raw(writer, ",");
    text_field(writer, "owner", record->followupOwnerId);
    number_field(writer, "revision", record->revision);
    number_field(writer, "requested", record->requestedQuantity);
    number_field(writer, "offered", record->offeredQuantity);
    number_field(writer, "accepted", record->acceptedQuantity);
    number_field(writer, "collected", record->collectedQuantity);
    number_field(writer, "reply", (uint64_t)record->reply);
    boolean_field(writer, "funded", record->simulatedFundingReady);
    boolean_field(writer, "reserved", record->reservationConfirmed);
    number_field(writer, "reserved_at", record->reservedAt);
    number_field(writer, "expires_at", record->expiresAt);
    number_field(writer, "unmet", (uint64_t)record->unmetReason);
    (void)umi_language_runtime_json_writer_raw(writer, "}");
}

UmiStatus UmiHealthcareAccessArchiveEncode(
    const UmiHealthcareSyntheticRegistry *registry,
    const UmiHealthcareAccessBook *book, char **outBytes, size_t *outLength)
{
    UmiHealthcareAccessRecord records[UMI_HEALTHCARE_ACCESS_CAPACITY];
    UmiHealthcareAccessBook *checked = NULL;
    UmiHealthcareSyntheticPatient patient;
    Writer writer;
    size_t patientCount = 0U, count = 0U;
    UmiStatus status;
    char *bytes;
    if (outBytes != NULL) *outBytes = NULL;
    if (outLength != NULL) *outLength = 0U;
    if (outBytes == NULL || outLength == NULL || registry == NULL || book == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiHealthcareSyntheticRegistryCount(registry, &patientCount);
    if (status != UMI_STATUS_OK) return status;
    status = UmiHealthcareAccessCount(book, &count);
    if (status != UMI_STATUS_OK) return status;
    if (count > UMI_HEALTHCARE_ACCESS_CAPACITY ||
        patientCount > UMI_HEALTHCARE_SYNTHETIC_MAX_PATIENTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i) {
        status = UmiHealthcareAccessAt(book, i, &records[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    /* Check registry/book agreement before allocating public archive bytes. */
    status = UmiHealthcareAccessBookRestore(registry, records, count, &checked);
    UmiHealthcareAccessBookDestroy(checked);
    if (status != UMI_STATUS_OK) return status;
    bytes = malloc(UMI_HEALTHCARE_ACCESS_ARCHIVE_MAX_BYTES + 1U);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_language_runtime_json_writer_init(&writer, bytes, UMI_HEALTHCARE_ACCESS_ARCHIVE_MAX_BYTES + 1U);
    (void)umi_language_runtime_json_writer_raw(&writer,
        "{\"format\":\"umicom.synthetic-access\",\"version\":1,\"patients\":[");
    for (size_t i = 0U; i < patientCount; ++i) {
        status = UmiHealthcareSyntheticRegistryAt(registry, i, &patient);
        if (status != UMI_STATUS_OK) { free(bytes); return status; }
        (void)umi_language_runtime_json_writer_raw(&writer, i == 0U ? "{" : ",{");
        text_field(&writer, "patient", patient.patientId);
        number_field(&writer, "revision", patient.revision);
        (void)umi_language_runtime_json_writer_raw(&writer, "}");
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "],\"requests\":[");
    for (size_t i = 0U; i < count; ++i) {
        if (i != 0U) (void)umi_language_runtime_json_writer_raw(&writer, ",");
        encode_request(&writer, &records[i]);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "]}\n");
    if (writer.status != UMI_STATUS_OK) { free(bytes); return writer.status; }
    *outBytes = bytes;
    *outLength = writer.length;
    return UMI_STATUS_OK;
}

void UmiHealthcareAccessArchiveFree(void *bytes) { free(bytes); }

static UmiStatus read_text(const UmiJsonTree *tree, int object, const char *key,
    char *out, size_t capacity)
{
    int node;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
    return status == UMI_STATUS_OK ? UmiJsonTreeText(tree, node, out, capacity) : status;
}

/* Revisions and deadlines are unsigned 64-bit integers. Inspect the validated
 * spelling rather than coercing through a signed integer or a floating point
 * value, so UINT64_MAX and integers above 2^53 retain their exact values. */
static UmiStatus read_number(const UmiJsonTree *tree, int object, const char *key,
    uint64_t limit, uint64_t *out)
{
    int node;
    const char *text = NULL;
    size_t length = 0U;
    uint64_t value = 0U;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE)
        return UMI_STATUS_PARSE_ERROR;
    status = UmiJsonTreeSourceSpan(tree, node, &text, &length);
    if (status != UMI_STATUS_OK) return status;
    if (length == 0U) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < length; ++i) {
        if (text[i] < '0' || text[i] > '9') return UMI_STATUS_PARSE_ERROR;
        uint64_t digit = (uint64_t)(text[i] - '0');
        if (digit > limit || value > (limit - digit) / 10U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        value = value * 10U + digit;
    }
    *out = value;
    return UMI_STATUS_OK;
}
static UmiStatus read_boolean(const UmiJsonTree *tree, int object, const char *key, int *out)
{
    int node;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
    return status == UMI_STATUS_OK ? UmiJsonTreeBoolean(tree, node, out) : status;
}
static int object_shape(const UmiJsonTree *tree, int node, size_t members)
{
    return UmiJsonTreeKind(tree, node) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT &&
        UmiJsonTreeCount(tree, node) == members;
}

static UmiStatus decode_request(const UmiJsonTree *tree, int node,
    UmiHealthcareAccessRecord *record)
{
    UmiStatus status;
    uint64_t value;
    if (!object_shape(tree, node, 16U)) return UMI_STATUS_PARSE_ERROR;
    /* Reading every required member plus the exact member count rejects both
     * unknown fields and duplicate names, including escaped duplicate keys. */
#define READ_ID(key, member) do { \
    status = read_text(tree, node, key, record->member, sizeof(record->member)); \
    if (status != UMI_STATUS_OK) return status; \
} while (0)
#define READ_NUMBER(key, member, limit, type) do { \
    status = read_number(tree, node, key, limit, &value); \
    if (status != UMI_STATUS_OK) return status; \
    record->member = (type)value; \
} while (0)
    READ_ID("request", requestId);
    READ_ID("patient", patientId);
    READ_ID("item", itemId);
    READ_ID("provider", providerId);
    READ_ID("owner", followupOwnerId);
    READ_NUMBER("revision", revision, UINT64_MAX, uint64_t);
    READ_NUMBER("requested", requestedQuantity, UMI_HEALTHCARE_ACCESS_QUANTITY_LIMIT, uint32_t);
    READ_NUMBER("offered", offeredQuantity, UMI_HEALTHCARE_ACCESS_QUANTITY_LIMIT, uint32_t);
    READ_NUMBER("accepted", acceptedQuantity, UMI_HEALTHCARE_ACCESS_QUANTITY_LIMIT, uint32_t);
    READ_NUMBER("collected", collectedQuantity, UMI_HEALTHCARE_ACCESS_QUANTITY_LIMIT, uint32_t);
    READ_NUMBER("reply", reply, UMI_HEALTHCARE_ACCESS_NO_RESPONSE, UmiHealthcareAccessReply);
    READ_NUMBER("reserved_at", reservedAt, UINT64_MAX, uint64_t);
    READ_NUMBER("expires_at", expiresAt, UINT64_MAX, uint64_t);
    READ_NUMBER("unmet", unmetReason, UMI_HEALTHCARE_ACCESS_UNMET_EXPIRED, UmiHealthcareAccessUnmetReason);
#undef READ_ID
#undef READ_NUMBER
    status = read_boolean(tree, node, "funded", &record->simulatedFundingReady);
    if (status != UMI_STATUS_OK) return status;
    return read_boolean(tree, node, "reserved", &record->reservationConfirmed);
}

UmiStatus UmiHealthcareAccessArchiveDecode(const void *bytes, size_t length,
    UmiHealthcareSyntheticRegistry **outRegistry, UmiHealthcareAccessBook **outBook)
{
    UmiJsonTreeLimits limits = {UMI_HEALTHCARE_ACCESS_ARCHIVE_MAX_BYTES, 2048U, 8U};
    UmiJsonTree *tree = NULL;
    UmiHealthcareSyntheticRegistry *registry = NULL;
    UmiHealthcareAccessBook *book = NULL;
    UmiHealthcareAccessRecord records[UMI_HEALTHCARE_ACCESS_CAPACITY] = {0};
    UmiHealthcareSyntheticPatient patient;
    char format[32], patientId[UMI_HEALTHCARE_SYNTHETIC_ID_CAPACITY];
    uint64_t version, revision;
    int patients, requests, node;
    size_t count = 0U;
    UmiStatus status;
    if (outRegistry != NULL) *outRegistry = NULL;
    if (outBook != NULL) *outBook = NULL;
    if (outRegistry == NULL || outBook == NULL || bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiJsonTreeCreate(bytes, length, &limits, NULL, &tree);
    if (status != UMI_STATUS_OK) return status;
#define REQUIRE(step) do { status = (step); if (status != UMI_STATUS_OK) goto finish; } while (0)
    if (!object_shape(tree, 0, 4U)) { status = UMI_STATUS_PARSE_ERROR; goto finish; }
    REQUIRE(read_text(tree, 0, "format", format, sizeof(format)));
    REQUIRE(read_number(tree, 0, "version", UINT64_MAX, &version));
    if (strcmp(format, "umicom.synthetic-access") != 0 || version != 1U) {
        status = UMI_STATUS_PARSE_ERROR;
        goto finish;
    }
    REQUIRE(UmiJsonTreeMember(tree, 0, "patients", &patients));
    REQUIRE(UmiJsonTreeMember(tree, 0, "requests", &requests));
    if (UmiJsonTreeKind(tree, patients) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY ||
        UmiJsonTreeKind(tree, requests) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY) {
        status = UMI_STATUS_PARSE_ERROR;
        goto finish;
    }
    if (UmiJsonTreeCount(tree, patients) > UMI_HEALTHCARE_SYNTHETIC_MAX_PATIENTS ||
        UmiJsonTreeCount(tree, requests) > UMI_HEALTHCARE_ACCESS_CAPACITY) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto finish;
    }
    REQUIRE(UmiHealthcareSyntheticRegistryCreate(&registry));
    for (node = UmiJsonTreeFirst(tree, patients); node >= 0; node = UmiJsonTreeNext(tree, node)) {
        if (!object_shape(tree, node, 2U)) { status = UMI_STATUS_PARSE_ERROR; goto finish; }
        REQUIRE(read_text(tree, node, "patient", patientId, sizeof(patientId)));
        REQUIRE(read_number(tree, node, "revision", UINT64_MAX, &revision));
        REQUIRE(UmiHealthcareSyntheticRegistryRegister(registry, patientId, &patient));
        if (patient.revision != revision) { status = UMI_STATUS_INVALID_STATE; goto finish; }
    }
    for (node = UmiJsonTreeFirst(tree, requests); node >= 0; node = UmiJsonTreeNext(tree, node)) {
        REQUIRE(decode_request(tree, node, &records[count]));
        ++count;
    }
    REQUIRE(UmiHealthcareAccessBookRestore(registry, records, count, &book));
    *outRegistry = registry;
    *outBook = book;
    registry = NULL;
    book = NULL;
finish:
    UmiHealthcareAccessBookDestroy(book);
    UmiHealthcareSyntheticRegistryDestroy(registry);
    UmiJsonTreeDestroy(tree);
    return status;
#undef REQUIRE
}
