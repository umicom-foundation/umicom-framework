/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/reservations_csv.c
 * PURPOSE: Export one captured account reservation explanation without repeating summary balances on detail rows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "reservations_private.h"
UmiStatus UmiBankReservationsExportCsv(const UmiBankReservations *report,UmiCsvDocument **outDocument)
{
    if(outDocument==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument=NULL;if(report==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCsvDocument *document=NULL;UmiStatus status=UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES,&document);
    if(status!=UMI_STATUS_OK)return status;
    static const char *const names[]={"record","scope","account_id","currency","scale","captured_revision","storage",
        "booked_minor","reserved_minor","available_minor","manual_minor","card_minor","transfer_minor","reservation_count",
        "kind","id","amount_minor","created_revision","maker_id","card_id","destination_account_id","beneficiary_id","state"};
    UmiCsvCell cells[sizeof names/sizeof names[0]];size_t count=sizeof cells/sizeof cells[0];
    for(size_t i=0;i<count;++i)cells[i]=UmiCsvText(names[i]);
    status=UmiCsvDocumentAppendRow(document,cells,count);
    for(size_t i=0;i<count;++i)cells[i]=UmiCsvText("");
    const UmiBankReservationsSummary *s=&report->summary;
    cells[0]=UmiCsvText("reservation-summary");cells[1]=UmiCsvText("LOCAL PRACTICE");cells[2]=UmiCsvText(s->accountId.value);
    cells[3]=UmiCsvText(s->balance.booked.currency.code);cells[4]=UmiCsvUnsigned(s->balance.booked.scale);
    cells[5]=UmiCsvUnsigned(s->revision);cells[6]=UmiCsvText(s->durable?"local durable store":"memory only");
    cells[7]=UmiCsvSigned(s->balance.booked.minor_units);cells[8]=UmiCsvSigned(s->balance.reserved.minor_units);
    cells[9]=UmiCsvSigned(s->balance.available.minor_units);cells[10]=UmiCsvSigned(s->manualMinor);
    cells[11]=UmiCsvSigned(s->cardMinor);cells[12]=UmiCsvSigned(s->transferMinor);cells[13]=UmiCsvUnsigned(s->count);
    if(status==UMI_STATUS_OK)status=UmiCsvDocumentAppendRow(document,cells,count);
    for(size_t i=0;status==UMI_STATUS_OK && i<s->count;++i){
        const UmiBankReservationRow *row=&report->rows[i];
        for(size_t j=7;j<count;++j)cells[j]=UmiCsvText("");
        cells[0]=UmiCsvText("reservation");cells[14]=UmiCsvText(UmiBankReservationKindName(row->kind));cells[15]=UmiCsvText(row->id.value);
        cells[16]=UmiCsvSigned(row->amount.minor_units);cells[17]=UmiCsvUnsigned(row->createdRevision);cells[18]=UmiCsvText(row->makerId.value);
        cells[19]=UmiCsvText(row->cardId.value);cells[20]=UmiCsvText(row->destinationAccountId.value);cells[21]=UmiCsvText(row->referenceId.value);
        cells[22]=UmiCsvText(row->kind==UMI_BANK_RESERVATION_TRANSFER?
            (row->transferState==UMI_BANK_TRANSFER_APPROVED?"approved":"pending approval"):"active");
        status=UmiCsvDocumentAppendRow(document,cells,count);
    }
    if(status!=UMI_STATUS_OK){UmiCsvDocumentDestroy(document);return status;}
    *outDocument=document;return UMI_STATUS_OK;
}
