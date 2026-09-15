/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/scores/score.h
 *
 * PURPOSE:
 *   Declare the small score contract used only by these exercises.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LEARNING_SCORE_H
#define UMICOM_LEARNING_SCORE_H
#include <stdbool.h>

/* A score outside 0..100 is invalid rather than silently clamped. */
bool LearningScoreIsValid(int score);
/* Only valid scores at or above 70 are ready. */
bool LearningScoreIsReady(int score);
#endif
