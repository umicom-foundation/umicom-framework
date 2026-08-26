/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/renderer_parity_matrix.h
 *
 * PURPOSE:
 *   Track renderer-by-renderer appearance conformance for semantic theme capabilities.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_RENDERER_PARITY_MATRIX_H
#define UMICOM_UI_APPEARANCE_RENDERER_PARITY_MATRIX_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceRendererParityEntry { UmiAppearanceRendererKind renderer; bool theme; bool typography; bool scaling; bool accessibility; } UmiAppearanceRendererParityEntry;
typedef struct UmiAppearanceRendererParityMatrix { UmiAppearanceRendererParityEntry items[8]; size_t count; } UmiAppearanceRendererParityMatrix;
/* Initialise an empty renderer parity matrix. */
UmiStatus umi_appearance_renderer_parity_matrix_init(UmiAppearanceRendererParityMatrix *matrix);
/* Add one renderer capability result. */
UmiStatus umi_appearance_renderer_parity_matrix_add(UmiAppearanceRendererParityMatrix *matrix,const UmiAppearanceRendererParityEntry *entry);
/* Return one only when every registered renderer passes all required appearance dimensions. */
int umi_appearance_renderer_parity_matrix_passed(const UmiAppearanceRendererParityMatrix *matrix);

#ifdef __cplusplus
}
#endif
#endif
