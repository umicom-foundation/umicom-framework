/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/observation_wire.h
 * PURPOSE: Share bounded field storage for independently owned broker observations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_OBSERVATION_WIRE_H
#define UMICOM_IBKR_OBSERVATION_WIRE_H
#include "internal.h"
typedef struct UmiIbkrObservationFields
{
    char *storage;
    char **values;
    size_t count;
} UmiIbkrObservationFields;
/* Own one copied frame and its field pointers. No pointer survives Close. */
UmiStatus UmiIbkrObservationFieldsOpen(const unsigned char *body, size_t length, size_t maximumFields,
                                       UmiIbkrObservationFields *out);
void UmiIbkrObservationFieldsClose(UmiIbkrObservationFields *fields);
UmiStatus UmiIbkrObservationIgnore(UmiIbkrConnection *connection);
#endif
