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
void umi_trading_brokerage_account_book_init(UmiTradingBrokerageAccountBook *book){if(book!=NULL)memset(book,0,sizeof *book);}
const UmiTradingBrokerageAccount *umi_trading_brokerage_account_book_find(const UmiTradingBrokerageAccountBook *book,const UmiFinancialId *account_id){if(book==NULL||account_id==NULL)return NULL;for(size_t i=0;i<book->count;i++)if(umi_trading_core_id_equal(&book->items[i].account_id,account_id))return &book->items[i];return NULL;}
UmiStatus umi_trading_brokerage_account_book_add(UmiTradingBrokerageAccountBook *book,const UmiTradingBrokerageAccount *account){if(book==NULL||!umi_trading_brokerage_account_valid(account))return UMI_STATUS_INVALID_ARGUMENT;if(umi_trading_brokerage_account_book_find(book,&account->account_id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(book->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;book->items[book->count++]=*account;return UMI_STATUS_OK;}
