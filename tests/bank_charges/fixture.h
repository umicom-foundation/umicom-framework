/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_charges/fixture.h
 * PURPOSE: Extend the shared practice-account fixture with reviewed charge commands.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_CHARGES_TEST_FIXTURE_H
#define UMICOM_BANK_CHARGES_TEST_FIXTURE_H
#include "../bank_interest/fixture.h"
static inline UmiBankCommand Charge(Fixture *f, const char *id, const char *reference)
{
    UmiBankCommand command=Make(f,UMI_BANK_CHARGE_SUBMIT,id);
    Id(&command.ownerId,reference); Id(&command.sourceAccountId,"account");
    strcpy(command.name,"Practice service charge"); command.amount=Cash(250); return command;
}
static inline void ChargeStep(Fixture *f,UmiBankAction action,const UmiBankActor *actor)
{ UmiBankCommand command=Make(f,action,"charge"); Send(f,actor,&command); }
static inline void ChargeFail(Fixture *f,const UmiBankActor *actor,UmiBankCommand command,UmiStatus expected)
{
    UmiBankCounts before,after; UmiBankChargeRequest prior={0},current={0};
    OK(UmiBankOperationsCounts(f->bank,&before));
    if(before.chargeRequests!=0) OK(UmiBankOperationsChargeAt(f->bank,0,&prior));
    Fail(f,actor,command,expected); OK(UmiBankOperationsCounts(f->bank,&after));
    CHECK(before.chargeRequests==after.chargeRequests);
    if(after.chargeRequests!=0) {
        OK(UmiBankOperationsChargeAt(f->bank,0,&current));
        CHECK(prior.state==current.state && prior.postedRevision==current.postedRevision &&
            prior.reversedRevision==current.reversedRevision && prior.amount.minor_units==current.amount.minor_units);
    }
}
#endif
