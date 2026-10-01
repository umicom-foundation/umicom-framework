/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reservations/fixture.h
 * PURPOSE: Create canonical manual, card and transfer reservations with separate non-reserving requests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_BANK_RESERVATIONS_TEST_FIXTURE_H
#define UMICOM_BANK_RESERVATIONS_TEST_FIXTURE_H
#include "../bank_interest/fixture.h"
#include "umicom/bank_operations/reservations.h"
static inline void ReservationActors(Fixture *f)
{
    Id(&f->maker.id,"maker");f->maker.capabilities=UMI_BANK_CAP_CUSTOMERS|UMI_BANK_CAP_PAYMENTS;
    Id(&f->checker.id,"checker");f->checker.capabilities=UMI_BANK_CAP_APPROVE;
    Id(&f->operator.id,"operator");f->operator.capabilities=UMI_BANK_CAP_OPERATE|UMI_BANK_CAP_TEST_FUNDING;
}
static inline void ReservationDestination(Fixture *f)
{
    UmiBankCommand c=Make(f,UMI_BANK_ACCOUNT_OPEN,"destination");Id(&c.ownerId,"customer");strcpy(c.name,"Destination");c.amount=Cash(0);Send(f,&f->maker,&c);
    c=Make(f,UMI_BANK_BENEFICIARY_CREATE,"beneficiary");Id(&c.ownerId,"customer");Id(&c.destinationAccountId,"destination");strcpy(c.name,"Practice destination");Send(f,&f->maker,&c);
}
static inline void ReservationPopulate(Fixture *f)
{
    ReservationDestination(f);
    UmiBankCommand c=Make(f,UMI_BANK_HOLD_PLACE,"shared-id");Id(&c.sourceAccountId,"account");c.amount=Cash(1000);Send(f,&f->operator,&c);
    c=Make(f,UMI_BANK_CARD_ISSUE,"card");Id(&c.sourceAccountId,"account");strcpy(c.name,"Practice card");c.amount=Cash(5000);Send(f,&f->maker,&c);
    c=Make(f,UMI_BANK_CARD_AUTHORISE,"card-hold");Id(&c.ownerId,"card");c.amount=Cash(2000);Send(f,&f->operator,&c);
    c=Make(f,UMI_BANK_TRANSFER_SUBMIT,"shared-id");Id(&c.sourceAccountId,"account");Id(&c.ownerId,"beneficiary");c.amount=Cash(3000);Send(f,&f->maker,&c);
    c=Make(f,UMI_BANK_CHARGE_SUBMIT,"charge");Id(&c.sourceAccountId,"account");Id(&c.ownerId,"reference");strcpy(c.name,"Practice charge");c.amount=Cash(250);Send(f,&f->maker,&c);
    c=Interest(f,"interest","period");Send(f,&f->maker,&c);
    Balance(f,100000,6000);
}
static inline UmiBankReservations *CaptureReservations(Fixture *f)
{UmiBankReservations *report=NULL;OK(UmiBankReservationsCapture(f->bank,"account",&report));CHECK(report!=NULL);return report;}
static inline UmiBankCommand ReleaseCommand(Fixture *f,const UmiBankReservations *report,size_t index)
{
    UmiBankAction action;UmiBankReservationRow row;OK(UmiBankReservationsReleaseAction(report,index,&action));
    OK(UmiBankReservationsRowAt(report,index,&row));return Make(f,action,row.id.value);
}
#endif
