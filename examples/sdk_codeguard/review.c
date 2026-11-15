/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_codeguard/review.c
 *
 * PURPOSE:
 *   Demonstrate current-context publication and cancellation through the installed review controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/codeguard.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

int main(int argc,char **argv)
{
    int cancel=0,stale=0;
    if(argc==3) {
        cancel=!strcmp(argv[2],"--cancel-before-publication");
        stale=!strcmp(argv[2],"--stale-before-publication");
    }
    if(argc<2 || argc>3 || (argc==3 && !cancel && !stale)) {
        fputs("Usage: umicom-sdk-codeguard-review <absolute-root> [--cancel-before-publication|--stale-before-publication]\n",stderr);
        return 2;
    }
    UmiCodeGuardReview *review=NULL;
    UmiStatus status=UmiCodeGuardReviewCreate(argv[1],&review);
    if(status!=UMI_STATUS_OK) {
        fprintf(stderr,"Cannot create review; use an absolute workspace root (status %d).\n",(int)status);
        return 2;
    }
    UmiTaskQueueConfig config={.worker_count=1U,.capacity=1U};
    UmiTaskQueue *queue=NULL;
    status=umi_task_queue_create(&config,&queue);
    if(status!=UMI_STATUS_OK) {
        (void)UmiCodeGuardReviewDestroy(&review);
        return 2;
    }
    UmiCodeGuardProfile profile=umi_codeguard_profile_security();
    uint64_t request=0U;
    status=UmiCodeGuardReviewBegin(review,queue,&profile,1U,&request);
    /* The demonstration deliberately waits for execution before changing the
     * publication decision. A GUI instead polls on its existing event loop,
     * preserving its shared queue and avoiding this blocking private shutdown. */
    UmiStatus stopped=umi_task_queue_shutdown(queue,0);
    if(stopped!=UMI_STATUS_OK) {
        fprintf(stderr,"Worker shutdown failed (status %d); cannot safely release review.\n",(int)stopped);
        return 3;
    }
    if(status==UMI_STATUS_OK && cancel) status=UmiCodeGuardReviewCancel(review);
    UmiCodeGuardReviewSnapshot snapshot={0};
    uint64_t current_context=stale?0U:1U;
    if(status==UMI_STATUS_OK)status=UmiCodeGuardReviewPoll(review,current_context,&snapshot);
    int exit_code=3;
    if(status!=UMI_STATUS_OK) {
        fprintf(stderr,"Review operation failed (status %d).\n",(int)status);
    } else {
        fprintf(stderr,"Request %" PRIu64 ": task=%d operation=%d publication=%d\n",
            request,(int)snapshot.state,(int)snapshot.operation_status,(int)snapshot.status);
        if(snapshot.status==UMI_STATUS_CANCELLED)exit_code=4;
        else if(snapshot.stale)exit_code=5;
        else if(snapshot.status==UMI_STATUS_OK && snapshot.has_report) {
            const UmiCodeGuardResult *report=UmiCodeGuardReviewResult(review,current_context);
            status=umi_codeguard_report_write(stdout,UMI_CODEGUARD_REPORT_TEXT,report);
            if(status==UMI_STATUS_OK && fflush(stdout)==0)
                exit_code=umi_codeguard_summary_failed(&snapshot.summary,profile.fail_on)?1:0;
        }
    }
    status=UmiCodeGuardReviewDestroy(&review);
    umi_task_queue_destroy(queue);
    if(status!=UMI_STATUS_OK) {
        fprintf(stderr,"Cannot release completed review (status %d).\n",(int)status);
        return 3;
    }
    return exit_code;
}
