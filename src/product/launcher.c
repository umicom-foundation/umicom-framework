/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/launcher.c
 *
 * PURPOSE:
 *   Provide a toolkit-neutral launcher decision for console and graphical entry points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/launcher.h"
#include <stddef.h>
/*
 * Provide the product launcher select operation used by this module and its client
 * applications.
 */
UmiStatus umi_product_launcher_select(const UmiProductProfile *p,UmiProductLaunchMode preferred,UmiProductFrontendKind *o){size_t i;UmiProductFrontendKind wanted;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(o==NULL||umi_product_profile_validate(p)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;wanted=preferred==UMI_PRODUCT_LAUNCH_CONSOLE?UMI_PRODUCT_FRONTEND_CONSOLE:(preferred==UMI_PRODUCT_LAUNCH_WEB?UMI_PRODUCT_FRONTEND_WEB:UMI_PRODUCT_FRONTEND_GTK4);/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->frontend_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->frontends[i].enabled&&p->frontends[i].kind==wanted){*o=wanted;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
