/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/research_window.c
 *
 * PURPOSE:
 *   Determine whether a research observation occurs inside the configured look-forward horizon.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of research window. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/research_window.h"
int umi_research_window_contains(int64_t anchor,int64_t event,int64_t forward){return forward>=0&&event>=anchor&&event-anchor<=forward;}
