/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_system/test_self_memory_host.c
 *
 * PURPOSE:
 *   Capture the test process repeatedly without inferring allocator reclamation or leak freedom from its working set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/desktop_system/self_memory.h"
int main(int argc,char **argv)
{
    assert(argc==2);UmiSelfMemorySample first;
    UmiStatus status=UmiDesktopSelfMemoryCapture(&first);
#if !defined(_WIN32) && !defined(__linux__)
    assert(status==UMI_STATUS_NOT_IMPLEMENTED);return 0;
#else
    UmiSelfMemorySample last;
    assert(status==UMI_STATUS_OK);
    unsigned count=strcmp(argv[1],"repeated")==0?256U:1U;
    assert(strcmp(argv[1],"repeated")==0 || strcmp(argv[1],"capture")==0);
    for(unsigned i=0U;i<count;++i) {
        assert(UmiDesktopSelfMemoryCapture(&last)==UMI_STATUS_OK);
        UmiSelfMemoryDifference difference;
        assert(UmiDesktopSelfMemoryCompare(&first,&last,&difference)==UMI_STATUS_OK);
        assert((difference.common_metrics & UMI_SELF_MEMORY_RESIDENT)!=0U);
    }
    return 0;
#endif
}
