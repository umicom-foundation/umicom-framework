/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_parse_internal.h
 * Purpose: Bound report fields before reconstructing immutable evidence.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_QUALIFICATION_PARSE_INTERNAL_H
#define UMICOM_QUALIFICATION_PARSE_INTERNAL_H
#include "qualification_internal.h"
#include "umicom/distribution/runtime/qualification_view.h"
typedef struct UmiQualificationField { const char *bytes; size_t length; } UmiQualificationField;
typedef struct UmiQualificationReader { const char *next, *end; size_t names; } UmiQualificationReader;
bool UmiQualificationFieldIs(UmiQualificationField field, const char *text);
UmiStatus UmiQualificationReadRow(UmiQualificationReader *reader, UmiQualificationField *fields,
    size_t capacity, size_t *outCount);
UmiStatus UmiQualificationReadNumber(UmiQualificationField field, uint64_t maximum, uint64_t *out);
UmiStatus UmiQualificationReadDigest(UmiQualificationField field, bool optional, char out[65]);
UmiStatus UmiQualificationReadName(UmiQualificationReader *reader, UmiQualificationField field, char **out);
UmiStatus UmiQualificationReadResult(const UmiQualificationField *fields, size_t count,
    UmiReleaseQualificationResult *result, bool *recorded);
void UmiQualificationViewMarkSaved(UmiQualificationView *view);
#endif
