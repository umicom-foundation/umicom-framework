/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT */
#include "test_support.h"
int main(int argc,char **argv)
{
    if(argc==5 && !strcmp(argv[1],"pack-linked"))return TestPackLinkedPe(argv[2],argv[3],argv[4]);
    if(argc==4 && !strcmp(argv[1],"linked-pe"))return TestLinkedPe(argv[2],!strcmp(argv[3],"delay"));
    if(argc!=2)return 2;
    if(!strncmp(argv[1],"pe.",3U))return TestPeCase(argv[1]);
    if(!strncmp(argv[1],"paths.",6U)||!strncmp(argv[1],"catalogue.",10U))return TestPathsCase(argv[1]);
    return TestFilesCase(argv[1]);
}
