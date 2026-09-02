/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/breakpoint.c
 *
 * PURPOSE:
 *   Represent named responsive viewport ranges without coupling application code to CSS media queries.
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

#include "umicom/ui/design/breakpoint.h"

#include <string.h>
/*
 * Initialise design breakpoint from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_breakpoint_init(UmiDesignBreakpoint *bp,const char *id,int32_t min_width,int32_t max_width,UmiDesignSizeClass size_class){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(bp==NULL||id==NULL||min_width<0||max_width<min_width||size_class<UMI_DESIGN_SIZE_COMPACT||size_class>UMI_DESIGN_SIZE_WIDE)return UMI_STATUS_INVALID_ARGUMENT;memset(bp,0,sizeof *bp);s=umi_design_copy_text(bp->id,sizeof bp->id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;bp->min_width=min_width;bp->max_width=max_width;bp->size_class=size_class;return UMI_STATUS_OK;}
/*
 * Provide the design breakpoint matches operation used by this module and its client
 * applications.
 */
int umi_design_breakpoint_matches(const UmiDesignBreakpoint *bp,int32_t width){return bp!=NULL&&width>=bp->min_width&&width<=bp->max_width?1:0;}
