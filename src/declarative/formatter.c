/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/declarative/formatter.c
 *
 * PURPOSE:
 *   Implement parse-and-serialise formatting so one canonical text form is used in source control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation works on the semantic .umiapp model.  It keeps parsing,
 * validation and generation independent of any particular graphical toolkit.
 */

#include "umicom/declarative/formatter.h"
#include "umicom/declarative/serializer.h"

/* Provide the decl format text operation used by this module and its client applications. */
UmiStatus umi_decl_format_text(const char *source, char *out_text, size_t capacity, UmiDeclDiagnosticList *diagnostics)
{
    UmiDeclDocument *document = NULL;
    UmiStatus status = umi_decl_parse_text(source, &document, diagnostics);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) status = umi_decl_serialize(document, out_text, capacity, NULL);
    umi_decl_document_destroy(document);
    return status;
}
