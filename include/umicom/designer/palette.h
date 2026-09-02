/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/palette.h
 *
 * PURPOSE:
 *   Expose component palettes and reusable fuzzy palette search derived from
 *   the declarative component registry and plug-in contributions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Palette metadata and search belong to Framework. A GTK4, web or other host
 * only decides how to present the Framework-owned results.
 */

#ifndef UMICOM_DESIGNER_PALETTE_H
#define UMICOM_DESIGNER_PALETTE_H

#include "umicom/designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the designer palette data shared with callers of this public contract.
 */
typedef struct UmiDesignerPalette {
    UmiDeclComponentDescriptor *items;
    size_t count;
} UmiDesignerPalette;

/**
 * Represent the designer palette match data shared with callers of this public contract.
 */
typedef struct UmiDesignerPaletteMatch {
    size_t palette_index;
    int score;
} UmiDesignerPaletteMatch;

/**
 * Represent the designer palette results data shared with callers of this public contract.
 */
typedef struct UmiDesignerPaletteResults {
    UmiDesignerPaletteMatch matches[UMI_DESIGNER_MAX_PALETTE_RESULTS];
    size_t count;
} UmiDesignerPaletteResults;

/**
 * Provide the designer palette build operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_palette_build(
    const UmiDeclComponentRegistry *registry,
    const char *category_filter,
    UmiDesignerPalette *out_palette
);

/**
 * Release or reset state held by designer palette so the same storage can be reused
 * safely.
 */
void umi_designer_palette_dispose(UmiDesignerPalette *palette);

/**
 * Provide the designer palette score operation used by this module and its client
 * applications.
 */
int umi_designer_palette_score(
    const char *query,
    const UmiDeclComponentDescriptor *component
);

/**
 * Provide the designer palette search operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_palette_search(
    const UmiDesignerPalette *palette,
    const char *query,
    UmiDesignerPaletteResults *out_results
);

/**
 * Provide the designer palette result operation used by this module and its client
 * applications.
 */
const UmiDeclComponentDescriptor *umi_designer_palette_result(
    const UmiDesignerPalette *palette,
    const UmiDesignerPaletteResults *results,
    size_t result_index
);

#ifdef __cplusplus
}
#endif

#endif
