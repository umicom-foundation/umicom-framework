/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/text.c
 *
 * PURPOSE:
 *   Validate bounded UTF-8, identifiers and embeddings before they enter shared AI state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workspace_internal.h"
#include <math.h>
#include <string.h>

/* Explicit byte decoding rejects overlong forms, surrogate encodings and
 * control bytes, without relying on the process locale or signed char. */
bool AwTextValid(const char *text, size_t capacity, bool allowEmpty)
{
    size_t length, index = 0U;
    const unsigned char *bytes = (const unsigned char *)text;
    if (text == NULL || capacity == 0U) return false;
    for (length = 0U; length < capacity && text[length] != '\0'; ++length) { }
    if (length == capacity) return false;
    if (length == 0U) return allowEmpty;
    while (index < length) {
        uint32_t code;
        size_t continuation;
        unsigned char first = bytes[index++];
        if (first < 0x80U) {
            if ((first < 0x20U && first != '\n' && first != '\r' && first != '\t') || first == 0x7FU)
                return false;
            continue;
        }
        if (first >= 0xC2U && first <= 0xDFU) { code = first & 0x1FU; continuation = 1U; }
        else if (first >= 0xE0U && first <= 0xEFU) { code = first & 0x0FU; continuation = 2U; }
        else if (first >= 0xF0U && first <= 0xF4U) { code = first & 0x07U; continuation = 3U; }
        else return false;
        if (continuation > length - index) return false;
        for (size_t n = 0U; n < continuation; ++n) {
            unsigned char next = bytes[index++];
            if ((next & 0xC0U) != 0x80U) return false;
            code = (code << 6U) | (uint32_t)(next & 0x3FU);
        }
        if ((continuation == 1U && code < 0x80U) ||
            (continuation == 2U && code < 0x800U) ||
            (continuation == 3U && code < 0x10000U) || code > 0x10FFFFU ||
            (code >= 0xD800U && code <= 0xDFFFU)) return false;
    }
    return true;
}

bool AwIdValid(const char *id, size_t capacity)
{
    size_t n;
    if (!AwTextValid(id, capacity, false)) return false;
    for (n = 0U; id[n] != '\0'; ++n) {
        const unsigned char c = (unsigned char)id[n];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || (n > 0U && (c == '-' || c == '_' || c == '.')))) return false;
    }
    return true;
}

UmiStatus AwTextCopy(char *out, size_t capacity, const char *text, bool allowEmpty)
{
    if (out == NULL || !AwTextValid(text, capacity, allowEmpty)) return UMI_STATUS_INVALID_ARGUMENT;
    memmove(out, text, strlen(text) + 1U);
    return UMI_STATUS_OK;
}

bool AwEmbeddingValid(const UmiAiEmbedding *embedding)
{
    double norm = 0.0;
    if (embedding == NULL || embedding->dimension == 0U ||
        embedding->dimension > UMI_AI_EMBEDDING_CAPACITY) return false;
    for (size_t i = 0U; i < embedding->dimension; ++i) {
        double value = embedding->values[i];
        if (!isfinite(value)) return false;
        norm += value * value;
    }
    return isfinite(norm) && norm > 0.0;
}
