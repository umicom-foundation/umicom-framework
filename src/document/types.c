/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/types.c
 *
 * PURPOSE:
 *   Provide stable diagnostic names for public document encoding and line-ending types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/types.h"

const char *umi_document_encoding_text(UmiDocumentTextEncoding encoding)
{
    static const char *const NAMES[] = {
        "UTF-8", "UTF-8 with BOM", "UTF-16 LE", "UTF-16 BE", "binary", "unknown"
    };
    return encoding >= UMI_DOCUMENT_ENCODING_UTF8 &&
           encoding <= UMI_DOCUMENT_ENCODING_UNKNOWN
        ? NAMES[encoding] : "invalid";
}

const char *umi_document_line_ending_text(UmiDocumentLineEnding line_ending)
{
    static const char *const NAMES[] = {"none", "LF", "CRLF", "CR", "mixed"};
    return line_ending >= UMI_DOCUMENT_LINE_ENDING_NONE &&
           line_ending <= UMI_DOCUMENT_LINE_ENDING_MIXED
        ? NAMES[line_ending] : "invalid";
}
