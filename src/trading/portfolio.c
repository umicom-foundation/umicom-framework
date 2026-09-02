/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/portfolio.c
 *
 * PURPOSE:
 *   Aggregate absolute gross position quantity across a position book.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of portfolio. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/portfolio.h"
/*
 * Provide the portfolio gross quantity operation used by this module and its client
 * applications.
 */
double umi_portfolio_gross_quantity(const UmiPositionBook *b){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL)return 0.0;double total=0.0;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<b->count;i++){double q=b->positions[i].quantity;total+=q<0.0?-q:q;}return total;}
