/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/support_resistance.c
 *
 * PURPOSE:
 *   Measure room from current price to the nearest directional level.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of support resistance. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/support_resistance.h"
double umi_room_to_level(double price,double level,int direction){double room=direction>=0?level-price:price-level;return room>0.0?room:0.0;}
