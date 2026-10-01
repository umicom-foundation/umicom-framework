/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_queue/fixture.h
 * PURPOSE: Create three request kinds with equal IDs to exercise typed queue selection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_QUEUE_TEST_FIXTURE_H
#define UMICOM_BANK_QUEUE_TEST_FIXTURE_H
#include "../bank_charges/fixture.h"
#include "umicom/bank_operations/work_queue.h"
static inline void QueueSetup(Fixture *f,int64_t chargeMinor)
{
    Setup(f);UmiBankCommand c=Make(f,UMI_BANK_ACCOUNT_OPEN,"recipient");Id(&c.ownerId,"customer");strcpy(c.name,"Recipient");c.amount=Cash(0);Send(f,&f->maker,&c);
    c=Make(f,UMI_BANK_BENEFICIARY_CREATE,"beneficiary");Id(&c.ownerId,"customer");Id(&c.destinationAccountId,"recipient");strcpy(c.name,"Practice recipient");Send(f,&f->maker,&c);
    c=Interest(f,"shared","period");Send(f,&f->maker,&c);
    c=Charge(f,"shared","fee-reference");c.amount=Cash(chargeMinor);strcpy(c.name,"Service, \"review\" caf\xc3\xa9");Send(f,&f->maker,&c);
    c=Make(f,UMI_BANK_TRANSFER_SUBMIT,"shared");Id(&c.ownerId,"beneficiary");Id(&c.sourceAccountId,"account");c.amount=Cash(1000);Send(f,&f->maker,&c);
    c=Make(f,UMI_BANK_CHARGE_APPROVE,"shared");Send(f,&f->checker,&c);
}
static inline UmiBankWorkQueue *QueueCapture(Fixture *f)
{ UmiBankWorkQueue *queue=NULL;OK(UmiBankWorkQueueCapture(f->bank,NULL,&queue));return queue; }
static inline UmiBankCommand QueueCommand(Fixture *f,const UmiBankWorkQueue *queue,size_t index,UmiBankWorkDecision decision)
{
    UmiBankWorkQueueRow row;UmiBankWorkQueueSummary summary;UmiBankAction action;
    OK(UmiBankWorkQueueRowAt(queue,index,&row));OK(UmiBankWorkQueueSummaryRead(queue,&summary));OK(UmiBankWorkQueueResolveAction(queue,index,decision,&action));
    UmiBankCommand c=Make(f,action,row.id.value);c.expectedRevision=summary.revision;return c;
}
#endif
