/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/media_spec.c
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

#include "umicom/ui/design/media_spec.h"

int umi_design_media_spec_valid(const UmiDesignMediaSpec *spec){return spec!=NULL&&spec->kind>=UMI_DESIGN_MEDIA_IMAGE&&spec->kind<=UMI_DESIGN_MEDIA_VIDEO&&!(spec->kind==UMI_DESIGN_MEDIA_IMAGE&&spec->autoplay)?1:0;}
UmiStatus umi_design_media_spec_init(UmiDesignMediaSpec *spec,UmiDesignMediaKind kind,int controls,int autoplay,int loop,int preserve_aspect){if(spec==NULL)return UMI_STATUS_INVALID_ARGUMENT;spec->kind=kind;spec->controls=controls?1:0;spec->autoplay=autoplay?1:0;spec->loop=loop?1:0;spec->preserve_aspect=preserve_aspect?1:0;return umi_design_media_spec_valid(spec)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
