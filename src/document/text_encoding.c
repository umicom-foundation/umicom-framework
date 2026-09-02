/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/text_encoding.c
 *
 * PURPOSE:
 *   Implement strict Unicode scalar validation and bounded UTF-8/UTF-16 conversion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/text_encoding.h"

#include <stdlib.h>
#include <string.h>

/*
 * Provide the decode utf8 scalar operation used by this module and its client
 * applications.
 */
static int decode_utf8_scalar(const unsigned char *bytes,
                              size_t count,
                              size_t *out_used,
                              uint32_t *out_scalar)
{
    unsigned char first;
    uint32_t scalar;
    size_t used;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bytes == NULL || count == 0U || out_used == NULL || out_scalar == NULL) return 0;
    first = bytes[0];
    /* Apply this branch only when its contract condition is satisfied. */
    if (first < 0x80U) {
        *out_used = 1U;
        *out_scalar = first;
        return 1;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (first >= 0xC2U && first <= 0xDFU) {
        used = 2U;
        scalar = (uint32_t)(first & 0x1FU);
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (first >= 0xE0U && first <= 0xEFU) {
        used = 3U;
        scalar = (uint32_t)(first & 0x0FU);
    } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (first >= 0xF0U && first <= 0xF4U) {
        used = 4U;
        scalar = (uint32_t)(first & 0x07U);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return 0;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (used > count) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; index < used; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if ((bytes[index] & 0xC0U) != 0x80U) return 0;
        scalar = (scalar << 6U) | (uint32_t)(bytes[index] & 0x3FU);
    }
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the encode utf8 scalar operation used by this module and its client
 * applications.
 */
static size_t encode_utf8_scalar(uint32_t scalar, unsigned char out[4])
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (scalar < 0x80U) {
        out[0] = (unsigned char)scalar;
        return 1U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (scalar < 0x800U) {
        out[0] = (unsigned char)(0xC0U | (scalar >> 6U));
        out[1] = (unsigned char)(0x80U | (scalar & 0x3FU));
        return 2U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
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

/* Check that document utf8 satisfies its contract before another service relies on it. */
int umi_document_utf8_validate(const unsigned char *bytes,
                               size_t byte_count,
                               size_t *out_error_offset)
{
    size_t offset = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bytes == NULL && byte_count > 0U) return 0;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (offset < byte_count) {
        size_t used = 0U;
        uint32_t scalar = 0U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!decode_utf8_scalar(bytes + offset, byte_count - offset, &used, &scalar)) {
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (out_error_offset != NULL) *out_error_offset = offset;
            return 0;
        }
        offset += used;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_error_offset != NULL) *out_error_offset = byte_count;
    return 1;
}

/*
 * Provide the document encoding detect operation used by this module and its client
 * applications.
 */
UmiDocumentTextEncoding umi_document_encoding_detect(
    const unsigned char *bytes,
    size_t byte_count,
    int *out_had_bom,
    int *out_binary)
{
    size_t index;
    int binary = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_had_bom != NULL) *out_had_bom = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_binary != NULL) *out_binary = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bytes == NULL && byte_count > 0U) return UMI_DOCUMENT_ENCODING_UNKNOWN;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (byte_count >= 3U && bytes[0] == 0xEFU && bytes[1] == 0xBBU && bytes[2] == 0xBFU) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_had_bom != NULL) *out_had_bom = 1;
        return UMI_DOCUMENT_ENCODING_UTF8_BOM;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (byte_count >= 2U && bytes[0] == 0xFFU && bytes[1] == 0xFEU) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_had_bom != NULL) *out_had_bom = 1;
        return UMI_DOCUMENT_ENCODING_UTF16_LE;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (byte_count >= 2U && bytes[0] == 0xFEU && bytes[1] == 0xFFU) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_had_bom != NULL) *out_had_bom = 1;
        return UMI_DOCUMENT_ENCODING_UTF16_BE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < byte_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (bytes[index] == 0U) {
            binary = 1;
            break;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_binary != NULL) *out_binary = binary;
    /* Apply this branch only when its contract condition is satisfied. */
    if (binary) return UMI_DOCUMENT_ENCODING_BINARY;
    return umi_document_utf8_validate(bytes, byte_count, NULL)
        ? UMI_DOCUMENT_ENCODING_UTF8 : UMI_DOCUMENT_ENCODING_UNKNOWN;
}

/* Provide the read u16 operation used by this module and its client applications. */
static uint16_t read_u16(const unsigned char *bytes, int big_endian)
{
    return big_endian
        ? (uint16_t)(((uint16_t)bytes[0] << 8U) | bytes[1])
        : (uint16_t)(((uint16_t)bytes[1] << 8U) | bytes[0]);
}

/* Provide the decode utf16 operation used by this module and its client applications. */
static UmiStatus decode_utf16(const unsigned char *bytes,
                              size_t count,
                              int big_endian,
                              char **out_text,
                              size_t *out_length)
{
    char *result;
    size_t input = 0U;
    size_t output = 0U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((count % 2U) != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    result = (char *)malloc(count * 2U + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (input < count) {
        uint32_t scalar = read_u16(bytes + input, big_endian);
        unsigned char encoded[4];
        size_t written;
        input += 2U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (scalar >= 0xD800U && scalar <= 0xDBFFU) {
            uint32_t low;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (input + 2U > count) {
                free(result);
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            low = read_u16(bytes + input, big_endian);
            /* Apply this branch only when its contract condition is satisfied. */
            if (low < 0xDC00U || low > 0xDFFFU) {
                free(result);
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            input += 2U;
            scalar = 0x10000U + ((scalar - 0xD800U) << 10U) + (low - 0xDC00U);
        } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (scalar >= 0xDC00U && scalar <= 0xDFFFU) {
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

/*
 * Provide the document decode text operation used by this module and its client
 * applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if ((bytes == NULL && byte_count > 0U) || out_utf8 == NULL || out_length == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_utf8 = NULL;
    *out_length = 0U;
    detected = umi_document_encoding_detect(bytes, byte_count, &had_bom, &binary);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (binary) return UMI_STATUS_NOT_IMPLEMENTED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (detected == UMI_DOCUMENT_ENCODING_UNKNOWN) detected = fallback;
    /* Apply this branch only when its contract condition is satisfied. */
    if (detected == UMI_DOCUMENT_ENCODING_UTF8_BOM) skip = 3U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (detected == UMI_DOCUMENT_ENCODING_UTF16_LE ||
        detected == UMI_DOCUMENT_ENCODING_UTF16_BE) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (had_bom) skip = 2U;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (byte_count < skip) return UMI_STATUS_INVALID_ARGUMENT;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_detected != NULL) *out_detected = detected;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_had_bom != NULL) *out_had_bom = had_bom;
        return decode_utf16(bytes + skip, byte_count - skip,
                            detected == UMI_DOCUMENT_ENCODING_UTF16_BE,
                            out_utf8, out_length);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (detected != UMI_DOCUMENT_ENCODING_UTF8 &&
        detected != UMI_DOCUMENT_ENCODING_UTF8_BOM) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_document_utf8_validate(bytes + skip, byte_count - skip, NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    copy = (char *)malloc(byte_count - skip + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (byte_count > skip) (void)memcpy(copy, bytes + skip, byte_count - skip);
    copy[byte_count - skip] = '\0';
    *out_utf8 = copy;
    *out_length = byte_count - skip;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_detected != NULL) *out_detected = detected;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_had_bom != NULL) *out_had_bom = had_bom;
    return UMI_STATUS_OK;
}

/* Provide the encode utf16 operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (include_bom) {
        result[0] = big_endian ? 0xFEU : 0xFFU;
        result[1] = big_endian ? 0xFFU : 0xFEU;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (input < length) {
        size_t used = 0U;
        uint32_t scalar = 0U;
        uint16_t units[2];
        size_t unit_count = 1U;
        size_t unit_index;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!decode_utf8_scalar((const unsigned char *)utf8 + input,
                                length - input, &used, &scalar)) {
            free(result);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        input += used;
        /* Apply this branch only when its contract condition is satisfied. */
        if (scalar < 0x10000U) {
            units[0] = (uint16_t)scalar;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            scalar -= 0x10000U;
            units[0] = (uint16_t)(0xD800U | (scalar >> 10U));
            units[1] = (uint16_t)(0xDC00U | (scalar & 0x3FFU));
            unit_count = 2U;
        }
        /* Visit each bounded item once so every record receives the same rule. */
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

/*
 * Provide the document encode text operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_encode_text(const char *utf8,
                                   size_t utf8_length,
                                   UmiDocumentTextEncoding encoding,
                                   int include_bom,
                                   unsigned char **out_bytes,
                                   size_t *out_byte_count)
{
    unsigned char *result;
    size_t prefix = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if ((utf8 == NULL && utf8_length > 0U) || out_bytes == NULL ||
        out_byte_count == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_bytes = NULL;
    *out_byte_count = 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (encoding == UMI_DOCUMENT_ENCODING_UTF16_LE ||
        encoding == UMI_DOCUMENT_ENCODING_UTF16_BE) {
        return encode_utf16(utf8, utf8_length,
                            encoding == UMI_DOCUMENT_ENCODING_UTF16_BE,
                            include_bom, out_bytes, out_byte_count);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (encoding != UMI_DOCUMENT_ENCODING_UTF8 &&
        encoding != UMI_DOCUMENT_ENCODING_UTF8_BOM) return UMI_STATUS_NOT_IMPLEMENTED;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_document_utf8_validate((const unsigned char *)utf8, utf8_length, NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (include_bom || encoding == UMI_DOCUMENT_ENCODING_UTF8_BOM) prefix = 3U;
    result = (unsigned char *)malloc(prefix + utf8_length + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (prefix > 0U) {
        result[0] = 0xEFU;
        result[1] = 0xBBU;
        result[2] = 0xBFU;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (utf8_length > 0U) (void)memcpy(result + prefix, utf8, utf8_length);
    result[prefix + utf8_length] = 0U;
    *out_bytes = result;
    *out_byte_count = prefix + utf8_length;
    return UMI_STATUS_OK;
}

/*
 * Provide the document encoding free operation used by this module and its client
 * applications.
 */
void umi_document_encoding_free(void *memory)
{
    free(memory);
}
