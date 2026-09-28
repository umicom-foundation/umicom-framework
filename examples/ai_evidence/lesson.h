/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_AI_EVIDENCE_LESSON_H
#define UMICOM_AI_EVIDENCE_LESSON_H
#include "umicom/ai_workspace/evidence.h"
#include <stdio.h>
UmiStatus UmiAiEvidenceLesson(FILE *output, bool verbose, bool local, uint16_t port, const char *model);
#endif
