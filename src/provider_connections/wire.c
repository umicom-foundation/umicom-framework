/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connections/wire.c
 *
 * PURPOSE:
 *   Encode portable connection records and reject ambiguous persisted data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include "umicom/provider_connections/document.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Fixed field order and hexadecimal UTF-8 bytes keep separators unambiguous.
 * This is encoding, not encryption: only public settings and references belong
 * in this format. Decode into temporary storage before publishing a value. */
static bool Append(char **cursor, size_t *remaining, const char *text)
{
    size_t length = strlen(text);
    if (length >= *remaining) return false;
    memcpy(*cursor, text, length + 1U);
    *cursor += length;
    *remaining -= length;
    return true;
}
static bool HexWrite(char **cursor, size_t *remaining, const char *text)
{
    static const char digits[] = "0123456789abcdef";
    size_t length = strlen(text);
    if (length > (*remaining > 1U ? (*remaining - 2U) / 2U : 0U) || *remaining < 2U) return false;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char value = (unsigned char)text[i];
        *(*cursor)++ = digits[value >> 4U];
        *(*cursor)++ = digits[value & 15U];
    }
    *(*cursor)++ = '\n';
    **cursor = '\0';
    *remaining -= length * 2U + 1U;
    return true;
}
static int HexDigit(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}
static bool HexRead(const char **cursor, char *out, size_t capacity)
{
    size_t length = 0U;
    while (**cursor != '\n') {
        int high = HexDigit((*cursor)[0]);
        if (high < 0) return false;
        int low = HexDigit((*cursor)[1]);
        if (low < 0 || (high == 0 && low == 0) || length + 1U >= capacity) return false;
        out[length++] = (char)((unsigned)high * 16U + (unsigned)low);
        *cursor += 2;
    }
    out[length] = '\0';
    ++*cursor;
    return true;
}
/* Read decimal values without locale, signs, leading zeroes or wraparound. */
static bool NumberRead(const char **cursor, uint64_t *out)
{
    const char *p = *cursor;
    if (*p < '0' || *p > '9') return false;
    if (*p == '0' && p[1] != '\n') return false;
    uint64_t value = 0U;
    while (*p >= '0' && *p <= '9') {
        unsigned digit = (unsigned)(*p++ - '0');
        if (value > (UINT64_MAX - digit) / 10U) return false;
        value = value * 10U + digit;
    }
    if (*p != '\n') return false;
    *cursor = p + 1;
    *out = value;
    return true;
}
UmiStatus UmiProviderConnectionEncode(const UmiProviderConnection *connection, char *out, size_t capacity)
{
    if (out == NULL || capacity == 0U || UmiProviderConnectionValidate(connection) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    char *cursor = out;
    size_t remaining = capacity;
    char numbers[64];
    int length = snprintf(numbers, sizeof(numbers), "connection\n%u\n%" PRIu32 "\n%u\n",
        (unsigned)connection->route, connection->timeout_ms, connection->enabled ? 1U : 0U);
    if (length < 0 || (size_t)length >= sizeof(numbers) || !Append(&cursor, &remaining, numbers))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *fields[] = {connection->id, connection->provider_id, connection->label,
        connection->endpoint, connection->model, connection->secret_reference};
    for (size_t i = 0U; i < sizeof(fields) / sizeof(fields[0]); ++i)
        if (!HexWrite(&cursor, &remaining, fields[i])) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
UmiStatus UmiProviderConnectionDecode(const char *wire, UmiProviderConnection *out)
{
    if (wire == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strncmp(wire, "connection\n", 11U) != 0) return UMI_STATUS_PARSE_ERROR;
    const char *cursor = wire + 11;
    uint64_t route, timeout, enabled;
    UmiProviderConnection value = {0};
    if (!NumberRead(&cursor, &route) || route < 1U || route > 2U ||
        !NumberRead(&cursor, &timeout) || timeout > UINT32_MAX ||
        !NumberRead(&cursor, &enabled) || enabled > 1U) return UMI_STATUS_PARSE_ERROR;
    value.route = (UmiProviderConnectionRoute)route;
    value.timeout_ms = (uint32_t)timeout;
    value.enabled = enabled != 0U;
    if (!HexRead(&cursor, value.id, sizeof(value.id)) ||
        !HexRead(&cursor, value.provider_id, sizeof(value.provider_id)) ||
        !HexRead(&cursor, value.label, sizeof(value.label)) ||
        !HexRead(&cursor, value.endpoint, sizeof(value.endpoint)) ||
        !HexRead(&cursor, value.model, sizeof(value.model)) ||
        !HexRead(&cursor, value.secret_reference, sizeof(value.secret_reference)) ||
        *cursor != '\0' || UmiProviderConnectionValidate(&value) != UMI_STATUS_OK)
        return UMI_STATUS_PARSE_ERROR;
    *out = value;
    return UMI_STATUS_OK;
}
UmiStatus UmiProviderConnectionIndexEncode(const UmiProviderConnectionSnapshot *snapshot, char *out, size_t capacity)
{
    if (snapshot == NULL || out == NULL || snapshot->count > UMI_PROVIDER_CONNECTION_LIMIT || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    char numbers[80];
    int length = snprintf(numbers, sizeof(numbers), "catalogue\n%" PRIu64 "\n%zu\n", snapshot->revision, snapshot->count);
    char *cursor = out;
    size_t remaining = capacity;
    if (length < 0 || (size_t)length >= sizeof(numbers) || !Append(&cursor, &remaining, numbers))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < snapshot->count; ++i) {
        if (!UmiProviderConnectionIdValid(snapshot->items[i].id, sizeof(snapshot->items[i].id)))
            return UMI_STATUS_INVALID_ARGUMENT;
        if (!Append(&cursor, &remaining, snapshot->items[i].id) || !Append(&cursor, &remaining, "\n"))
            return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiProviderConnectionIndexDecode(const char *wire, UmiProviderConnectionSnapshot *out)
{
    if (wire == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strncmp(wire, "catalogue\n", 10U) != 0) return UMI_STATUS_PARSE_ERROR;
    const char *cursor = wire + 10;
    uint64_t revision, count;
    if (!NumberRead(&cursor, &revision) || revision == 0U ||
        !NumberRead(&cursor, &count) || count > UMI_PROVIDER_CONNECTION_LIMIT) return UMI_STATUS_PARSE_ERROR;
    out->revision = revision;
    out->count = (size_t)count;
    for (size_t i = 0U; i < out->count; ++i) {
        const char *end = strchr(cursor, '\n');
        if (end == NULL || (size_t)(end - cursor) >= sizeof(out->items[i].id)) return UMI_STATUS_PARSE_ERROR;
        memcpy(out->items[i].id, cursor, (size_t)(end - cursor));
        out->items[i].id[end - cursor] = '\0';
        if (!UmiProviderConnectionIdValid(out->items[i].id, sizeof(out->items[i].id))) return UMI_STATUS_PARSE_ERROR;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(out->items[j].id, out->items[i].id) == 0) return UMI_STATUS_PARSE_ERROR;
        cursor = end + 1;
    }
    return *cursor == '\0' ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

/* Recovery identity reuses the storage owner's canonical field ordering and
 * validation. Hashing the C structure would include platform padding and stale
 * trailing bytes; a second serializer would risk disagreeing with persistence. */
UmiStatus UmiProviderConnectionFingerprint(const UmiProviderConnection *connection,
    unsigned char out_digest[UMI_SHA256_BYTES])
{
    if(out_digest==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char wire[UMI_PROVIDER_CONNECTION_WIRE_CAPACITY];
    UmiStatus status=UmiProviderConnectionEncode(connection,wire,sizeof(wire));
    if(status!=UMI_STATUS_OK) return status;
    static const char domain[]="Umicom provider connection fingerprint v1\n";
    UmiSha256 hash;UmiSha256Init(&hash);
    unsigned char candidate[UMI_SHA256_BYTES];
    status=UmiSha256Update(&hash,domain,sizeof(domain)-1U);
    if(status==UMI_STATUS_OK) status=UmiSha256Update(&hash,wire,strlen(wire));
    if(status==UMI_STATUS_OK) status=UmiSha256Final(&hash,candidate);
    if(status==UMI_STATUS_OK) memcpy(out_digest,candidate,sizeof(candidate));
    return status;
}

/* Receipt persistence shares the existing provider wire primitives. Digests
 * are raw lowercase hexadecimal lines; logical strings use HexWrite/HexRead.
 * Never write a C structure or its padding into durable storage. */
static bool TaskDigestWrite(char **cursor,size_t *remaining,const unsigned char *digest)
{
    char hex[UMI_SHA256_HEX_CAPACITY];UmiSha256Hex(digest,hex);
    return Append(cursor,remaining,hex) && Append(cursor,remaining,"\n");
}
static bool TaskDigestRead(const char **cursor,unsigned char *out)
{
    const char *p=*cursor;
    for(size_t i=0U;i<UMI_SHA256_BYTES;++i) {
        int high=HexDigit(*p);if(high<0) return false;++p;
        int low=HexDigit(*p);if(low<0) return false;++p;
        out[i]=(unsigned char)((unsigned)high*16U+(unsigned)low);
    }
    if(*p!='\n') return false;
    *cursor=p+1;return true;
}
UmiStatus UmiProviderTaskEncode(const UmiProviderTaskReceipt *receipt,char *out,
    size_t capacity,size_t *out_length)
{
    if(out==NULL || out_length==NULL || capacity==0U ||
        UmiProviderTaskValidate(receipt)!=UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    char wire[UMI_PROVIDER_TASK_WIRE_CAPACITY],numbers[160];
    char *cursor=wire;size_t remaining=sizeof(wire);
    int size=snprintf(numbers,sizeof(numbers),"provider-task\n%u\n%" PRIu64 "\n%" PRIu64 "\n%u\n%" PRIu64 "\n",
        (unsigned)receipt->schema,receipt->revision,receipt->connection_revision,(unsigned)receipt->phase,receipt->output_bytes);
    if(size<0 || (size_t)size>=sizeof(numbers) || !Append(&cursor,&remaining,numbers))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *fields[]={receipt->context.application_id,receipt->context.profile_id,
        receipt->context.local_id,receipt->context.operation_id,receipt->connection_id,
        receipt->provider_id,receipt->remote_id};
    for(size_t i=0U;i<sizeof(fields)/sizeof(fields[0]);++i)
        if(!HexWrite(&cursor,&remaining,fields[i])) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(!TaskDigestWrite(&cursor,&remaining,receipt->context.request_sha256) ||
        !TaskDigestWrite(&cursor,&remaining,receipt->connection_fingerprint) ||
        !TaskDigestWrite(&cursor,&remaining,receipt->output_sha256)) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t length=(size_t)(cursor-wire);
    if(length>=capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out,wire,length+1U);*out_length=length;return UMI_STATUS_OK;
}
UmiStatus UmiProviderTaskDecode(const char *wire,size_t length,UmiProviderTaskReceipt *out)
{
    if(wire==NULL || out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(length>=UMI_PROVIDER_TASK_WIRE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(length==0U || memchr(wire,'\0',length)!=NULL) return UMI_STATUS_PARSE_ERROR;
    /* Bound and terminate one private copy before reusing the existing text
     * readers. No parser may walk past a caller's exact byte extent. */
    char local[UMI_PROVIDER_TASK_WIRE_CAPACITY];memcpy(local,wire,length);local[length]='\0';
    static const char prefix[]="provider-task\n";
    if(length<sizeof(prefix)-1U || strncmp(local,prefix,sizeof(prefix)-1U)!=0) return UMI_STATUS_PARSE_ERROR;
    const char *cursor=local+sizeof(prefix)-1U;uint64_t schema,phase;
    UmiProviderTaskReceipt candidate={0};
    if(!NumberRead(&cursor,&schema)) return UMI_STATUS_PARSE_ERROR;
    if(schema!=UMI_PROVIDER_TASK_SCHEMA) return UMI_STATUS_NOT_IMPLEMENTED;
    candidate.schema=UMI_PROVIDER_TASK_SCHEMA;
    if(!NumberRead(&cursor,&candidate.revision) || !NumberRead(&cursor,&candidate.connection_revision) ||
        !NumberRead(&cursor,&phase) || phase>UMI_PROVIDER_TASK_OUTPUT_STORED ||
        !NumberRead(&cursor,&candidate.output_bytes)) return UMI_STATUS_PARSE_ERROR;
    candidate.phase=(UmiProviderTaskPhase)phase;
    if(!HexRead(&cursor,candidate.context.application_id,sizeof(candidate.context.application_id)) ||
        !HexRead(&cursor,candidate.context.profile_id,sizeof(candidate.context.profile_id)) ||
        !HexRead(&cursor,candidate.context.local_id,sizeof(candidate.context.local_id)) ||
        !HexRead(&cursor,candidate.context.operation_id,sizeof(candidate.context.operation_id)) ||
        !HexRead(&cursor,candidate.connection_id,sizeof(candidate.connection_id)) ||
        !HexRead(&cursor,candidate.provider_id,sizeof(candidate.provider_id)) ||
        !HexRead(&cursor,candidate.remote_id,sizeof(candidate.remote_id)) ||
        !TaskDigestRead(&cursor,candidate.context.request_sha256) ||
        !TaskDigestRead(&cursor,candidate.connection_fingerprint) ||
        !TaskDigestRead(&cursor,candidate.output_sha256) || *cursor!='\0' ||
        UmiProviderTaskValidate(&candidate)!=UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
    *out=candidate;return UMI_STATUS_OK;
}

/* Public exchange reuses the persistence owner's field order. A separate
 * envelope adds a schema and observed revision without changing stored keys,
 * fingerprints or the previous private record contract. */
UmiStatus UmiProviderConnectionDocumentExport(const UmiProviderConnection *connection,
    uint64_t observed_revision,char *out,size_t capacity,size_t *out_length)
{
    if(!out || !out_length || observed_revision==0U)return UMI_STATUS_INVALID_ARGUMENT;
    char wire[UMI_PROVIDER_CONNECTION_WIRE_CAPACITY];
    UmiStatus status=UmiProviderConnectionEncode(connection,wire,sizeof wire);
    if(status!=UMI_STATUS_OK)return status;
    char document[UMI_PROVIDER_CONNECTION_DOCUMENT_MAX_BYTES];
    int prefix=snprintf(document,sizeof document,"umicom-provider-connection\n1\n%" PRIu64 "\n",observed_revision);
    if(prefix<0 || (size_t)prefix>=sizeof document)return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t wire_length=strlen(wire),length=(size_t)prefix+wire_length;
    if(length>=sizeof document || length>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(document+(size_t)prefix,wire,wire_length+1U);
    memcpy(out,document,length+1U);*out_length=length;return UMI_STATUS_OK;
}
UmiStatus UmiProviderConnectionDocumentImport(const void *bytes,size_t length,
    UmiProviderConnection *out_connection,uint64_t *out_observed_revision)
{
    if(!bytes || !out_connection || !out_observed_revision)return UMI_STATUS_INVALID_ARGUMENT;
    if(length>=UMI_PROVIDER_CONNECTION_DOCUMENT_MAX_BYTES)return UMI_STATUS_CAPACITY_EXCEEDED;
    if(length==0U || memchr(bytes,'\0',length))return UMI_STATUS_PARSE_ERROR;
    static const char prefix[]="umicom-provider-connection\n";
    if(length<sizeof prefix-1U || memcmp(bytes,prefix,sizeof prefix-1U)!=0)return UMI_STATUS_PARSE_ERROR;
    char local[UMI_PROVIDER_CONNECTION_DOCUMENT_MAX_BYTES];
    memcpy(local,bytes,length);local[length]='\0';
    const char *cursor=local+sizeof prefix-1U;uint64_t version=0U,revision=0U;
    if(!NumberRead(&cursor,&version))return UMI_STATUS_PARSE_ERROR;
    if(version!=1U)return UMI_STATUS_NOT_IMPLEMENTED;
    if(!NumberRead(&cursor,&revision) || revision==0U)return UMI_STATUS_PARSE_ERROR;
    UmiProviderConnection candidate;
    UmiStatus status=UmiProviderConnectionDecode(cursor,&candidate);
    if(status!=UMI_STATUS_OK)return status;
    *out_connection=candidate;*out_observed_revision=revision;return UMI_STATUS_OK;
}
