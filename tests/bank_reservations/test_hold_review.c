/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reservations/test_hold_review.c
 * PURPOSE: Resolve hold authorisation, partial final capture and refund economics in the shared review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];Fixture f={0};OK(UmiBankOperationsOpenMemory(&f.bank));Setup(&f);ReservationPopulate(&f);
    UmiBankCommand c;
    if(strcmp(name,"place")==0){c=Make(&f,UMI_BANK_HOLD_PLACE,"new-hold");Id(&c.sourceAccountId,"account");c.amount=Cash(400);}
    else if(strcmp(name,"authorise")==0){c=Make(&f,UMI_BANK_CARD_AUTHORISE,"new-card-hold");Id(&c.ownerId,"card");c.amount=Cash(400);}
    else if(strcmp(name,"capture")==0){c=Make(&f,UMI_BANK_CARD_CAPTURE,"card-hold");c.amount=Cash(500);}
    else{CHECK(strcmp(name,"refund")==0);c=Make(&f,UMI_BANK_CARD_CAPTURE,"card-hold");c.amount=Cash(500);Send(&f,&f.operator,&c);c=Make(&f,UMI_BANK_CARD_REFUND,"card-hold");}
    UmiBankReview *review=NULL;OK(UmiBankOperationsReview(f.bank,&f.operator,&c,&review));
    UmiBankReviewSnapshot *s=calloc(1,sizeof *s);CHECK(s!=NULL);OK(UmiBankReviewSnapshotRead(review,s));CHECK(s->hasHold);
    bool newHold=strcmp(name,"place")==0 || strcmp(name,"authorise")==0;
    CHECK(s->holdExistedBefore!=newHold && s->holdAfter.amount.minor_units==(newHold?400:2000));
    CHECK(s->holdAfter.capturedMinor==(newHold?0:500));
    CHECK(s->holdAfter.state==(newHold?UMI_BANK_HOLD_ACTIVE:strcmp(name,"capture")==0?UMI_BANK_HOLD_CAPTURED:UMI_BANK_HOLD_REFUNDED));
    size_t required=0;OK(UmiBankReviewDescribe(review,NULL,0,&required));char *text=malloc(required);CHECK(text!=NULL);
    OK(UmiBankReviewDescribe(review,text,required,NULL));CHECK(strstr(text,"Original reservation:")!=NULL && strstr(text,newHold?"GBP 4.00":"GBP 20.00")!=NULL);
    if(!newHold)CHECK(strstr(text,"Captured amount: GBP 5.00")!=NULL);
    free(text);free(s);UmiBankReviewDestroy(review);UmiBankOperationsDestroy(f.bank);return 0;
}
