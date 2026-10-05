/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/recovery_draft.c
 * PURPOSE: Frame private recovery source with strict lengths, stable identity and validated selection ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/recovery_draft.h"
#include "umicom/document/text_encoding.h"
#include "umicom/editor/text_position.h"
#include "umicom/platform/secure_random.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiDocumentRecoveryDraft
{
    UmiDocumentRecoveryInfo info;
    char *text;
};
static int RecoveryKey(const char *key)
{
    for (size_t i = 0U; i < 32U; ++i)
        if (!((key[i] >= '0' && key[i] <= '9') || (key[i] >= 'a' && key[i] <= 'f')))
            return 0;
    return key[32] == '\0';
}
static int RecoveryText(const char *text, size_t capacity, int required)
{
    const char *end = memchr(text, '\0', capacity);
    if (end == NULL || (required && end == text))
        return 0;
    size_t size = (size_t)(end - text);
    if (!umi_document_utf8_validate((const unsigned char *)text, size, NULL))
        return 0;
    for (size_t i = 0U; i < size; ++i)
        if ((unsigned char)text[i] < 0x20U || (unsigned char)text[i] == 0x7fU)
            return 0;
    return 1;
}
UmiStatus UmiDocumentRecoveryKeyCreate(char *out, size_t capacity)
{
    if (out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    out[0] = '\0';
    if (capacity < UMI_DOCUMENT_RECOVERY_KEY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    unsigned char entropy[16];
    UmiStatus status = UmiSecureRandomBytes(entropy, sizeof(entropy));
    if (status != UMI_STATUS_OK)
        return status;
    static const char digits[] = "0123456789abcdef";
    for (size_t i = 0U; i < sizeof(entropy); ++i)
    {
        out[i * 2U] = digits[entropy[i] >> 4U];
        out[i * 2U + 1U] = digits[entropy[i] & 15U];
    }
    out[32] = '\0';
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentRecoveryDraftCreate(const UmiDocumentRecoveryInfo *info, const char *text, size_t bytes,
                                         const UmiCancellationToken *cancel, UmiDocumentRecoveryDraft **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (info == NULL || text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (bytes > UMI_DOCUMENT_RECOVERY_TEXT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (info->text_bytes != bytes || !RecoveryKey(info->key) ||
        !RecoveryText(info->display_name, sizeof(info->display_name), 1) ||
        !RecoveryText(info->source_path, sizeof(info->source_path), 0) ||
        !RecoveryText(info->language_id, sizeof(info->language_id), 0) || memchr(text, '\0', bytes) != NULL ||
        info->cursor_offset > bytes || info->selection_bytes > bytes - info->cursor_offset)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = text;
    view.byte_count = view.capacity = bytes;
    UmiEditorTextPosition position;
    UmiStatus status = UmiEditorTextViewPositionAt(&view, bytes, &position);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, info->cursor_offset, &position);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, info->cursor_offset + info->selection_bytes, &position);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiDocumentRecoveryDraft *draft = calloc(1U, sizeof(*draft));
    if (draft == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    draft->text = malloc(bytes + 1U);
    if (draft->text == NULL)
    {
        free(draft);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(draft->text, text, bytes);
    draft->text[bytes] = '\0';
    draft->info = *info;
    if (umi_cancellation_token_is_requested(cancel))
    {
        UmiDocumentRecoveryDraftDestroy(draft);
        return UMI_STATUS_CANCELLED;
    }
    *out = draft;
    return UMI_STATUS_OK;
}
void UmiDocumentRecoveryDraftDestroy(UmiDocumentRecoveryDraft *draft)
{
    if (draft != NULL)
    {
        free(draft->text);
        free(draft);
    }
}
UmiStatus UmiDocumentRecoveryDraftInspect(const UmiDocumentRecoveryDraft *draft, UmiDocumentRecoveryInfo *out)
{
    if (draft == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = draft->info;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentRecoveryDraftRead(const UmiDocumentRecoveryDraft *draft, const char **out, size_t *bytes)
{
    if (out != NULL)
        *out = NULL;
    if (bytes != NULL)
        *bytes = 0U;
    if (draft == NULL || out == NULL || bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = draft->text;
    *bytes = draft->info.text_bytes;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentRecoveryDraftEncode(const UmiDocumentRecoveryDraft *draft, unsigned char **out,
                                         size_t *size)
{
    if (out != NULL)
        *out = NULL;
    if (size != NULL)
        *size = 0U;
    if (draft == NULL || out == NULL || size == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiDocumentRecoveryInfo *info = &draft->info;
    size_t name = strlen(info->display_name), path = strlen(info->source_path),
           language = strlen(info->language_id);
    char header[512];
    int written = snprintf(
        header, sizeof(header),
        "UMICOM-DRAFT\nkey=%s\nrevision=%" PRIu64
        "\ncursor=%zu\nselection=%zu\nname-bytes=%zu\npath-bytes=%zu\nlanguage-bytes=%zu\ntext-bytes=%zu\n\n",
        info->key, info->source_revision, info->cursor_offset, info->selection_bytes, name, path, language,
        info->text_bytes);
    if (written < 0 || (size_t)written >= sizeof(header))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t total = (size_t)written + name + path + language + info->text_bytes;
    unsigned char *bytes = malloc(total);
    if (bytes == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t offset = (size_t)written;
    memcpy(bytes, header, offset);
    memcpy(bytes + offset, info->display_name, name);
    offset += name;
    memcpy(bytes + offset, info->source_path, path);
    offset += path;
    memcpy(bytes + offset, info->language_id, language);
    offset += language;
    memcpy(bytes + offset, draft->text, info->text_bytes);
    *out = bytes;
    *size = total;
    return UMI_STATUS_OK;
}
static UmiStatus RecoveryLine(const unsigned char *bytes, size_t size, size_t *offset, char *line,
                              size_t capacity)
{
    size_t start = *offset, end = start;
    while (end < size && bytes[end] != '\n')
    {
        if (bytes[end] == '\0' || end - start + 1U >= capacity)
            return UMI_STATUS_PARSE_ERROR;
        ++end;
    }
    if (end == size)
        return UMI_STATUS_PARSE_ERROR;
    memcpy(line, bytes + start, end - start);
    line[end - start] = '\0';
    *offset = end + 1U;
    return UMI_STATUS_OK;
}
static UmiStatus RecoveryNumber(const char *line, const char *prefix, uint64_t *out)
{
    size_t count = strlen(prefix);
    if (strncmp(line, prefix, count) != 0)
        return UMI_STATUS_PARSE_ERROR;
    const char *digits = line + count;
    if (*digits == '\0' || (digits[0] == '0' && digits[1] != '\0'))
        return UMI_STATUS_PARSE_ERROR;
    uint64_t value = 0U;
    for (const char *at = digits; *at != '\0'; ++at)
    {
        if (*at < '0' || *at > '9')
            return UMI_STATUS_PARSE_ERROR;
        uint64_t digit = (uint64_t)(*at - '0');
        if (value > (UINT64_MAX - digit) / 10U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        value = value * 10U + digit;
    }
    *out = value;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentRecoveryDraftDecode(const void *input, size_t size, const UmiCancellationToken *cancel,
                                         UmiDocumentRecoveryDraft **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (input == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (size > UMI_DOCUMENT_RECOVERY_RECORD_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    const unsigned char *bytes = input;
    char line[128];
    size_t offset = 0U;
    UmiDocumentRecoveryInfo info = {0};
    UmiStatus status = RecoveryLine(bytes, size, &offset, line, sizeof(line));
    if (status == UMI_STATUS_OK && strcmp(line, "UMICOM-DRAFT") != 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = RecoveryLine(bytes, size, &offset, line, sizeof(line));
    if (status == UMI_STATUS_OK && (strncmp(line, "key=", 4U) != 0 || strlen(line + 4U) != 32U))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        memcpy(info.key, line + 4U, sizeof(info.key));
    const char *prefixes[] = {
        "revision=", "cursor=", "selection=", "name-bytes=", "path-bytes=", "language-bytes=", "text-bytes="};
    uint64_t fields[7] = {0};
    for (size_t i = 0U; status == UMI_STATUS_OK && i < 7U; ++i)
    {
        status = RecoveryLine(bytes, size, &offset, line, sizeof(line));
        if (status == UMI_STATUS_OK)
            status = RecoveryNumber(line, prefixes[i], &fields[i]);
    }
    if (status == UMI_STATUS_OK)
        status = RecoveryLine(bytes, size, &offset, line, sizeof(line));
    if (status == UMI_STATUS_OK && line[0] != '\0')
        status = UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK)
        return status;
    /* Narrow hosts must reject offsets before converting to size_t. */
#if SIZE_MAX < UINT64_MAX
    if (fields[1] > SIZE_MAX || fields[2] > SIZE_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
#endif
    if (fields[3] >= sizeof(info.display_name) ||
        fields[4] >= sizeof(info.source_path) || fields[5] >= sizeof(info.language_id) ||
        fields[6] > UMI_DOCUMENT_RECOVERY_TEXT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    info.source_revision = fields[0];
    info.cursor_offset = (size_t)fields[1];
    info.selection_bytes = (size_t)fields[2];
    info.text_bytes = (size_t)fields[6];
    char *destinations[] = {info.display_name, info.source_path, info.language_id};
    for (size_t i = 0U; i < 3U; ++i)
    {
        size_t count = (size_t)fields[i + 3U];
        if (count > size - offset || memchr(bytes + offset, '\0', count) != NULL)
            return UMI_STATUS_PARSE_ERROR;
        memcpy(destinations[i], bytes + offset, count);
        destinations[i][count] = '\0';
        offset += count;
    }
    if (info.text_bytes != size - offset)
        return UMI_STATUS_PARSE_ERROR;
    return UmiDocumentRecoveryDraftCreate(&info, (const char *)bytes + offset, info.text_bytes, cancel, out);
}
void UmiDocumentRecoveryBytesFree(void *bytes) { free(bytes); }
