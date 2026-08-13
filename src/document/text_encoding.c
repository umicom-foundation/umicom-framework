/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/text_encoding.c
 *
 * PURPOSE:
 *   Implement strict Unicode scalar validation and bounded UTF-8/UTF-16 conversion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/text_encoding.h"

#include <stdlib.h>
#include <string.h>

static int decode_utf8_scalar(const unsigned char *bytes,
                              size_t count,
                              size_t *out_used,
                              uint32_t *out_scalar)
{
    unsigned char first;
    uint32_t scalar;
    size_t used;
    size_t index;
    if (bytes == NULL || count == 0U || out_used == NULL || out_scalar == NULL) return 0;
    first = bytes[0];
    if (first < 0x80U) {
        *out_used = 1U;
        *out_scalar = first;
        return 1;
    }
    if (first >= 0xC2U && first <= 0xDFU) {
        used = 2U;
        scalar = (uint32_t)(first & 0x1FU);
    } else if (first >= 0xE0U && first <= 0xEFU) {
        used = 3U;
        scalar = (uint32_t)(first & 0x0FU);
    } else if (first >= 0xF0U && first <= 0xF4U) {
        used = 4U;
        scalar = (uint32_t)(first & 0x07U);
    } else {
        return 0;
    }
    if (used > count) return 0;
    for (index = 1U; index < used; ++index) {
        if ((bytes[index] & 0xC0U) != 0x80U) return 0;
        scalar = (scalar << 6U) | (uint32_t)(bytes[index] & 0x3FU);
    }
    if ((used == 3U && scalar < 0x800U) ||
        (used == 4U && scalar < 0x10000U) ||
        scalar > 0x10FFFFU ||
        (scalar >= 0xD800U && scalar <= 0xDFFFU)) {
        return 0;
    }
    *out_used = used;
    *out_scalar = scalar;
    return 1;
}

static size_t encode_utf8_scalar(uint32_t scalar, unsigned char out[4])
{
    if (scalar < 0x80U) {
        out[0] = (unsigned char)scalar;
        return 1U;
    }
    if (scalar < 0x800U) {
        out[0] = (unsigned char)(0xC0U | (scalar >> 6U));
        out[1] = (unsigned char)(0x80U | (scalar & 0x3FU));
        return 2U;
    }
    if (scalar < 0x10000U) {
        out[0] = (unsigned char)(0xE0U | (scalar >> 12U));
        out[1] = (unsigned char)(0x80U | ((scalar >> 6U) & 0x3FU));
        out[2] = (unsigned char)(0x80U | (scalar & 0x3FU));
        return 3U;
    }
    out[0] = (unsigned char)(0xF0U | (scalar >> 18U));
    out[1] = (unsigned char)(0x80U | ((scalar >> 12U) & 0x3FU));
    out[2] = (unsigned char)(0x80U | ((scalar >> 6U) & 0x3FU));
    out[3] = (unsigned char)(0x80U | (scalar & 0x3FU));
    return 4U;
}

int umi_document_utf8_validate(const unsigned char *bytes,
                               size_t byte_count,
                               size_t *out_error_offset)
{
    size_t offset = 0U;
    if (bytes == NULL && byte_count > 0U) return 0;
    while (offset < byte_count) {
        size_t used = 0U;
        uint32_t scalar = 0U;
        if (!decode_utf8_scalar(bytes + offset, byte_count - offset, &used, &scalar)) {
            if (out_error_offset != NULL) *out_error_offset = offset;
            return 0;
        }
        offset += used;
    }
    if (out_error_offset != NULL) *out_error_offset = byte_count;
    return 1;
}

UmiDocumentTextEncoding umi_document_encoding_detect(
    const unsigned char *bytes,
    size_t byte_count,
    int *out_had_bom,
    int *out_binary)
{
    size_t index;
    int binary = 0;
    if (out_had_bom != NULL) *out_had_bom = 0;
    if (out_binary != NULL) *out_binary = 0;
    if (bytes == NULL && byte_count > 0U) return UMI_DOCUMENT_ENCODING_UNKNOWN;
    if (byte_count >= 3U && bytes[0] == 0xEFU && bytes[1] == 0xBBU && bytes[2] == 0xBFU) {
        if (out_had_bom != NULL) *out_had_bom = 1;
        return UMI_DOCUMENT_ENCODING_UTF8_BOM;
    }
    if (byte_count >= 2U && bytes[0] == 0xFFU && bytes[1] == 0xFEU) {
        if (out_had_bom != NULL) *out_had_bom = 1;
        return UMI_DOCUMENT_ENCODING_UTF16_LE;
    }
    if (byte_count >= 2U && bytes[0] == 0xFEU && bytes[1] == 0xFFU) {
        if (out_had_bom != NULL) *out_had_bom = 1;
        return UMI_DOCUMENT_ENCODING_UTF16_BE;
    }
    for (index = 0U; index < byte_count; ++index) {
        if (bytes[index] == 0U) {
            binary = 1;
            break;
        }
    }
    if (out_binary != NULL) *out_binary = binary;
    if (binary) return UMI_DOCUMENT_ENCODING_BINARY;
    return umi_document_utf8_validate(bytes, byte_count, NULL)
        ? UMI_DOCUMENT_ENCODING_UTF8 : UMI_DOCUMENT_ENCODING_UNKNOWN;
}

static uint16_t read_u16(const unsigned char *bytes, int big_endian)
{
    return big_endian
        ? (uint16_t)(((uint16_t)bytes[0] << 8U) | bytes[1])
        : (uint16_t)(((uint16_t)bytes[1] << 8U) | bytes[0]);
}

static UmiStatus decode_utf16(const unsigned char *bytes,
                              size_t count,
                              int big_endian,
                              char **out_text,
                              size_t *out_length)
{
    char *result;
    size_t input = 0U;
    size_t output = 0U;
    if ((count % 2U) != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    result = (char *)malloc(count * 2U + 1U);
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    while (input < count) {
        uint32_t scalar = read_u16(bytes + input, big_endian);
        unsigned char encoded[4];
        size_t written;
        input += 2U;
        if (scalar >= 0xD800U && scalar <= 0xDBFFU) {
            uint32_t low;
            if (input + 2U > count) {
                free(result);
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            low = read_u16(bytes + input, big_endian);
            if (low < 0xDC00U || low > 0xDFFFU) {
                free(result);
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            input += 2U;
            scalar = 0x10000U + ((scalar - 0xD800U) << 10U) + (low - 0xDC00U);
        } else if (scalar >= 0xDC00U && scalar <= 0xDFFFU) {
            free(result);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        written = encode_utf8_scalar(scalar, encoded);
        (void)memcpy(result + output, encoded, written);
        output += written;
    }
    result[output] = '\0';
    *out_text = result;
    *out_length = output;
    return UMI_STATUS_OK;
}

UmiStatus umi_document_decode_text(const unsigned char *bytes,
                                   size_t byte_count,
                                   UmiDocumentTextEncoding fallback,
                                   char **out_utf8,
                                   size_t *out_length,
                                   UmiDocumentTextEncoding *out_detected,
                                   int *out_had_bom)
{
    UmiDocumentTextEncoding detected;
    size_t skip = 0U;
    int had_bom = 0;
    int binary = 0;
    char *copy;
    if ((bytes == NULL && byte_count > 0U) || out_utf8 == NULL || out_length == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_utf8 = NULL;
    *out_length = 0U;
    detected = umi_document_encoding_detect(bytes, byte_count, &had_bom, &binary);
    if (binary) return UMI_STATUS_NOT_IMPLEMENTED;
    if (detected == UMI_DOCUMENT_ENCODING_UNKNOWN) detected = fallback;
    if (detected == UMI_DOCUMENT_ENCODING_UTF8_BOM) skip = 3U;
    if (detected == UMI_DOCUMENT_ENCODING_UTF16_LE ||
        detected == UMI_DOCUMENT_ENCODING_UTF16_BE) {
        if (had_bom) skip = 2U;
        if (byte_count < skip) return UMI_STATUS_INVALID_ARGUMENT;
        if (out_detected != NULL) *out_detected = detected;
        if (out_had_bom != NULL) *out_had_bom = had_bom;
        return decode_utf16(bytes + skip, byte_count - skip,
                            detected == UMI_DOCUMENT_ENCODING_UTF16_BE,
                            out_utf8, out_length);
    }
    if (detected != UMI_DOCUMENT_ENCODING_UTF8 &&
        detected != UMI_DOCUMENT_ENCODING_UTF8_BOM) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (!umi_document_utf8_validate(bytes + skip, byte_count - skip, NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    copy = (char *)malloc(byte_count - skip + 1U);
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (byte_count > skip) (void)memcpy(copy, bytes + skip, byte_count - skip);
    copy[byte_count - skip] = '\0';
    *out_utf8 = copy;
    *out_length = byte_count - skip;
    if (out_detected != NULL) *out_detected = detected;
    if (out_had_bom != NULL) *out_had_bom = had_bom;
    return UMI_STATUS_OK;
}

static UmiStatus encode_utf16(const char *utf8,
                              size_t length,
                              int big_endian,
                              int include_bom,
                              unsigned char **out_bytes,
                              size_t *out_count)
{
    unsigned char *result;
    size_t input = 0U;
    size_t output = include_bom ? 2U : 0U;
    result = (unsigned char *)malloc(length * 2U + 4U);
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (include_bom) {
        result[0] = big_endian ? 0xFEU : 0xFFU;
        result[1] = big_endian ? 0xFFU : 0xFEU;
    }
    while (input < length) {
        size_t used = 0U;
        uint32_t scalar = 0U;
        uint16_t units[2];
        size_t unit_count = 1U;
        size_t unit_index;
        if (!decode_utf8_scalar((const unsigned char *)utf8 + input,
                                length - input, &used, &scalar)) {
            free(result);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        input += used;
        if (scalar < 0x10000U) {
            units[0] = (uint16_t)scalar;
        } else {
            scalar -= 0x10000U;
            units[0] = (uint16_t)(0xD800U | (scalar >> 10U));
            units[1] = (uint16_t)(0xDC00U | (scalar & 0x3FFU));
            unit_count = 2U;
        }
        for (unit_index = 0U; unit_index < unit_count; ++unit_index) {
            result[output++] = big_endian
                ? (unsigned char)(units[unit_index] >> 8U)
                : (unsigned char)(units[unit_index] & 0xFFU);
            result[output++] = big_endian
                ? (unsigned char)(units[unit_index] & 0xFFU)
                : (unsigned char)(units[unit_index] >> 8U);
        }
    }
    *out_bytes = result;
    *out_count = output;
    return UMI_STATUS_OK;
}

UmiStatus umi_document_encode_text(const char *utf8,
                                   size_t utf8_length,
                                   UmiDocumentTextEncoding encoding,
                                   int include_bom,
                                   unsigned char **out_bytes,
                                   size_t *out_byte_count)
{
    unsigned char *result;
    size_t prefix = 0U;
    if ((utf8 == NULL && utf8_length > 0U) || out_bytes == NULL ||
        out_byte_count == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_bytes = NULL;
    *out_byte_count = 0U;
    if (encoding == UMI_DOCUMENT_ENCODING_UTF16_LE ||
        encoding == UMI_DOCUMENT_ENCODING_UTF16_BE) {
        return encode_utf16(utf8, utf8_length,
                            encoding == UMI_DOCUMENT_ENCODING_UTF16_BE,
                            include_bom, out_bytes, out_byte_count);
    }
    if (encoding != UMI_DOCUMENT_ENCODING_UTF8 &&
        encoding != UMI_DOCUMENT_ENCODING_UTF8_BOM) return UMI_STATUS_NOT_IMPLEMENTED;
    if (!umi_document_utf8_validate((const unsigned char *)utf8, utf8_length, NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (include_bom || encoding == UMI_DOCUMENT_ENCODING_UTF8_BOM) prefix = 3U;
    result = (unsigned char *)malloc(prefix + utf8_length + 1U);
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (prefix > 0U) {
        result[0] = 0xEFU;
        result[1] = 0xBBU;
        result[2] = 0xBFU;
    }
    if (utf8_length > 0U) (void)memcpy(result + prefix, utf8, utf8_length);
    result[prefix + utf8_length] = 0U;
    *out_bytes = result;
    *out_byte_count = prefix + utf8_length;
    return UMI_STATUS_OK;
}

void umi_document_encoding_free(void *memory)
{
    free(memory);
}
