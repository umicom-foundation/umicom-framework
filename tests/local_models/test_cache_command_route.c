/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_cache_command_route.c
 *
 * PURPOSE:
 *   Verify the model command routes ten-value cache plans while preserving session ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/local_models/manager.h"
#include <stdio.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(void)
{
    char *args[]={"cache-plan","1919","10","2","3","2","2","4","4","2","2"};
    UmiModelSession *session=NULL;
    CHECK(UmiModelsCommandWithSession(11,args,&session)==0 && session==NULL);
    args[1]="-1";
    CHECK(UmiModelsCommandWithSession(11,args,&session)==2 && session==NULL);
    CHECK(UmiModelsCommandWithSession(10,args,&session)==2 && session==NULL);
    /* An occupied slot must be rejected before command dispatch, regardless
     * of the operation's offline nature. The sentinel is never dereferenced. */
    unsigned sentinel=0;
    session=(UmiModelSession *)(void *)&sentinel;
    CHECK(UmiModelsCommandWithSession(11,args,&session)!=0);
    CHECK(session==(UmiModelSession *)(void *)&sentinel);
    return 0;
}
