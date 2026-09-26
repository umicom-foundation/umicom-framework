/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Canonical process ownership stays in platform/process.c. QEMU and GRUB are
 * external native tools; all arguments remain separate from the executable. */

#include "umicom/setup_centre/execution.h"
UmiStatus UmiSetupExecuteProcess(const char *program,const char *const *args,
size_t count,const char *directory,void *opaque,int *exitCode)
{

    UmiSetupProcessContext *context=opaque;

    if(!context||!context->result||!exitCode)return UMI_STATUS_INVALID_ARGUMENT;

    UmiProcessRequest request= {
        0
    };

    request.program=program;
    request.arguments=args;
    request.argument_count=count;

    request.working_directory=directory;
    request.capture_stdout=1;
    request.capture_stderr=1;

    request.timeout_ms=context->timeoutMs;
    request.poll_interval_ms=25;

    request.cancellation=context->cancellation;
    request.window_mode=UMI_PROCESS_WINDOW_HIDDEN;

    UmiStatus status=UmiProcessExecuteWithLifetime(&request,UMI_PROCESS_LIFETIME_TREE,NULL,NULL,NULL,context->result);

    *exitCode=context->result->exit_code;

    if(context->result->cancelled)return UMI_STATUS_CANCELLED;

    if(context->result->timed_out)return UMI_STATUS_TIMEOUT;

    if(status==UMI_STATUS_OK&&!context->result->launched)return UMI_STATUS_IO_ERROR;

    return status;

}
