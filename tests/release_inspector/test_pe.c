/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT */
#include "test_support.h"
int TestPeCase(const char *name)
{
    unsigned char b[TEST_PE_BYTES];TestPe(b,0,"KERNEL32.DLL",NULL);
    UmiReleasePeInfo *info=malloc(sizeof *info);CHECK(info);
    UmiStatus expected=UMI_STATUS_PARSE_ERROR;
    if(!strcmp(name,"pe.valid")) {
        TestPe(b,0,NULL,NULL);expected=UMI_STATUS_OK;
    } else if(!strcmp(name,"pe.normal")) {
        OK(UmiReleasePeInspect(b,sizeof b,info));
        CHECK(info->importCount==1U && !strcmp(info->imports[0],"kernel32.dll") && !info->delayed[0]);
        free(info);return 0;
    } else if(!strcmp(name,"pe.delay")) {
        TestPe(b,0,"KERNEL32.DLL","notes.dll");
        OK(UmiReleasePeInspect(b,sizeof b,info));
        CHECK(info->importCount==2U && !strcmp(info->imports[1],"notes.dll") && info->delayed[1]);
        free(info);return 0;
    } else if(!strcmp(name,"pe.pe32")) {
        TestPe(b,0,NULL,NULL);TestWord(b+132,0x14cU);TestWord(b+152,0x10bU);
        TestDword(b+152+108,0U);TestDword(b+152+92,0U);expected=UMI_STATUS_OK;
    } else if(!strcmp(name,"pe.truncated")) {
        for(size_t i=0U;i<sizeof b;++i) {
            CHECK(UmiReleasePeInspect(b,i,info)!=UMI_STATUS_OK);
            CHECK(info->importCount==0U && info->machine==0U);
        }
        free(info);return 0;
    } else if(!strcmp(name,"pe.signature"))b[128]='X';
    else if(!strcmp(name,"pe.offset"))TestDword(b+60,UINT32_MAX);
    else if(!strcmp(name,"pe.optional"))TestWord(b+148,12U);
    else if(!strcmp(name,"pe.sections"))TestWord(b+134,97U);
    else if(!strcmp(name,"pe.headers"))TestDword(b+212,320U);
    else if(!strcmp(name,"pe.image_size"))TestDword(b+208,4100U);
    else if(!strcmp(name,"pe.alignment"))TestDword(b+188,513U);
    else if(!strcmp(name,"pe.overlap_raw") || !strcmp(name,"pe.overlap_rva")) {
        TestWord(b+134,2U);memcpy(b+432,b+392,40U);
        if(!strcmp(name,"pe.overlap_raw")) {
            TestDword(b+432+12,8192U);TestDword(b+208,12288U);
        } else {TestDword(b+432+16,0U);TestDword(b+432+20,0U);}
    } else if(!strcmp(name,"pe.virtual_tail")) {
        TestDword(b+392+8,4096U);TestDword(b+524,0x1800U);
    } else if(!strcmp(name,"pe.bad_import_name")) {
        memcpy(b+832,"../evil.dll",12U);
    } else if(!strcmp(name,"pe.no_terminator"))TestDword(b+276,20U);
    else if(!strcmp(name,"pe.delay_legacy")) {
        TestPe(b,0,NULL,"notes.dll");TestDword(b+1280,0U);expected=UMI_STATUS_UNAVAILABLE;
    } else if(!strcmp(name,"pe.unknown_machine")) {TestWord(b+132,0xaa64U);expected=UMI_STATUS_OK;}
    else if(!strcmp(name,"pe.flags"))TestWord(b+150,0U);
    else if(!strcmp(name,"pe.entry_outside"))TestDword(b+168,0x1900U);
    else if(!strcmp(name,"pe.directory_overflow"))TestDword(b+272,UINT32_MAX);
    else if(!strcmp(name,"pe.directory_count"))TestDword(b+260,17U);
    else if(!strcmp(name,"pe.missing_iat"))TestDword(b+528,0U);
    else if(!strcmp(name,"pe.null_output")) {
        CHECK(UmiReleasePeInspect(b,sizeof b,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiReleasePeInspect(NULL,sizeof b,info)==UMI_STATUS_INVALID_ARGUMENT);
        free(info);return 0;
    } else if(!strcmp(name,"pe.mutations")) {
        uint32_t random=0x912ac7U;unsigned char original[TEST_PE_BYTES];memcpy(original,b,sizeof b);
        for(size_t n=0U;n<10000U;++n) {
            memcpy(b,original,sizeof b);random=random*1664525U+1013904223U;
            size_t at=random%sizeof b;random=random*1664525U+1013904223U;
            b[at]^=(unsigned char)(1U<<(random%8U));
            UmiStatus status=UmiReleasePeInspect(b,sizeof b,info);
            CHECK(status==UMI_STATUS_OK || info->importCount==0U);
        }
        free(info);return 0;
    } else if(!strcmp(name,"pe.name_capacity")) {
        memset(b+832,'a',256U);expected=UMI_STATUS_CAPACITY_EXCEEDED;
    } else {free(info);return 1;}
    CHECK(UmiReleasePeInspect(b,sizeof b,info)==expected);
    if(expected!=UMI_STATUS_OK)CHECK(info->importCount==0U && info->machine==0U);
    free(info);return 0;
}
int TestLinkedPe(const char *path,int delayed)
{
    UmiSetupReport report={0};char *data=NULL;size_t length=0U;
    OK(ScRead(path,UMI_RELEASE_MAX_PE_BYTES,&data,&length,&report));
    UmiReleasePeInfo *info=malloc(sizeof *info);CHECK(info);
    OK(UmiReleasePeInspect(data,length,info));
    CHECK(info->machine==0x8664U && info->optionalMagic==0x20bU && info->importCount==1U);
    CHECK(!strcmp(info->imports[0],"umicom-practice-storage.dll") && info->delayed[0]==delayed);
    free(info);free(data);return 0;
}
