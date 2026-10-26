/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/course_home_link.c
 * PURPOSE: Non-destructively integrate an optional course continuation link
 *          into the complete original HTML document.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/course_home_link.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static int ValidHref(const char *value, size_t *outLength)
{
    size_t length = 0U, partStart = 0U;
    if (!value || !value[0]) return 0;
    while (value[length]) {
        unsigned char c = (unsigned char)value[length];
        if (++length > UMI_EDUCATION_HOME_MAX_HREF_BYTES) return 0;
        if (c == '/') {
            size_t part = length - 1U - partStart;
            if (part == 0U || (part == 1U && value[partStart] == '.') ||
                (part == 2U && value[partStart] == '.' && value[partStart + 1U] == '.'))
                return 0;
            partStart = length;
        } else if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                     (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.'))
            return 0;
    }
    if (length < 6U || length - partStart < 6U ||
        strcmp(value + length - 5U, ".html") != 0) return 0;
    *outLength = length;
    return 1;
}

static int ValidLabel(const char *label, size_t *length, size_t *escaped)
{
    size_t l = 0U, escapedCount = 0U;
    if (label == NULL || label[0] == '\0') return 0;
    while (label[l] != '\0') {
        unsigned char c = (unsigned char)label[l];
        if (++l > UMI_EDUCATION_HOME_MAX_LABEL_BYTES || c < 0x20U || c == 0x7fU)
            return 0;
        size_t addition = c == '&' ? 5U : c == '<' || c == '>' ? 4U :
                          c == '\'' ? 5U : c == '"' ? 6U : 1U;
        if (escapedCount > SIZE_MAX - addition) return 0;
        escapedCount += addition;
    }
    *length = l;
    *escaped = escapedCount;
    return 1;
}

static void EscapeLabel(char **cursor, const char *label)
{
    for (const char *p = label; *p != '\0'; ++p) {
        const char *entity = *p == '&' ? "&amp;" : *p == '<' ? "&lt;" :
                             *p == '>' ? "&gt;" : *p == '\'' ? "&#39;" :
                             *p == '"' ? "&quot;" : NULL;
        if (entity) {
            size_t len = strlen(entity);
            memcpy(*cursor, entity, len);
            *cursor += len;
        } else {
            *(*cursor)++ = *p;
        }
    }
}

static const char *FindBytes(const char *text, size_t size,
                              const char *needle, size_t needleSize)
{
    if (needleSize > size) return NULL;
    for (size_t index = 0U; index <= size - needleSize; ++index)
        if (memcmp(text + index, needle, needleSize) == 0) return text + index;
    return NULL;
}

UmiStatus UmiEducationCourseHomeLinkCompose(
    const char *original, size_t originalLength,
    const char *relativeHref, const char *linkLabel,
    char *output, size_t capacity, size_t *outWritten)
{
    static const char start[] = "\n<nav class=\"umicom-course-continuation\" "
        "aria-label=\"Continue the complete course\" style=\"background:#eaf7f6;"
        "border-left:4px solid #168789;padding:1rem;margin:1rem 0\">"
        "<strong>Continue learning:</strong> <a href=\"";
    static const char middle[] = "\">";
    static const char finish[] = "</a></nav>\n";
    const char *mainStart, *mainEnd;
    size_t hrefLength = 0U, labelLength = 0U, escapedLength = 0U;
    char linkNeedle[UMI_EDUCATION_HOME_MAX_HREF_BYTES + 8U];
    size_t originalPrefix, inserted, needed;

    if (!original || !output || !outWritten || originalLength == 0U ||
        originalLength > UMI_EDUCATION_HOME_MAX_SOURCE_BYTES ||
        memchr(original, 0, originalLength) != NULL ||
        !ValidHref(relativeHref, &hrefLength) ||
        !ValidLabel(linkLabel, &labelLength, &escapedLength))
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)labelLength;
    /* One existing <main> opening tag is the only structural assumption. */
    mainStart = FindBytes(original, originalLength, "<main", 5U);
    if (mainStart == NULL) return UMI_STATUS_PARSE_ERROR;
    mainEnd = memchr(mainStart, '>', originalLength - (size_t)(mainStart - original));
    if (mainEnd == NULL || mainEnd - mainStart > 512) return UMI_STATUS_PARSE_ERROR;
    originalPrefix = (size_t)(mainEnd + 1 - original);
    if (!FindBytes(original + originalPrefix, originalLength - originalPrefix,
                   "</main>", 7U)) return UMI_STATUS_PARSE_ERROR;

    memcpy(linkNeedle, "href=\"", 6U);
    memcpy(linkNeedle + 6U, relativeHref, hrefLength);
    linkNeedle[6U + hrefLength] = '"';
    if (FindBytes(original, originalLength, linkNeedle, 7U + hrefLength)) {
        if (capacity <= originalLength) return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(output, original, originalLength);
        output[originalLength] = '\0';
        *outWritten = originalLength;
        return UMI_STATUS_OK;
    }
    inserted = sizeof(start) - 1U + hrefLength + sizeof(middle) - 1U +
               escapedLength + sizeof(finish) - 1U;
    if (inserted >= SIZE_MAX - originalLength) return UMI_STATUS_CAPACITY_EXCEEDED;
    needed = originalLength + inserted;
    if (capacity <= needed) return UMI_STATUS_CAPACITY_EXCEEDED;
    char *p = output;
    memcpy(p, original, originalPrefix); p += originalPrefix;
    memcpy(p, start, sizeof(start) - 1U); p += sizeof(start) - 1U;
    memcpy(p, relativeHref, hrefLength); p += hrefLength;
    memcpy(p, middle, sizeof(middle) - 1U); p += sizeof(middle) - 1U;
    EscapeLabel(&p, linkLabel);
    memcpy(p, finish, sizeof(finish) - 1U); p += sizeof(finish) - 1U;
    memcpy(p, original + originalPrefix, originalLength - originalPrefix);
    p += originalLength - originalPrefix;
    *p = '\0';
    *outWritten = needed;
    return UMI_STATUS_OK;
}
