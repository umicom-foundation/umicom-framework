/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/asset_library_archive.c
 * PURPOSE: Encode complete asset collections and reject malformed bundles before publishing owned data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "asset_library_wire.h"
#include "asset_library_internal.h"
#include "asset_archive_internal.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

static const unsigned char library_magic[8] = {'U', 'M', 'I', 'L', 'I', 'B', 'R', 'Y'};
static void Put(unsigned char *out, uint64_t value, size_t count)
{
    for (size_t i = 0U; i < count; ++i)
    {
        out[i] = (unsigned char)(value & 255U);
        value >>= 8U;
    }
}
static uint64_t Get(const unsigned char *bytes, size_t count)
{
    uint64_t value = 0U;
    for (size_t i = 0U; i < count; ++i)
        value |= (uint64_t)bytes[i] << (8U * i);
    return value;
}
typedef struct Writer
{
    UmiCreativeLibraryEmit emit;
    void *context;
    const UmiCancellationToken *cancel;
    UmiNativeSha256 hash;
} Writer;
/* One chunk boundary serves hashing, memory output, disk output and cancellation.
 * The archive's final digest covers framing as well as payload bytes. */
static UmiStatus Emit(Writer *writer, const void *bytes, size_t size)
{
    for (size_t offset = 0U; offset < size;)
    {
        if (umi_cancellation_token_is_requested(writer->cancel))
            return UMI_STATUS_CANCELLED;
        size_t count = size - offset;
        if (count > 65536U)
            count = 65536U;
        const unsigned char *chunk = (const unsigned char *)bytes + offset;
        UmiStatus status = writer->emit(writer->context, chunk, count);
        if (status == UMI_STATUS_OK)
            status = UmiNativeSha256Update(&writer->hash, chunk, count);
        if (status != UMI_STATUS_OK)
            return status;
        offset += count;
    }
    return umi_cancellation_token_is_requested(writer->cancel) ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}
UmiStatus UmiCreativeLibraryArchiveSize(const UmiCreativeAssetLibrary *library, size_t *out)
{
    if (library == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t size = 64U + strlen(library->info.title);
    for (size_t i = 0U; i < library->info.asset_count; ++i)
    {
        UmiCreativeAssetInfo info;
        UmiStatus status = UmiCreativeAssetInspect(library->slots[i].asset, &info);
        if (status != UMI_STATUS_OK)
            return status;
        size += 16U + strlen(library->slots[i].id) + 56U + strlen(info.label) + info.byte_count;
    }
    if (size > UMI_CREATIVE_LIBRARY_ARCHIVE_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *out = size;
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeLibraryArchiveWrite(const UmiCreativeAssetLibrary *library,
                                         const UmiCancellationToken *cancel, UmiCreativeLibraryEmit emit,
                                         void *context)
{
    if (library == NULL || emit == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    unsigned char head[32] = {0};
    memcpy(head, library_magic, sizeof(library_magic));
    Put(head + 8U, 1U, 2U); /* Storage schema, independent of product releases. */
    Put(head + 12U, (uint64_t)library->info.asset_count, 4U);
    size_t title_size = strlen(library->info.title);
    Put(head + 16U, (uint64_t)title_size, 2U);
    Put(head + 24U, (uint64_t)library->info.byte_count, 8U);
    Writer writer = {0};
    writer.emit = emit;
    writer.context = context;
    writer.cancel = cancel;
    UmiNativeSha256Init(&writer.hash);
    UmiStatus status = Emit(&writer, head, sizeof(head));
    if (status == UMI_STATUS_OK)
        status = Emit(&writer, library->info.title, title_size);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < library->info.asset_count; ++i)
    {
        const UmiCreativeLibrarySlot *slot = &library->slots[i];
        UmiCreativeAssetEnvelope envelope;
        status = UmiCreativeAssetArchiveDescribe(slot->asset, cancel, &envelope);
        if (status != UMI_STATUS_OK)
            break;
        unsigned char record[16] = {0};
        size_t id_size = strlen(slot->id);
        Put(record, (uint64_t)(envelope.metadata_size + envelope.payload_size + sizeof(envelope.digest)), 8U);
        Put(record + 8U, (uint64_t)id_size, 2U);
        status = Emit(&writer, record, sizeof(record));
        if (status == UMI_STATUS_OK)
            status = Emit(&writer, slot->id, id_size);
        if (status == UMI_STATUS_OK)
            status = Emit(&writer, envelope.metadata, envelope.metadata_size);
        if (status == UMI_STATUS_OK)
            status = Emit(&writer, envelope.payload, envelope.payload_size);
        if (status == UMI_STATUS_OK)
            status = Emit(&writer, envelope.digest, sizeof(envelope.digest));
    }
    unsigned char digest[UMI_NATIVE_SHA256_BYTES];
    if (status == UMI_STATUS_OK)
        status = UmiNativeSha256Final(&writer.hash, digest);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    /* Do not hash the final digest into itself. The receiving sink still uses
     * the same bounded, explicit byte contract as every earlier write. */
    if (status == UMI_STATUS_OK)
        status = emit(context, digest, sizeof(digest));
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    return status;
}
typedef struct Memory
{
    unsigned char *bytes;
    size_t size, capacity;
} Memory;
static UmiStatus MemoryWrite(void *context, const void *bytes, size_t count)
{
    Memory *memory = context;
    if (count > memory->capacity - memory->size)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(memory->bytes + memory->size, bytes, count);
    memory->size += count;
    return UMI_STATUS_OK;
}
UmiStatus UmiCreativeAssetLibraryArchiveEncode(const UmiCreativeAssetLibrary *library,
                                               const UmiCancellationToken *cancel, unsigned char **out_bytes,
                                               size_t *out_size)
{
    if (out_bytes == NULL || out_size == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out_bytes != NULL)
        return UMI_STATUS_INVALID_STATE;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    size_t size = 0U;
    UmiStatus status = UmiCreativeLibraryArchiveSize(library, &size);
    if (status != UMI_STATUS_OK)
        return status;
    Memory memory = {malloc(size), 0U, size};
    if (memory.bytes == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiCreativeLibraryArchiveWrite(library, cancel, MemoryWrite, &memory);
    if (status == UMI_STATUS_OK && memory.size != size)
        status = UMI_STATUS_INVALID_STATE;
    if (status != UMI_STATUS_OK)
    {
        free(memory.bytes);
        return status;
    }
    *out_bytes = memory.bytes;
    *out_size = memory.size;
    return UMI_STATUS_OK;
}
void UmiCreativeAssetLibraryArchiveFree(void *bytes) { free(bytes); }
static UmiStatus ValidateDigest(const unsigned char *bytes, size_t size, const UmiCancellationToken *cancel)
{
    UmiNativeSha256 hash;
    unsigned char digest[UMI_NATIVE_SHA256_BYTES];
    UmiNativeSha256Init(&hash);
    for (size_t offset = 0U; offset < size;)
    {
        if (umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        size_t count = size - offset;
        if (count > 65536U)
            count = 65536U;
        UmiStatus status = UmiNativeSha256Update(&hash, bytes + offset, count);
        if (status != UMI_STATUS_OK)
            return status;
        offset += count;
    }
    UmiStatus status = UmiNativeSha256Final(&hash, digest);
    if (status != UMI_STATUS_OK)
        return status;
    return memcmp(digest, bytes + size, sizeof(digest)) == 0 ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
/* Read lengths before forming views into the bundle. Every subtraction uses a
 * cursor already bounded by the enclosing envelope, and the aggregate payload
 * budget decreases as complete captures are accepted into the temporary owner. */
static UmiStatus ReadRecord(const unsigned char *wire, size_t end, size_t *cursor, size_t maximum_bytes,
                            const UmiCancellationToken *cancel, UmiCreativeAssetLibrary *library)
{
    if (end - *cursor < 16U)
        return UMI_STATUS_PARSE_ERROR;
    const unsigned char *record = wire + *cursor;
    uint64_t archive_size = Get(record, 8U);
    size_t id_size = (size_t)Get(record + 8U, 2U);
    if (Get(record + 10U, 6U) != 0U || id_size == 0U || id_size >= UMI_CREATIVE_ID_CAPACITY)
        return UMI_STATUS_PARSE_ERROR;
    *cursor += 16U;
    if (id_size > end - *cursor)
        return UMI_STATUS_PARSE_ERROR;
    char id[UMI_CREATIVE_ID_CAPACITY];
    if (memchr(wire + *cursor, 0, id_size) != NULL)
        return UMI_STATUS_PARSE_ERROR;
    memcpy(id, wire + *cursor, id_size);
    id[id_size] = '\0';
    if (!UmiCreativeIdValid(id))
        return UMI_STATUS_PARSE_ERROR;
    *cursor += id_size;
    if (archive_size > (uint64_t)(end - *cursor))
        return UMI_STATUS_PARSE_ERROR;
    UmiCreativeAsset *asset = NULL;
    UmiStatus status = UmiCreativeAssetArchiveDecodeBounded(
        wire + *cursor, (size_t)archive_size, maximum_bytes - library->info.byte_count, cancel, &asset);
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetLibraryInsert(library, id, &asset);
    UmiCreativeAssetDestroy(asset);
    if (status == UMI_STATUS_ALREADY_EXISTS || status == UMI_STATUS_INVALID_ARGUMENT)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        *cursor += (size_t)archive_size;
    return status;
}
/* Parsing builds a temporary library. Only the final assignment publishes it,
 * so duplicate identities, a damaged last entry or cancellation cannot expose
 * the valid prefix of an otherwise invalid collection to an application. */
UmiStatus UmiCreativeAssetLibraryArchiveDecode(const void *bytes, size_t size, size_t maximum_bytes,
                                               const UmiCancellationToken *cancel,
                                               UmiCreativeAssetLibrary **out)
{
    if (bytes == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL)
        return UMI_STATUS_INVALID_STATE;
    if (maximum_bytes > UMI_CREATIVE_LIBRARY_MAX_BYTES || size > UMI_CREATIVE_LIBRARY_ARCHIVE_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (size < 65U)
        return UMI_STATUS_PARSE_ERROR;
    const unsigned char *wire = bytes;
    if (memcmp(wire, library_magic, sizeof(library_magic)) != 0)
        return UMI_STATUS_PARSE_ERROR;
    if (Get(wire + 8U, 2U) != 1U)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (Get(wire + 10U, 2U) != 0U || Get(wire + 18U, 6U) != 0U)
        return UMI_STATUS_PARSE_ERROR;
    uint64_t count = Get(wire + 12U, 4U), declared = Get(wire + 24U, 8U);
    size_t title_size = (size_t)Get(wire + 16U, 2U), end = size - UMI_NATIVE_SHA256_BYTES;
    if (count > UMI_CREATIVE_LIBRARY_MAX_ASSETS || declared > maximum_bytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (title_size == 0U || title_size >= UMI_CREATIVE_LABEL_CAPACITY || title_size > end - 32U)
        return UMI_STATUS_PARSE_ERROR;
    char title[UMI_CREATIVE_LABEL_CAPACITY];
    if (memchr(wire + 32U, 0, title_size) != NULL)
        return UMI_STATUS_PARSE_ERROR;
    memcpy(title, wire + 32U, title_size);
    title[title_size] = '\0';
    if (!UmiCreativeTextValid(title, sizeof(title), false))
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = ValidateDigest(wire, end, cancel);
    if (status != UMI_STATUS_OK)
        return status;
    UmiCreativeAssetLibrary *library = NULL;
    status = UmiCreativeAssetLibraryCreate(title, &library);
    if (status != UMI_STATUS_OK)
        return status;
    size_t cursor = 32U + title_size;
    for (uint64_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
        status = ReadRecord(wire, end, &cursor, (size_t)declared, cancel, library);
    if (status == UMI_STATUS_OK && (cursor != end || library->info.byte_count != (size_t)declared))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        UmiCreativeAssetLibraryDestroy(library);
        return status;
    }
    *out = library;
    return UMI_STATUS_OK;
}
