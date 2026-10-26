/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/course_catalogue.c
 * PURPOSE:
 *   Publish safe, deterministic, accessible educational course navigation
 *   using an allocation-free, two-pass C23 HTML writer.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/course_catalogue.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* The first pass only counts bytes. The second pass writes after confirming
 * the caller's complete capacity, making rejected renders failure-atomic. */
typedef struct CatalogueSink {
    char *buffer;
    size_t capacity;
    size_t length;
    bool overflow;
} CatalogueSink;

static void CataloguePut(CatalogueSink *sink, char value)
{
    if (sink->length >= SIZE_MAX - 1U) {
        sink->overflow = true;
        return;
    }
    if (sink->buffer != NULL) {
        if (sink->length >= sink->capacity - 1U) {
            sink->overflow = true;
            return;
        }
        sink->buffer[sink->length] = value;
    }
    ++sink->length;
}

static void CatalogueString(CatalogueSink *sink, const char *text)
{
    for (const char *it = text; *it != '\0' && !sink->overflow; ++it)
        CataloguePut(sink, *it);
}

static void CatalogueEscape(CatalogueSink *sink, const char *text)
{
    for (const char *it = text; *it != '\0' && !sink->overflow; ++it) {
        switch (*it) {
        case '&': CatalogueString(sink, "&amp;"); break;
        case '<': CatalogueString(sink, "&lt;"); break;
        case '>': CatalogueString(sink, "&gt;"); break;
        case '\"': CatalogueString(sink, "&quot;"); break;
        case '\'': CatalogueString(sink, "&#39;"); break;
        default: CataloguePut(sink, *it); break;
        }
    }
}

static int CatalogueTitleValid(const char *value, size_t maxLength)
{
    size_t length = 0U;
    if (value == NULL || value[0] == '\0') return 0;
    for (const unsigned char *p = (const unsigned char *)value;
         *p != 0U; ++p) {
        if (++length > maxLength || *p < 0x20U || *p == 0x7fU)
            return 0;
    }
    return 1;
}

static int CatalogueHrefValid(const char *href)
{
    size_t length = 0U, componentStart = 0U;
    if (href == NULL || href[0] == '\0' || href[0] == '/') return 0;
    for (const unsigned char *p = (const unsigned char *)href;
         *p != 0U; ++p) {
        const unsigned char c = *p;
        if (++length > UMI_EDUCATION_COURSE_MAX_HREF) return 0;
        if (c == '/') {
            const size_t component = (size_t)(p - (const unsigned char *)href) - componentStart;
            if (component == 0U ||
                (component == 1U && href[componentStart] == '.') ||
                (component == 2U && href[componentStart] == '.' &&
                 href[componentStart + 1U] == '.')) return 0;
            componentStart = (size_t)(p - (const unsigned char *)href) + 1U;
        } else if (!((c >= 'a' && c <= 'z') ||
                     (c >= 'A' && c <= 'Z') ||
                     (c >= '0' && c <= '9') ||
                     c == '-' || c == '_' || c == '.')) {
            return 0;
        }
    }
    const size_t tail = length - componentStart;
    if (tail < 6U) return 0; /* a.html is the shortest permitted file */
    if ((tail == 1U && href[componentStart] == '.') ||
        (tail == 2U && href[componentStart] == '.' &&
         href[componentStart + 1U] == '.')) return 0;
    return strcmp(href + length - 5U, ".html") == 0;
}

static void CatalogueNumber(CatalogueSink *sink, uint32_t number)
{
    char buffer[16];
    const int written = snprintf(buffer, sizeof(buffer), "%u", (unsigned)number);
    if (written < 0 || (size_t)written >= sizeof(buffer)) {
        sink->overflow = true;
        return;
    }
    CatalogueString(sink, buffer);
}

static void CatalogueCompose(CatalogueSink *sink, const char *title,
                             const UmiEducationCourseEntry *entries,
                             size_t count, const char *home)
{
    CatalogueString(sink,
        "<!doctype html>\n<html lang=\"en-GB\"><head><meta charset=\"utf-8\">"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
        "<title>");
    CatalogueEscape(sink, title);
    CatalogueString(sink,
        " · Bits2Banking</title><style>body{font:17px/1.65 system-ui,Arial,sans-serif;"
        "color:#173247;background:#f5f8fb;margin:0}header{background:#173247;"
        "color:#fff;padding:2rem max(1rem,7vw)}main{max-width:70rem;margin:1rem auto;"
        "padding:1.5rem;background:white;border-radius:.6rem}a{color:#075eaf}"
        "li{padding:.25rem}li::marker{font-weight:700}nav{padding:.6rem;"
        "background:#eef5fb}footer{max-width:70rem;margin:2rem auto;padding:1rem}"
        "a:focus-visible{outline:3px solid #007d8a}</style></head><body>\n"
        "<header><h1>");
    CatalogueEscape(sink, title);
    CatalogueString(sink,
        "</h1><p>Sammy Hegab · Umicom Foundation · "
        "Service to Many Leads to Greatness.</p></header>\n"
        "<main id=\"main\"><nav aria-label=\"Course navigation\"><a href=\"");
    CatalogueString(sink, home);
    CatalogueString(sink, "\">Course home</a></nav>\n<ol>\n");
    for (size_t index = 0U; index < count && !sink->overflow; ++index) {
        CatalogueString(sink, "<li value=\"");
        CatalogueNumber(sink, entries[index].lessonNumber);
        CatalogueString(sink, "\"><a href=\"");
        CatalogueString(sink, entries[index].href);
        CatalogueString(sink, "\">");
        CatalogueEscape(sink, entries[index].title);
        CatalogueString(sink, "</a></li>\n");
    }
    CatalogueString(sink, "</ol></main>\n<footer>Accessible native computing "
                    "and AI education. Reading material is not a model-training approval."
                    "</footer></body></html>\n");
}

UmiStatus UmiEducationCourseCatalogueRender(
    const char *courseTitle, const UmiEducationCourseEntry *entries,
    size_t entryCount, const char *courseHomeHref,
    char *outHtml, size_t capacity, size_t *outLength)
{
    uint32_t previous = 0U;
    CatalogueSink measured = {0};
    CatalogueSink written = {0};
    if (outHtml == NULL || outLength == NULL ||
        entries == NULL || entryCount == 0U ||
        entryCount > UMI_EDUCATION_COURSE_MAX_ENTRIES ||
        !CatalogueTitleValid(courseTitle, UMI_EDUCATION_COURSE_MAX_TITLE) ||
        !CatalogueHrefValid(courseHomeHref))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < entryCount; ++i) {
        if (entries[i].lessonNumber == 0U ||
            entries[i].lessonNumber <= previous ||
            !CatalogueTitleValid(entries[i].title,
                                 UMI_EDUCATION_COURSE_MAX_TITLE) ||
            !CatalogueHrefValid(entries[i].href))
            return UMI_STATUS_INVALID_ARGUMENT;
        previous = entries[i].lessonNumber;
    }
    CatalogueCompose(&measured, courseTitle, entries, entryCount,
                     courseHomeHref);
    if (measured.overflow) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (measured.length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Once validated, writing cannot fail if all disjoint inputs remain
     * stable and the previously measured capacity is available. */
    written = (CatalogueSink){.buffer=outHtml, .capacity=capacity};
    CatalogueCompose(&written, courseTitle, entries, entryCount,
                     courseHomeHref);
    if (written.overflow || written.length != measured.length)
        return UMI_STATUS_INTERNAL_ERROR;
    outHtml[written.length] = '\0';
    *outLength = written.length;
    return UMI_STATUS_OK;
}
