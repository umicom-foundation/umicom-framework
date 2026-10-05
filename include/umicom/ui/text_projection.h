/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/text_projection.h
 * PURPOSE: Own a bounded text snapshot and map filtered rows back to their source identity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_TEXT_PROJECTION_H
#define UMICOM_UI_TEXT_PROJECTION_H
#include "umicom/ui/sort_filter_model.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_UI_TEXT_PROJECTION_ROWS 32768U
#define UMI_UI_TEXT_PROJECTION_ROW_BYTES 8192U
#define UMI_UI_TEXT_PROJECTION_BYTES (16U * 1024U * 1024U)
    typedef struct UmiUiTextProjection UmiUiTextProjection;

    /** Copy NUL-terminated text rows, including empty and duplicate labels. Identity
 * is the original zero-based row index, never its label. Inputs are borrowed
 * only during this call. Text is treated as bytes; presenters validate UTF-8.
 * Each row includes its terminator in the byte limits. Empty input is allowed.
 * A failed creation leaves *out_projection NULL. Serialize all access on the
 * owner's thread; returned text stays valid until Destroy. No I/O occurs. */
    UmiStatus UmiUiTextProjectionCreate(const char *const *rows, size_t count,
                                        UmiUiTextProjection **out_projection);
    void UmiUiTextProjectionDestroy(UmiUiTextProjection *projection);

    /** Apply the existing literal matching and ASCII case rules. Only query,
 * enabled, case_sensitive and ascending are used; sort_key and registry
 * identity are not needed. With sort=false, keep original order. Equal sort
 * keys retain original order in either direction. No numeric interpretation,
 * regular expressions, Unicode folding, callbacks or allocation occur here.
 * Invalid input leaves the last complete mapping unchanged. */
    UmiStatus UmiUiTextProjectionApply(UmiUiTextProjection *projection, const UmiUiSortFilterSnapshot *filter,
                                       bool sort);
    size_t UmiUiTextProjectionCount(const UmiUiTextProjection *projection);
    size_t UmiUiTextProjectionSourceCount(const UmiUiTextProjection *projection);
    /** Query the captured text by original source index, independent of filtering. */
    const char *UmiUiTextProjectionText(const UmiUiTextProjection *projection, size_t source_index);
    /** Failed lookups do not write the output. SourceToView returns NOT_FOUND when
 * that row is hidden. A source index belongs only to this captured snapshot;
 * callers must retire it when replacing their underlying catalogue. */
    UmiStatus UmiUiTextProjectionToSource(const UmiUiTextProjection *projection, size_t view_index,
                                          size_t *out_source_index);
    UmiStatus UmiUiTextProjectionToView(const UmiUiTextProjection *projection, size_t source_index,
                                        size_t *out_view_index);
#ifdef __cplusplus
}
#endif
#endif
