/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_watch_expression.c
 *
 * PURPOSE:
 *   Implement the test debug watch expression behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework debugger watch-expression test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/debug/watch_expression.h"
int main(void){UmiDebugWatchSnapshot w={0};UmiDebugWatchEvaluation e;strcpy(w.id,"watch.1");strcpy(w.expression,"counter");w.enabled=1;assert(umi_debug_watch_evaluation_from_snapshot(&w,7,&e)==UMI_STATUS_OK);assert(e.frame_id==7&&strcmp(e.context,"watch")==0);return 0;}
