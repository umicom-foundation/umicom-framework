/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/typography_scale.h
 *
 * PURPOSE:
 *   Maintain named typography levels for captions, body text, titles and display surfaces.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_TYPOGRAPHY_SCALE_H
#define INCLUDE_UMICOM_UI_DESIGN_TYPOGRAPHY_SCALE_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/typography.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/typography.h"
/**
 * Represent the design typography scale entry data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignTypographyScaleEntry { char level_id[UMI_DESIGN_ID_CAPACITY]; UmiDesignTypography typography; } UmiDesignTypographyScaleEntry;
/**
 * Represent the design typography scale data shared with callers of this public contract.
 */
typedef struct UmiDesignTypographyScale { UmiDesignTypographyScaleEntry entries[UMI_DESIGN_MAX_ITEMS]; size_t count; } UmiDesignTypographyScale;
/* Insert or replace one named typography level. */
UmiStatus umi_design_typography_scale_upsert(UmiDesignTypographyScale *scale, const char *level_id, const UmiDesignTypography *typography);
/* Resolve one named typography level. */
UmiStatus umi_design_typography_scale_find(const UmiDesignTypographyScale *scale, const char *level_id, UmiDesignTypography *out_typography);

#ifdef __cplusplus
}
#endif

#endif
