/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/component_variant.c
 *
 * PURPOSE:
 *   Define named semantic presentation variants without coupling variants to CSS classes or toolkit widget subclasses.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/component_variant.h"

#include <string.h>
UmiStatus umi_design_component_variant_init(UmiDesignComponentVariant *variant,const char *id,UmiDesignSemanticRole role,int outlined,int subtle){UmiStatus s;if(variant==NULL||id==NULL||role<UMI_DESIGN_ROLE_NEUTRAL||role>UMI_DESIGN_ROLE_ACCENT)return UMI_STATUS_INVALID_ARGUMENT;memset(variant,0,sizeof *variant);s=umi_design_copy_text(variant->id,sizeof variant->id,id);if(s!=UMI_STATUS_OK)return s;variant->role=role;variant->outlined=outlined?1:0;variant->subtle=subtle?1:0;return UMI_STATUS_OK;}
