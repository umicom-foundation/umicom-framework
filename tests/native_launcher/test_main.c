/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
 * CTest and this runner read the same case catalogue; names cannot drift. */
#include "test_support.h"
const char *TestBinary=NULL;
#define CASE(name,function) int function(void);
#include "cases_common.inc"
#ifdef __linux__
#include "cases_linux.inc"
#endif
#undef CASE
typedef struct TestCase {const char *name;int (*run)(void);} TestCase;
#define CASE(name,function) {name,function},
static const TestCase CASES[]={
#include "cases_common.inc"
#ifdef __linux__
#include "cases_linux.inc"
#endif
};
#undef CASE
int main(int argc,char **argv)
{
    if(argc<2)return 2;
    if(argc>2)TestBinary=argv[2];
    for(size_t i=0U;i<sizeof CASES/sizeof CASES[0];++i) {
        if(strcmp(argv[1],"--list")==0){puts(CASES[i].name);continue;}
        if(strcmp(argv[1],CASES[i].name)==0){int result=CASES[i].run();printf("%s: %s\n",CASES[i].name,result==0?"passed":"FAILED");return result;}
    }
    return strcmp(argv[1],"--list")==0?0:2;
}
