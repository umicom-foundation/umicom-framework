/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/risk_context.c
 *
 * PURPOSE:
 *   Calculate post-order projected position quantity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of risk context. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/risk_context.h"
double umi_risk_projected_position(double current,const UmiOrderRequest *r){if(r==NULL)return current;return current+(r->side==UMI_SIDE_BUY?r->quantity:-r->quantity);}
