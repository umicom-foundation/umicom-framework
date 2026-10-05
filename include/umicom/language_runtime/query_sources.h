/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/query_sources.h
 * PURPOSE: Describe additional captured drafts synchronized for one temporary language query.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_QUERY_SOURCES_H
#define UMICOM_LANGUAGE_RUNTIME_QUERY_SOURCES_H
#include <stddef.h>
#define UMI_LANGUAGE_SOURCE_QUERY_MAXIMUM_DOCUMENTS 64U
/* The caller owns these complete UTF-8 bytes until the query returns. Each URI
 * must identify a distinct document, different from the primary request URI.
 * Select documents understood by the chosen language server. Sending them
 * shares their unsaved text with that process; no disk Save is performed.
 * All drafts start at protocol version 1 in the new, exclusively owned session.
 * This is input data, not a persistent editor or language-server subscription. */
typedef struct UmiLanguageQuerySource
{
    const char *document_uri;
    const char *language_id;
    const char *source;
    size_t source_bytes;
} UmiLanguageQuerySource;
#endif
