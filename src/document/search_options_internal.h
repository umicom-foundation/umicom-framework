/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/search_options_internal.h
 * PURPOSE: Canonicalize literal-search policy once for navigation and replacement owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_SEARCH_OPTIONS_INTERNAL_H
#define UMICOM_DOCUMENT_SEARCH_OPTIONS_INTERNAL_H
#include "umicom/editor/search_engine.h"
/* Documents use complete, non-overlapping replacement and unbounded match
 * navigation. Only case and identifier-boundary policy comes from the caller;
 * presentation result limits must never silently limit a replacement. */
static UmiStatus DocumentSearchOptions(const UmiEditorSearchOptions *requested,
                                       UmiEditorSearchOptions *outOptions)
{
    UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SMART, 0, 0, 0U};
    if (requested != NULL)
    {
        if (requested->case_mode < UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
            requested->case_mode > UMI_EDITOR_SEARCH_CASE_SMART ||
            (requested->whole_word != 0 && requested->whole_word != 1))
            return UMI_STATUS_INVALID_ARGUMENT;
        options.case_mode = requested->case_mode;
        options.whole_word = requested->whole_word;
    }
    *outOptions = options;
    return UMI_STATUS_OK;
}
#endif
