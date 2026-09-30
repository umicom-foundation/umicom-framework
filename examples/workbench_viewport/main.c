/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/workbench_viewport/main.c
 * PURPOSE:
 *   Demonstrate the reusable workbench viewport contract.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "practice_layout.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    UmiWorkbenchLayoutDocument *d=calloc(1,sizeof *d);
    UmiWorkbenchViewportPlan *p=calloc(1,sizeof *p);
    if(!d||!p){free(d);free(p);return 1;}
    UmiViewportLessonCreate(d);UmiWorkbenchViewportDiagnostic error;
    UmiStatus status=UmiWorkbenchViewportBuild(d,(UmiWorkbenchLayoutRect){0,0,1000,600},NULL,p,&error);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"%s\n",error.message);free(p);free(d);return 1;}
    for(size_t i=0;i<p->focusCount;++i){size_t n=p->focusOrder[i];UmiWorkbenchLayoutRect r=p->slots[n].bounds;
        printf("%s: x=%" PRId32 " y=%" PRId32 " width=%" PRId32 " height=%" PRId32 "\n",d->nodes[n].node_id,r.x,r.y,r.width,r.height);
    }
    puts("Practice complete. One canonical layout; no files, widgets or databases were opened.");
    free(p);free(d);return 0;
}
