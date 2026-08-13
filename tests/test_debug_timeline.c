/* Umicom Framework debugger timeline test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/debug/timeline.h"
int main(void){UmiDebugTimeline*t=NULL;UmiDebugEventSnapshot e={0},o;strcpy(e.id,"1");strcpy(e.kind,"stopped");assert(umi_debug_timeline_create(&t)==UMI_STATUS_OK);assert(umi_debug_timeline_append(t,&e)==UMI_STATUS_OK);assert(umi_debug_timeline_at(t,0U,&o)==UMI_STATUS_OK&&strcmp(o.kind,"stopped")==0);umi_debug_timeline_destroy(t);return 0;}
