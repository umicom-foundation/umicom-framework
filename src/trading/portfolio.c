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
double umi_portfolio_gross_quantity(const UmiPositionBook *b){if(b==NULL)return 0.0;double total=0.0;for(size_t i=0U;i<b->count;i++){double q=b->positions[i].quantity;total+=q<0.0?-q:q;}return total;}
