/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_data_server_visit_snapshot.c
 *
 * PURPOSE:
 *   Implement the test data server visit snapshot behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/data/data_server.h"
/*
 * Exercise count record and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus count_record(const char *key,const char *value,void *user){size_t *count=(size_t *)user;assert(key[0]!='\0');assert(value!=NULL);*count+=1U;return UMI_STATUS_OK;}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiDataServer *s=NULL;UmiDataServerSnapshot snap;size_t count=0U;assert(umi_data_server_create_memory(&s)==UMI_STATUS_OK);assert(umi_data_server_set(s,"alpha","one")==UMI_STATUS_OK);assert(umi_data_server_set(s,"beta","two")==UMI_STATUS_OK);assert(umi_data_server_visit(s,count_record,&count)==UMI_STATUS_OK&&count==2U);assert(umi_data_server_snapshot(s,&snap)==UMI_STATUS_OK);assert(snap.record_count==2U&&strcmp(snap.backend_name,"memory")==0);umi_data_server_destroy(s);return 0;}
