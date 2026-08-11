/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/launcher.c
 *
 * PURPOSE:
 *   Provide a toolkit-neutral launcher decision for console and graphical entry points.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This implementation keeps product composition separate from reusable Framework mechanisms. It lets independent applications share the same platform without copying infrastructure.
 */
#include "umicom/product/launcher.h"
#include <stddef.h>
UmiStatus umi_product_launcher_select(const UmiProductProfile *p,UmiProductLaunchMode preferred,UmiProductFrontendKind *o){size_t i;UmiProductFrontendKind wanted;if(o==NULL||umi_product_profile_validate(p)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;wanted=preferred==UMI_PRODUCT_LAUNCH_CONSOLE?UMI_PRODUCT_FRONTEND_CONSOLE:(preferred==UMI_PRODUCT_LAUNCH_WEB?UMI_PRODUCT_FRONTEND_WEB:UMI_PRODUCT_FRONTEND_GTK4);for(i=0U;i<p->frontend_count;++i)if(p->frontends[i].enabled&&p->frontends[i].kind==wanted){*o=wanted;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
