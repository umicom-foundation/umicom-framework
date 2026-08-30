/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/pnl.c
 *
 * PURPOSE:
 *   Calculate unrealised and total P&L for a position.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of pnl. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/pnl.h"
double umi_position_unrealised_pnl(const UmiPosition *p,double mark){return p!=NULL&&mark>0.0?(mark-p->average_price)*p->quantity*p->instrument.multiplier:0.0;}
double umi_position_total_pnl(const UmiPosition *p,double mark){return p!=NULL?p->realised_pnl+umi_position_unrealised_pnl(p,mark):0.0;}
