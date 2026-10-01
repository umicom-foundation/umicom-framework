/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/reservations_text.c
 * PURPOSE: Explain complete booked, reserved and available balances with their captured contributing records.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "reservations_private.h"
#include "umicom/finance/money_text.h"
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
typedef struct ReservationText {char *text;size_t capacity,length;UmiStatus status;} ReservationText;
static void Append(ReservationText *sink,const char *format,...)
{
    if(sink->status!=UMI_STATUS_OK)return;
    size_t remaining=sink->length<sink->capacity?sink->capacity-sink->length:0U;
    va_list args;va_start(args,format);
    int written=vsnprintf(remaining?sink->text+sink->length:NULL,remaining,format,args);va_end(args);
    if(written<0 || (size_t)written>SIZE_MAX-sink->length-1U)sink->status=UMI_STATUS_CAPACITY_EXCEEDED;
    else sink->length+=(size_t)written;
}
static void Money(ReservationText *sink,UmiMoney value)
{
    char text[UMI_MONEY_TEXT_CAPACITY];UmiStatus status=UmiMoneyTextFormat(&value,text,sizeof text,NULL);
    if(status!=UMI_STATUS_OK)sink->status=status;else Append(sink,"%s",text);
}
UmiStatus UmiBankReservationsDescribe(const UmiBankReservations *report,char *output,size_t capacity,size_t *outRequired)
{
    if(outRequired!=NULL)*outRequired=0U;
    if(output!=NULL && capacity!=0U)output[0]='\0';
    if(report==NULL || (output==NULL && capacity!=0U))return UMI_STATUS_INVALID_ARGUMENT;
    ReservationText sink={output,capacity,0U,UMI_STATUS_OK};const UmiBankReservationsSummary *s=&report->summary;
    Append(&sink,"LOCAL PRACTICE RESERVED FUNDS\nAccount: %s\nCaptured revision: %" PRIu64 "\nStorage: %s\n\nBooked: ",
        s->accountId.value,s->revision,s->durable?"persistent local SQLite":"memory only");
    Money(&sink,s->balance.booked);Append(&sink,"\nReserved: ");Money(&sink,s->balance.reserved);
    Append(&sink,"\nAvailable = booked minus reserved: ");Money(&sink,s->balance.available);
    UmiMoney amount=s->balance.booked;amount.minor_units=s->manualMinor;
    Append(&sink,"\n\nManual holds (%zu): ",s->manualCount);Money(&sink,amount);amount.minor_units=s->cardMinor;
    Append(&sink,"\nCard authorisations (%zu): ",s->cardCount);Money(&sink,amount);amount.minor_units=s->transferMinor;
    Append(&sink,"\nPending / approved transfers (%zu): ",s->transferCount);Money(&sink,amount);
    Append(&sink,"\nThese three totals exactly explain the reserved balance at capture.\n");
    if(s->count==0U)Append(&sink,"No active reservations for this account.\n");
    for(size_t i=0;i<s->count;++i){
        const UmiBankReservationRow *row=&report->rows[i];
        Append(&sink,"\n%zu. %s | %s\nReserved amount: ",i+1U,UmiBankReservationKindName(row->kind),row->id.value);Money(&sink,row->amount);
        Append(&sink,"\nCreated revision: %" PRIu64 "; maker: %s\n",row->createdRevision,row->makerId.value);
        if(row->kind==UMI_BANK_RESERVATION_CARD)Append(&sink,"Card: %s; state: active authorisation\n",row->cardId.value);
        else if(row->kind==UMI_BANK_RESERVATION_TRANSFER)Append(&sink,"To account: %s; beneficiary: %s; state: %s\n",
            row->destinationAccountId.value,row->referenceId.value,row->transferState==UMI_BANK_TRANSFER_APPROVED?"approved":"pending approval");
        else Append(&sink,"State: active manual hold\n");
    }
    Append(&sink,"\nReservations are not posted debits. Released, captured and refunded holds and completed transfers are excluded.\n"
        "Interest and charges do not reserve money. Their open requests appear in Review queue.\n"
        "This copied report does not update automatically. Review release / cancel separately; no expiry, payment or network action occurs here.\n");
    if(sink.status==UMI_STATUS_OK && outRequired!=NULL)*outRequired=sink.length+1U;
    if(sink.status==UMI_STATUS_OK && output!=NULL && sink.length>=capacity)sink.status=UMI_STATUS_CAPACITY_EXCEEDED;
    if(sink.status!=UMI_STATUS_OK && output!=NULL && capacity!=0U)output[0]='\0';
    return sink.status;
}
