/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/position_key.c
 *
 * PURPOSE:
 *   Implement canonical position keys.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/position_key.h"

/* Initialize key. */ UmiStatus umi_position_key_init(UmiPositionKey *k,const char *book,const char *portfolio,const char *product){UmiStatus st;if(k==NULL)return UMI_STATUS_INVALID_ARGUMENT;st=umi_financial_id_assign(&k->book_id,book);if(st!=UMI_STATUS_OK)return st;st=umi_financial_id_assign(&k->portfolio_id,portfolio);if(st!=UMI_STATUS_OK)return st;return umi_product_id_set(&k->product_id,product);}
/* Compare keys. */ bool umi_position_key_equal(const UmiPositionKey *a,const UmiPositionKey *b){return a!=NULL&&b!=NULL&&umi_financial_id_compare(&a->book_id,&b->book_id)==0&&umi_financial_id_compare(&a->portfolio_id,&b->portfolio_id)==0&&umi_financial_id_compare(&a->product_id.id,&b->product_id.id)==0;}
