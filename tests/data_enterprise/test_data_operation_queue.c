/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_operation_queue.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data operation queue enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_operation_queue.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataOperationQueue q; UmiDataOperation a,b,out; umi_data_data_operation_queue_init(&q); CHECK(umi_data_data_operation_init(&a,"a","s","query",10U,1U)==UMI_STATUS_OK); CHECK(umi_data_data_operation_init(&b,"b","s","migration",20U,9U)==UMI_STATUS_OK); CHECK(umi_data_data_operation_queue_push(&q,&a)==UMI_STATUS_OK); CHECK(umi_data_data_operation_queue_push(&q,&b)==UMI_STATUS_OK); CHECK(umi_data_data_operation_queue_pop(&q,&out)==UMI_STATUS_OK); CHECK(strcmp(out.operation_id,"b")==0);
    return 0;
}
