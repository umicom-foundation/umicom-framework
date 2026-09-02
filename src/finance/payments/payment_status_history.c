/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_status_history.c
 *
 * PURPOSE:
 *   Implement monotonic payment status history and duplicate/regression protection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_status_history.h"
#include <string.h>
/*
 * Initialise payments payment status history from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_payments_payment_status_history_init(UmiPaymentsPaymentStatusHistory *value,const char *id,const char *payment_id){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(value,0,sizeof *value); UmiStatus s=umi_payments_id_assign(&value->id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; return umi_payments_id_assign(&value->payment_id,payment_id); }
/*
 * Check that payments payment status history satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_status_history_valid(const UmiPaymentsPaymentStatusHistory *value){ return value!=NULL && umi_financial_id_is_valid(&value->id) && umi_financial_id_is_valid(&value->payment_id) && value->count<=UMI_PAYMENTS_MAX_ITEMS; }
/*
 * Add payments payment status history only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_payments_payment_status_history_append(UmiPaymentsPaymentStatusHistory *value,UmiPaymentsStatus status,int64_t epoch_millis){ /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!umi_payments_payment_status_history_valid(value)||epoch_millis<0||status<UMI_PAYMENTS_CREATED||status>UMI_PAYMENTS_REJECTED)return UMI_STATUS_INVALID_ARGUMENT; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value->count>=UMI_PAYMENTS_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value->count>0U){ const UmiPaymentsPaymentStatusPoint *p=&value->items[value->count-1U]; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(epoch_millis<p->epoch_millis)return UMI_STATUS_INVALID_STATE; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if((int)status<(int)p->status && status!=UMI_PAYMENTS_RETURNED && status!=UMI_PAYMENTS_REJECTED)return UMI_STATUS_INVALID_STATE; } value->items[value->count++]=(UmiPaymentsPaymentStatusPoint){status,epoch_millis}; return UMI_STATUS_OK; }
