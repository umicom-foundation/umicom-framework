/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/theme_inheritance.h
 *
 * PURPOSE:
 *   Validate acyclic theme-pack inheritance before layered token resolution.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_THEME_INHERITANCE_H
#define UMICOM_UI_APPEARANCE_THEME_INHERITANCE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance theme inheritance edge data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceThemeInheritanceEdge { char child_id[UMI_APPEARANCE_ID_CAPACITY]; char parent_id[UMI_APPEARANCE_ID_CAPACITY]; } UmiAppearanceThemeInheritanceEdge;
/**
 * Represent the appearance theme inheritance data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceThemeInheritance { UmiAppearanceThemeInheritanceEdge edges[UMI_APPEARANCE_MAX_ITEMS]; size_t count; } UmiAppearanceThemeInheritance;

/* Initialise an empty theme inheritance graph. */
UmiStatus umi_appearance_theme_inheritance_init(UmiAppearanceThemeInheritance *graph);
/* Add a child-to-parent edge only when it preserves an acyclic graph. */
UmiStatus umi_appearance_theme_inheritance_add(UmiAppearanceThemeInheritance *graph, const char *child_id, const char *parent_id);
/* Return one when the current graph contains an inheritance cycle. */
int umi_appearance_theme_inheritance_has_cycle(const UmiAppearanceThemeInheritance *graph);

#ifdef __cplusplus
}
#endif
#endif
