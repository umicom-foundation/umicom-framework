/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_transaction.c
 *
 * PURPOSE:
 *   Validate track atomic designer transactions and mutation counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/transaction.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadDesignerTransaction item;CHECK(umi_rad_transaction_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_transaction_is_valid(&item));return 0;}
