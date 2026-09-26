/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Bind the setup provider port to the existing canonical process runner. */

#ifndef UMICOM_SETUP_CENTRE_EXECUTION_H
#define UMICOM_SETUP_CENTRE_EXECUTION_H
#include "umicom/setup_centre/virtual_machine.h"
#include "umicom/platform/process.h"
#ifdef __cplusplus
extern "C" {

#endif
    typedef struct UmiSetupProcessContext {

        const UmiCancellationToken *cancellation;

        uint32_t timeoutMs;

        UmiProcessResult *result;

    }
    UmiSetupProcessContext;

    /** Call on a worker. The existing process-tree lifetime contract is retained.
     * This adapter does not implement a second process engine or invoke a shell. */

    UmiStatus UmiSetupExecuteProcess(const char *program,const char *const *arguments,
    size_t count,const char *directory,void *context,int *exitCode);

    int UmiSetupMainWithExecutor(int argc,char **argv,UmiSetupExecute execute,void *context);

#ifdef __cplusplus
}

#endif
#endif
