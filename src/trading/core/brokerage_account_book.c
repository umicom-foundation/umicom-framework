/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/brokerage_account_book.c
 *
 * PURPOSE:
 *   Maintain bounded brokerage account state owned by the reusable trading core.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/brokerage_account_book.h"

#include <string.h>
/*
 * Initialise trading brokerage account book from caller-provided values so later
 * operations receive a known state.
 */
void umi_trading_brokerage_account_book_init(UmiTradingBrokerageAccountBook *book){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(book!=NULL)memset(book,0,sizeof *book);}
/*
 * Find trading brokerage account book while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiTradingBrokerageAccount *umi_trading_brokerage_account_book_find(const UmiTradingBrokerageAccountBook *book,const UmiFinancialId *account_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(book==NULL||account_id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<book->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_id_equal(&book->items[i].account_id,account_id))return &book->items[i];return NULL;}
/*
 * Add trading brokerage account book only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_trading_brokerage_account_book_add(UmiTradingBrokerageAccountBook *book,const UmiTradingBrokerageAccount *account){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(book==NULL||!umi_trading_brokerage_account_valid(account))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_brokerage_account_book_find(book,&account->account_id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(book->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;book->items[book->count++]=*account;return UMI_STATUS_OK;}
