/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/reservations_private.h
 * PURPOSE: Own copied reservation rows and canonical history independently of the banking handle.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_BANK_RESERVATIONS_PRIVATE_H
#define UMICOM_BANK_RESERVATIONS_PRIVATE_H
#include "umicom/bank_operations/reservations.h"
struct UmiBankReservations {
    UmiBankReservationsSummary summary;
    UmiBankReservationRow rows[UMI_BANK_RESERVATION_CAPACITY];
    UmiBankAuditEvent *history;
    size_t eventCount;
};
#endif
