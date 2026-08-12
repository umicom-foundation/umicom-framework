/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/palette.h
 *
 * PURPOSE:
 *   Expose component palettes and reusable fuzzy palette search derived from
 *   the declarative component registry and plug-in contributions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Palette metadata and search belong to Framework. A GTK4, web or other host
 * only decides how to present the Framework-owned results.
 */

#ifndef UMICOM_DESIGNER_PALETTE_H
#define UMICOM_DESIGNER_PALETTE_H

#include "umicom/designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerPalette {
    UmiDeclComponentDescriptor *items;
    size_t count;
} UmiDesignerPalette;

typedef struct UmiDesignerPaletteMatch {
    size_t palette_index;
    int score;
} UmiDesignerPaletteMatch;

typedef struct UmiDesignerPaletteResults {
    UmiDesignerPaletteMatch matches[UMI_DESIGNER_MAX_PALETTE_RESULTS];
    size_t count;
} UmiDesignerPaletteResults;

UmiStatus umi_designer_palette_build(
    const UmiDeclComponentRegistry *registry,
    const char *category_filter,
    UmiDesignerPalette *out_palette
);

void umi_designer_palette_dispose(UmiDesignerPalette *palette);

int umi_designer_palette_score(
    const char *query,
    const UmiDeclComponentDescriptor *component
);

UmiStatus umi_designer_palette_search(
    const UmiDesignerPalette *palette,
    const char *query,
    UmiDesignerPaletteResults *out_results
);

const UmiDeclComponentDescriptor *umi_designer_palette_result(
    const UmiDesignerPalette *palette,
    const UmiDesignerPaletteResults *results,
    size_t result_index
);

#ifdef __cplusplus
}
#endif

#endif
