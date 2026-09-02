/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_transaction.c
 *
 * PURPOSE:
 *   Validate track atomic designer transactions and mutation counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/transaction.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDesignerTransaction item;CHECK(umi_rad_transaction_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_transaction_is_valid(&item));return 0;}
