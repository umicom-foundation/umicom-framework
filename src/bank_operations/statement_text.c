/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/statement_text.c
 * PURPOSE: Format complete copied statements without a second ledger or floating point.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/bank_operations/statement_text.h"
#include "umicom/finance/money_text.h"
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct StatementText { char *text; size_t capacity, length; UmiStatus status; } StatementText;
static void Append(StatementText *sink, const char *format, ...)
{
    va_list args; int written;
    size_t remaining = sink->length < sink->capacity ? sink->capacity - sink->length : 0U;
    if (sink->status != UMI_STATUS_OK) return;
    va_start(args,format);
    written=vsnprintf(remaining != 0U ? sink->text+sink->length : NULL,remaining,format,args);
    va_end(args);
    if (written < 0 || (size_t)written > SIZE_MAX-sink->length-1U) sink->status=UMI_STATUS_CAPACITY_EXCEEDED;
    else sink->length+=(size_t)written;
}
static void Money(StatementText *sink, UmiMoney money)
{
    char text[UMI_MONEY_TEXT_CAPACITY];
    UmiStatus status=UmiMoneyTextFormat(&money,text,sizeof text,NULL);
    if(status != UMI_STATUS_OK) sink->status=status;
    else Append(sink,"%s",text);
}
UmiStatus UmiBankOperationsDescribeStatement(const UmiBankOperations *operations,
    const char *accountId, uint64_t firstRevision, uint64_t lastRevision,
    char *output, size_t capacity, size_t *outRequired)
{
    UmiBankStatement *statement; UmiBankCounts counts;
    StatementText sink={output,capacity,0U,UMI_STATUS_OK}; UmiStatus status;
    if(outRequired != NULL) *outRequired=0U;
    if(output != NULL && capacity != 0U) output[0]='\0';
    if(operations==NULL || accountId==NULL || (output==NULL && capacity!=0U)) return UMI_STATUS_INVALID_ARGUMENT;
    status=UmiBankOperationsCounts(operations,&counts);
    if(status != UMI_STATUS_OK) return status;
    statement=malloc(sizeof *statement);
    if(statement==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status=UmiBankOperationsStatement(operations,accountId,firstRevision,lastRevision,statement);
    if(status != UMI_STATUS_OK){free(statement);return status;}
    Append(&sink,"LOCAL PRACTICE ACCOUNT STATEMENT\nAccount: %s\nSelected revisions: %" PRIu64 " to %" PRIu64
        " (inclusive)\nCaptured service revision: %" PRIu64 "\nStorage: %s\nOpening: ",
        statement->accountId.value,firstRevision,lastRevision,counts.revision,
        counts.durable ? "persistent SQLite" : "memory-only; lost when the session closes");
    Money(&sink,statement->opening); Append(&sink,"\nClosing: "); Money(&sink,statement->closing);
    Append(&sink,"\nEntries in this range: %zu\n",statement->count);
    for(size_t i=0U;i<statement->count;++i){
        const UmiBankStatementLine *line=&statement->lines[i]; UmiMoney amount=statement->closing;
        Append(&sink,"\nRevision %" PRIu64 "; date %04" PRId32 "-%02u-%02u\nJournal: %s; reference: %s\n  Debit: ",
            line->revision,line->businessDate.year,(unsigned)line->businessDate.month,(unsigned)line->businessDate.day,
            line->journalId.value,line->referenceId.value);
        amount.minor_units=line->debitMinor; Money(&sink,amount); Append(&sink,"; credit: ");
        amount.minor_units=line->creditMinor; Money(&sink,amount); Append(&sink,"; balance: ");
        amount.minor_units=line->balanceMinor; Money(&sink,amount); Append(&sink,"\n");
    }
    Append(&sink,"\nBooked balances only. Holds and unposted approvals are not statement entries.\n"
        "Reversals remain separate compensating entries. Revision order is not a business-date filter.\n"
        "This report is copied from cached local simulation data, not a live bank statement.\n");
    free(statement);
    if(sink.status==UMI_STATUS_OK && outRequired!=NULL) *outRequired=sink.length+1U;
    if(sink.status==UMI_STATUS_OK && output!=NULL && sink.length>=capacity) sink.status=UMI_STATUS_CAPACITY_EXCEEDED;
    if(sink.status!=UMI_STATUS_OK && output!=NULL && capacity!=0U) output[0]='\0';
    return sink.status;
}
