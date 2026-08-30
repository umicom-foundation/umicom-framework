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
UmiStatus umi_payments_payment_status_history_init(UmiPaymentsPaymentStatusHistory *value,const char *id,const char *payment_id){ if(value==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(value,0,sizeof *value); UmiStatus s=umi_payments_id_assign(&value->id,id); if(s!=UMI_STATUS_OK)return s; return umi_payments_id_assign(&value->payment_id,payment_id); }
bool umi_payments_payment_status_history_valid(const UmiPaymentsPaymentStatusHistory *value){ return value!=NULL && umi_financial_id_is_valid(&value->id) && umi_financial_id_is_valid(&value->payment_id) && value->count<=UMI_PAYMENTS_MAX_ITEMS; }
UmiStatus umi_payments_payment_status_history_append(UmiPaymentsPaymentStatusHistory *value,UmiPaymentsStatus status,int64_t epoch_millis){ if(!umi_payments_payment_status_history_valid(value)||epoch_millis<0||status<UMI_PAYMENTS_CREATED||status>UMI_PAYMENTS_REJECTED)return UMI_STATUS_INVALID_ARGUMENT; if(value->count>=UMI_PAYMENTS_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; if(value->count>0U){ const UmiPaymentsPaymentStatusPoint *p=&value->items[value->count-1U]; if(epoch_millis<p->epoch_millis)return UMI_STATUS_INVALID_STATE; if((int)status<(int)p->status && status!=UMI_PAYMENTS_RETURNED && status!=UMI_PAYMENTS_REJECTED)return UMI_STATUS_INVALID_STATE; } value->items[value->count++]=(UmiPaymentsPaymentStatusPoint){status,epoch_millis}; return UMI_STATUS_OK; }
