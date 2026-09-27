/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A complete trusted C example: source is editable in Studio; compile it to
 * change the rule. This is not a downloadable plug-in or trading advice. */
#ifndef UMICOM_RESEARCH_EXAMPLE_STRATEGY_H
#define UMICOM_RESEARCH_EXAMPLE_STRATEGY_H
#include "umicom/strategy_research/research_replay.h"
typedef struct PracticeThresholds { double enterBelow; double leaveAbove; } PracticeThresholds;
UmiStatus PracticeStrategy(const UmiResearchView *,void *,UmiResearchDirection *);
UmiInstrument PracticeInstrument(void);
void PracticeQuotes(UmiResearchObservation out[8]);
#endif
