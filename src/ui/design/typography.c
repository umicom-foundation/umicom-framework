/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/typography.c
 *
 * PURPOSE:
 *   Define validated toolkit-neutral typography specifications for semantic text roles.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/typography.h"

#include <string.h>
UmiStatus umi_design_typography_init(UmiDesignTypography *spec,const char *family,double size,uint16_t weight,double line_height)
{ UmiStatus s; if(spec==NULL||family==NULL||!umi_design_number_valid(size)||!umi_design_number_valid(line_height)||size<=0.0||line_height<1.0||weight<100U||weight>1000U)return UMI_STATUS_INVALID_ARGUMENT; memset(spec,0,sizeof *spec); s=umi_design_copy_text(spec->family,sizeof spec->family,family); if(s!=UMI_STATUS_OK)return s; spec->size=size;spec->weight=weight;spec->line_height=line_height;return UMI_STATUS_OK; }
int umi_design_typography_valid(const UmiDesignTypography *spec) { return spec!=NULL && spec->family[0]!='\0' && umi_design_number_valid(spec->size) && spec->size>0.0 && spec->weight>=100U && spec->weight<=1000U && umi_design_number_valid(spec->line_height) && spec->line_height>=1.0; }
