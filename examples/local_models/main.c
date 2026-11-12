/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/local_models/main.c
 *
 * PURPOSE:
 *   Expose the shared model-manager commands as a native Unicode-aware console application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/local_models/manager.h"
#include "umicom/platform/native_arguments.h"
#include <stdio.h>
static int NativeMain(int argc, char **argv)
{ return UmiModelsCommand(argc - 1, argv + 1); }
int main(int argc, char **argv)
{
    int result = 1;
    UmiStatus status = UmiNativeArgumentsDispatch(argc, argv, NativeMain, &result);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Umicom Models could not read native arguments: %s.\n", umi_status_text(status));
        return 1;
    }
    return result;
}
