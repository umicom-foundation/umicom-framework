/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/modules_build/score.c
 *
 * PURPOSE:
 *   Implement the small score contract with bounded arithmetic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "score.h"
#include <stddef.h>
bool NormaliseScore(unsigned value, unsigned maximum, unsigned *outPercent)
{
    if (outPercent==NULL || maximum==0U || maximum>100U || value>maximum) return false;
    *outPercent=(value*100U)/maximum;
    return true;
}
