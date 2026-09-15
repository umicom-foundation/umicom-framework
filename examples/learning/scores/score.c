/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/scores/score.c
 *
 * PURPOSE:
 *   Implement the score contract without a user-interface dependency.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "score.h"

bool LearningScoreIsValid(int score)
{
    return score >= 0 && score <= 100;
}

bool LearningScoreIsReady(int score)
{
    return LearningScoreIsValid(score) && score >= 70;
}
