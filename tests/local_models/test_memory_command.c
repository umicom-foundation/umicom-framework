/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_memory_command.c
 *
 * PURPOSE:
 *   Check native cache command arithmetic, strict decimal admission and output failure reporting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/local_models/memory_command.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;} } while(0)
static int Run(int count,const char *const *args,int expected,const char *needle)
{
    FILE *out=tmpfile(),*err=tmpfile();
    if(out==NULL || err==NULL) {
        if(out!=NULL) fclose(out);
        if(err!=NULL) fclose(err);
        return 1;
    }
    int result=UmiModelDenseKvCommand(count,args,out,err);
    rewind(out);
    char data[1024]={0};
    size_t bytes=fread(data,1,sizeof(data)-1,out);
    int valid=result==expected && (needle==NULL ? bytes==0 : strstr(data,needle)!=NULL);
    fclose(out);fclose(err);
    return valid?0:1;
}
int main(void)
{
    const char *args[]={"1920","10","2","3","2","2","4","4","2","2"};
    CHECK(Run(10,args,0,"affordable_tokens=10\n")==0);
    args[0]="1919";CHECK(Run(10,args,0,"affordable_tokens=9\n")==0);
    args[0]="0";CHECK(Run(10,args,0,"request_fits=no\n")==0);
    args[0]="18446744073709551615";CHECK(Run(10,args,0,"affordable_tokens=10\n")==0);
    const char *bad[]={"","-1","+1"," 1","1 ","1x","0x10","18446744073709551616",
        "000000000000000000000","999999999999999999999999999",NULL};
    for(size_t i=0;i<sizeof(bad)/sizeof(bad[0]);++i) {
        args[0]=bad[i];CHECK(Run(10,args,2,NULL)==0);
    }
    args[0]="1920";args[1]="0";CHECK(Run(10,args,2,NULL)==0);args[1]="10";
    args[8]="3";CHECK(Run(10,args,2,NULL)==0);
    args[8]="4294967296";CHECK(Run(10,args,2,NULL)==0);args[8]="2";
    args[3]="18446744073709551615";CHECK(Run(10,args,1,NULL)==0);args[3]="3";
    CHECK(Run(9,args,2,NULL)==0 && Run(11,args,2,NULL)==0 && Run(-1,args,2,NULL)==0);
    CHECK(Run(10,NULL,2,NULL)==0);
    const char *help[]={"--help"};
    CHECK(Run(1,help,0,"KV payload only")==0);
    CHECK(UmiModelDenseKvCommand(10,args,NULL,stderr)==2);
    CHECK(UmiModelDenseKvCommand(10,args,stdout,NULL)==2);
    CHECK(UmiModelDenseKvCommand(10,args,stdout,stdout)==2);
    /* A readable source stream has no write permission. A valid calculation
     * must still report failed delivery instead of returning success. */
    FILE *read_only=fopen(__FILE__,"rb"),*err=tmpfile();
    CHECK(read_only!=NULL && err!=NULL);
    int failed=UmiModelDenseKvCommand(10,args,read_only,err);
    fclose(read_only);fclose(err);
    CHECK(failed==1);
    return 0;
}
