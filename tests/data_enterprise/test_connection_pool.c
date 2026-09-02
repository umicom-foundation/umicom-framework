/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_connection_pool.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the connection pool enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/connection_pool.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataConnectionPool p; UmiDataConnectionSlot s; uint64_t token=0U; umi_data_connection_pool_init(&p); CHECK(umi_data_connection_slot_init(&s,"slot1",101U)==UMI_STATUS_OK); CHECK(umi_data_connection_pool_add(&p,&s)==UMI_STATUS_OK); CHECK(umi_data_connection_pool_acquire(&p,10U,&token)==UMI_STATUS_OK); CHECK(token==101U); CHECK(umi_data_connection_pool_release(&p,token,20U)==UMI_STATUS_OK); CHECK(p.leased_count==0U);
    return 0;
}
