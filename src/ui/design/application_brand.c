/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/application_brand.c
 *
 * PURPOSE:
 *   Represent application-level identity as semantic accents and density defaults without embedding toolkit-specific styling.
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

#include "umicom/ui/design/application_brand.h"

#include <string.h>
/*
 * Initialise design application brand from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_application_brand_init(UmiDesignApplicationBrand *brand,const char *application_id,const char *display_name,UmiDesignRgba accent,UmiDesignRgba secondary,UmiDesignDensity density){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(brand==NULL||application_id==NULL||display_name==NULL||!umi_design_color_valid(accent)||!umi_design_color_valid(secondary)||density<UMI_DESIGN_DENSITY_COMPACT||density>UMI_DESIGN_DENSITY_TOUCH)return UMI_STATUS_INVALID_ARGUMENT;memset(brand,0,sizeof *brand);s=umi_design_copy_text(brand->application_id,sizeof brand->application_id,application_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_design_copy_text(brand->display_name,sizeof brand->display_name,display_name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;brand->accent=accent;brand->secondary=secondary;brand->density=density;return UMI_STATUS_OK;}
/* Provide the preset colors operation used by this module and its client applications. */
static UmiStatus preset_colors(const char *application_id,const char **name,UmiDesignRgba *accent,UmiDesignRgba *secondary,UmiDesignDensity *density){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(application_id,"studio")==0){*name="Umicom Studio";*accent=(UmiDesignRgba){0.55,0.36,0.96,1.0};*secondary=(UmiDesignRgba){0.29,0.57,1.0,1.0};*density=UMI_DESIGN_DENSITY_COMPACT;return UMI_STATUS_OK;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(application_id,"trader")==0){*name="Umicom Trader";*accent=(UmiDesignRgba){0.20,0.83,0.60,1.0};*secondary=(UmiDesignRgba){0.36,0.66,1.0,1.0};*density=UMI_DESIGN_DENSITY_COMPACT;return UMI_STATUS_OK;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(application_id,"tms")==0){*name="Umicom TMS";*accent=(UmiDesignRgba){0.32,0.78,0.91,1.0};*secondary=(UmiDesignRgba){0.31,0.55,0.97,1.0};*density=UMI_DESIGN_DENSITY_COMPACT;return UMI_STATUS_OK;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(application_id,"bank")==0){*name="Umicom Bank";*accent=(UmiDesignRgba){0.28,0.84,0.82,1.0};*secondary=(UmiDesignRgba){0.60,0.52,1.0,1.0};*density=UMI_DESIGN_DENSITY_STANDARD;return UMI_STATUS_OK;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(application_id,"llm")==0){*name="Umicom LLM";*accent=(UmiDesignRgba){0.49,0.42,1.0,1.0};*secondary=(UmiDesignRgba){0.34,0.84,0.76,1.0};*density=UMI_DESIGN_DENSITY_STANDARD;return UMI_STATUS_OK;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(application_id,"accountant")==0){*name="Umicom Accountant";*accent=(UmiDesignRgba){0.13,0.71,0.55,1.0};*secondary=(UmiDesignRgba){0.29,0.64,1.0,1.0};*density=UMI_DESIGN_DENSITY_STANDARD;return UMI_STATUS_OK;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(application_id,"commodity")==0){*name="Umicom Commodity Exchange";*accent=(UmiDesignRgba){0.29,0.84,0.61,1.0};*secondary=(UmiDesignRgba){0.89,0.71,0.34,1.0};*density=UMI_DESIGN_DENSITY_COMPACT;return UMI_STATUS_OK;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(application_id,"music")==0){*name="Umicom Music Studio";*accent=(UmiDesignRgba){0.66,0.45,1.0,1.0};*secondary=(UmiDesignRgba){1.0,0.47,0.74,1.0};*density=UMI_DESIGN_DENSITY_COMPACT;return UMI_STATUS_OK;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(application_id,"video")==0){*name="Umicom Video Studio";*accent=(UmiDesignRgba){0.55,0.36,1.0,1.0};*secondary=(UmiDesignRgba){0.13,0.77,0.84,1.0};*density=UMI_DESIGN_DENSITY_COMPACT;return UMI_STATUS_OK;}/* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(application_id,"os")==0){*name="Umicom OS";*accent=(UmiDesignRgba){0.33,0.65,1.0,1.0};*secondary=(UmiDesignRgba){0.48,0.82,1.0,1.0};*density=UMI_DESIGN_DENSITY_STANDARD;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
/*
 * Provide the design application brand preset operation used by this module and its client
 * applications.
 */
UmiStatus umi_design_application_brand_preset(const char *application_id,UmiDesignApplicationBrand *out_brand){const char *name;UmiDesignRgba accent,secondary;UmiDesignDensity density;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(application_id==NULL||out_brand==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=preset_colors(application_id,&name,&accent,&secondary,&density);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;return umi_design_application_brand_init(out_brand,application_id,name,accent,secondary,density);}
