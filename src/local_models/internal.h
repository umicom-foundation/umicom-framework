/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/internal.h
 *
 * PURPOSE:
 *   Keep bounded validation helpers private to the native model service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LOCAL_MODELS_INTERNAL_H
#define UMICOM_LOCAL_MODELS_INTERNAL_H
#include "umicom/local_models/manager.h"
bool UmiModelTextFits(const char *text, size_t capacity);
bool UmiModelDigestValid(const char *text, size_t digits);
bool UmiModelNameValid(const char *text, size_t capacity, bool paths);
bool UmiModelNumber(const char *text, uint64_t minimum, uint64_t maximum, uint64_t *out);
UmiStatus UmiModelRunPlanValidate(const UmiModelRunPlan *plan);
#endif
