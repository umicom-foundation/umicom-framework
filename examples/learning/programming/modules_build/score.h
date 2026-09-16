/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/modules_build/score.h
 *
 * PURPOSE:
 *   Declare a bounded score normalisation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LEARNING_PROGRAMMING_SCORE_H
#define UMICOM_LEARNING_PROGRAMMING_SCORE_H
#include <stdbool.h>
/* 0 <= value <= maximum <= 100; output remains unchanged on rejection. */
bool NormaliseScore(unsigned value, unsigned maximum, unsigned *outPercent);
#endif
