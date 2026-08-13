/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/request_ledger.h
 * PURPOSE: Correlate bounded LSP request lifecycle and latency evidence.
 * Created by: Sammy Hegab | Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LANGUAGE_REQUEST_LEDGER_H
#define UMICOM_LANGUAGE_REQUEST_LEDGER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_REQUEST_LEDGER_CAPACITY 1024U
typedef enum UmiLanguageRequestState { UMI_LANGUAGE_REQUEST_PENDING=0, UMI_LANGUAGE_REQUEST_COMPLETED=1, UMI_LANGUAGE_REQUEST_FAILED=2, UMI_LANGUAGE_REQUEST_CANCELLED=3 } UmiLanguageRequestState;
typedef struct UmiLanguageRequestRecord { int64_t request_id; char method[128]; char document_uri[1024]; uint64_t started_at; uint64_t completed_at; UmiLanguageRequestState state; UmiStatus status; uint64_t revision; } UmiLanguageRequestRecord;
typedef struct UmiLanguageRequestLedger UmiLanguageRequestLedger;
UmiStatus umi_language_request_ledger_create(UmiLanguageRequestLedger **out_ledger);
void umi_language_request_ledger_destroy(UmiLanguageRequestLedger *ledger);
UmiStatus umi_language_request_ledger_begin(UmiLanguageRequestLedger *ledger,const UmiLanguageRequestRecord *record);
UmiStatus umi_language_request_ledger_complete(UmiLanguageRequestLedger *ledger,int64_t request_id,UmiStatus status,uint64_t completed_at);
UmiStatus umi_language_request_ledger_find(const UmiLanguageRequestLedger *ledger,int64_t request_id,UmiLanguageRequestRecord *out_record);
UmiStatus umi_language_request_ledger_at(const UmiLanguageRequestLedger *ledger,size_t index,UmiLanguageRequestRecord *out_record);
size_t umi_language_request_ledger_count(const UmiLanguageRequestLedger *ledger);
size_t umi_language_request_ledger_pending(const UmiLanguageRequestLedger *ledger);
#ifdef __cplusplus
}
#endif
#endif
