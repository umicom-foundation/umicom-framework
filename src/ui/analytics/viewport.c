/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/viewport.c
 *
 * PURPOSE:
 *   Represent and zoom two-dimensional analytical viewports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/viewport.h"

/*
 * Initialise analytics viewport from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_viewport_init(UmiAnalyticsViewport *v,double x_min,double x_max,double y_min,double y_max){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL||x_min>=x_max||y_min>=y_max)return UMI_STATUS_INVALID_ARGUMENT;v->x_min=x_min;v->x_max=x_max;v->y_min=y_min;v->y_max=y_max;return UMI_STATUS_OK;}
/*
 * Provide the analytics viewport zoom operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_viewport_zoom(UmiAnalyticsViewport *v,double factor,double x_center,double y_center){double hx,hy;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL||factor<=0.0)return UMI_STATUS_INVALID_ARGUMENT;hx=(v->x_max-v->x_min)*0.5/factor;hy=(v->y_max-v->y_min)*0.5/factor;v->x_min=x_center-hx;v->x_max=x_center+hx;v->y_min=y_center-hy;v->y_max=y_center+hy;return UMI_STATUS_OK;}
