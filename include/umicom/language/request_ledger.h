/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/request_ledger.h
 *
 * PURPOSE:
 *   Publish the public request ledger contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * List the named language request state values accepted by this public contract.
 */
typedef enum UmiLanguageRequestState { UMI_LANGUAGE_REQUEST_PENDING=0, UMI_LANGUAGE_REQUEST_COMPLETED=1, UMI_LANGUAGE_REQUEST_FAILED=2, UMI_LANGUAGE_REQUEST_CANCELLED=3 } UmiLanguageRequestState;
/**
 * Represent the language request record data shared with callers of this public contract.
 */
typedef struct UmiLanguageRequestRecord { int64_t request_id; char method[128]; char document_uri[1024]; uint64_t started_at; uint64_t completed_at; UmiLanguageRequestState state; UmiStatus status; uint64_t revision; } UmiLanguageRequestRecord;
/**
 * Represent the language request ledger data shared with callers of this public contract.
 */
typedef struct UmiLanguageRequestLedger UmiLanguageRequestLedger;
/**
 * Initialise language request ledger from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_request_ledger_create(UmiLanguageRequestLedger **out_ledger);
/**
 * Release or reset state held by language request ledger so the same storage can be reused
 * safely.
 */
void umi_language_request_ledger_destroy(UmiLanguageRequestLedger *ledger);
/**
 * Provide the language request ledger begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_request_ledger_begin(UmiLanguageRequestLedger *ledger,const UmiLanguageRequestRecord *record);
/**
 * Provide the language request ledger complete operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_request_ledger_complete(UmiLanguageRequestLedger *ledger,int64_t request_id,UmiStatus status,uint64_t completed_at);
/**
 * Find language request ledger while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_request_ledger_find(const UmiLanguageRequestLedger *ledger,int64_t request_id,UmiLanguageRequestRecord *out_record);
/**
 * Find language request ledger while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_language_request_ledger_at(const UmiLanguageRequestLedger *ledger,size_t index,UmiLanguageRequestRecord *out_record);
/**
 * Return the number of records represented by language request ledger without changing
 * their state.
 */
size_t umi_language_request_ledger_count(const UmiLanguageRequestLedger *ledger);
/**
 * Provide the language request ledger pending operation used by this module and its client
 * applications.
 */
size_t umi_language_request_ledger_pending(const UmiLanguageRequestLedger *ledger);
#ifdef __cplusplus
}
#endif
#endif
