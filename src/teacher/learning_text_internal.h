/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_text_internal.h
 *
 * PURPOSE:
 *   Keep immutable learning index ownership private to the Framework implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LEARNING_TEXT_INTERNAL_H
#define UMICOM_LEARNING_TEXT_INTERNAL_H
#include "umicom/teacher/learning_text.h"
#include "umicom/editor/workspace_search_index.h"
#include "../base/utf8_text_private.h"
#include <string.h>

typedef struct LearningTextDocument {
    UmiLearningPackInfo pack;
    UmiLearningPackResource resource;
    int reading_projection;
} LearningTextDocument;
struct UmiLearningTextIndex {
    UmiLearningTextIndexInfo info;
    UmiEditorWorkspaceSearchIndex *documents;
    LearningTextDocument *metadata;
};
static inline unsigned char LearningTextFold(unsigned char c)
{
    return c >= 'A' && c <= 'Z' ? (unsigned char)(c + ('a' - 'A')) : c;
}
static inline int LearningTextEqualFold(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        if (LearningTextFold((unsigned char)*a++) != LearningTextFold((unsigned char)*b++)) return 0;
    }
    return *a == *b;
}
/* Encoding validation is shared with report writers; the reading policy adds
 * its own control-byte restriction without treating text as executable input. */
static inline int LearningTextValid(const char *text, size_t bytes)
{
    if (!UmiUtf8TextValid(text, bytes)) return 0;
    for (size_t i = 0U; i < bytes; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c == 127U || (c < 32U && c != '\t' && c != '\n' && c != '\r')) return 0;
    }
    return 1;
}
#endif
