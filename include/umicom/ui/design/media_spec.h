/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/media_spec.h
 *
 * PURPOSE:
 *   Define image, audio and video media presentation and transport-control semantics.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_MEDIA_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_MEDIA_SPEC_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDesignMediaKind { UMI_DESIGN_MEDIA_IMAGE=1, UMI_DESIGN_MEDIA_AUDIO=2, UMI_DESIGN_MEDIA_VIDEO=3 } UmiDesignMediaKind;
typedef struct UmiDesignMediaSpec { UmiDesignMediaKind kind; int controls; int autoplay; int loop; int preserve_aspect; } UmiDesignMediaSpec;
/* Initialise a semantic media-surface specification. */
UmiStatus umi_design_media_spec_init(UmiDesignMediaSpec *spec, UmiDesignMediaKind kind, int controls, int autoplay, int loop, int preserve_aspect);
/* Return one when media kind and behaviour are valid. */
int umi_design_media_spec_valid(const UmiDesignMediaSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
