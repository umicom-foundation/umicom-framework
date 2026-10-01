/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/work_queue_csv.c
 * PURPOSE: Export the immutable queue with exact monetary amounts and applied filter evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "work_queue_private.h"
UmiStatus UmiBankWorkQueueExportCsv(const UmiBankWorkQueue *queue,UmiCsvDocument **outDocument)
{
    if(outDocument==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *outDocument=NULL;if(queue==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCsvDocument *document=NULL;UmiStatus status=UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES,&document);
    if(status!=UMI_STATUS_OK)return status;
    static const char *const names[]={"record","scope","captured_revision","storage","account_filter","kind_mask","state_mask",
        "visible_count","total_open","kind","id","state","source_account","destination_account","reference","maker","checker",
        "currency","scale","amount_minor","submitted_revision","reason","principal_minor","annual_rate_bps","days","year_basis"};
    UmiCsvCell cells[sizeof(names)/sizeof(names[0])];const size_t count=sizeof(cells)/sizeof(cells[0]);
    for(size_t i=0;i<count;++i)cells[i]=UmiCsvText(names[i]);
    status=UmiCsvDocumentAppendRow(document,cells,count);
    for(size_t i=0;i<count;++i)cells[i]=UmiCsvText("");
    cells[0]=UmiCsvText("queue-summary");cells[1]=UmiCsvText("LOCAL PRACTICE");cells[2]=UmiCsvUnsigned(queue->summary.revision);
    cells[3]=UmiCsvText(queue->summary.durable?"local durable store":"memory only");
    cells[4]=UmiCsvText(queue->summary.filter.accountId.value);cells[5]=UmiCsvUnsigned(queue->summary.filter.kinds);
    cells[6]=UmiCsvUnsigned(queue->summary.filter.states);cells[7]=UmiCsvUnsigned(queue->summary.count);cells[8]=UmiCsvUnsigned(queue->summary.totalOpen);
    if(status==UMI_STATUS_OK)status=UmiCsvDocumentAppendRow(document,cells,count);
    for(size_t i=0;status==UMI_STATUS_OK&&i<queue->summary.count;++i){
        const UmiBankWorkQueueRow *row=&queue->rows[i];
        for(size_t column=9;column<count;++column)cells[column]=UmiCsvText("");
        cells[0]=UmiCsvText("request");cells[9]=UmiCsvText(UmiBankWorkKindName(row->kind));cells[10]=UmiCsvText(row->id.value);
        cells[11]=UmiCsvText(row->state==UMI_BANK_TRANSFER_PENDING?"pending":"approved");
        cells[12]=UmiCsvText(row->sourceAccountId.value);cells[13]=UmiCsvText(row->destinationAccountId.value);
        cells[14]=UmiCsvText(row->referenceId.value);cells[15]=UmiCsvText(row->makerId.value);cells[16]=UmiCsvText(row->checkerId.value);
        cells[17]=UmiCsvText(row->amount.currency.code);cells[18]=UmiCsvUnsigned(row->amount.scale);cells[19]=UmiCsvSigned(row->amount.minor_units);
        cells[20]=UmiCsvUnsigned(row->submittedRevision);cells[21]=UmiCsvText(row->reason);
        if(row->kind==UMI_BANK_WORK_INTEREST){cells[22]=UmiCsvSigned(row->principal.minor_units);cells[23]=UmiCsvSigned(row->interest.annualRateBps);
            cells[24]=UmiCsvUnsigned(row->interest.days);cells[25]=UmiCsvUnsigned(row->interest.dayCountBasis);}
        status=UmiCsvDocumentAppendRow(document,cells,count);
    }
    if(status!=UMI_STATUS_OK){UmiCsvDocumentDestroy(document);return status;}
    *outDocument=document;return UMI_STATUS_OK;
}
