/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Explicit synthetic fixtures. These headers and transcripts are NOT kernels,
 * guest boots or Windows loader tests. Real boot tests use the public tool. */
#ifndef UMICOM_OS_IMAGE_TEST_FIXTURES_H
#define UMICOM_OS_IMAGE_TEST_FIXTURES_H
#include "internal.h"
#define TEST_ARCHIVE_TEXT "Synthetic source archive for native pipeline tests.\n"
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);return 1; } } while(0)
static inline void TestPut16(unsigned char *p,unsigned v){p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8);}
static inline void TestPut64(unsigned char *p,uint64_t v){for(unsigned i=0;i<8;++i)p[i]=(unsigned char)(v>>(i*8U));}
static inline void TestElf(unsigned char p[4096],UmiOsImageArch a)
{
    memset(p,0,4096);memcpy(p,"\177ELF\2\1\1",7);TestPut16(p+16,2);TestPut16(p+18,a==UMI_OS_IMAGE_RISCV64?243U:62U);
    OiPut32(p+20,1);TestPut64(p+24,UINT64_C(0x400100));TestPut64(p+32,64);TestPut16(p+52,64);TestPut16(p+54,56);TestPut16(p+56,2);
    OiPut32(p+64,1);OiPut32(p+68,5);TestPut64(p+72,0);TestPut64(p+80,UINT64_C(0x400000));TestPut64(p+96,4096);TestPut64(p+104,4096);TestPut64(p+112,4096);
    OiPut32(p+120,UINT32_C(0x6474e551));OiPut32(p+124,6);
}
static inline void TestKernel(unsigned char p[8192],UmiOsImageArch a)
{
    memset(p,0,8192);
    if(a==UMI_OS_IMAGE_X86_64){p[0x1f1]=4;p[0x1fe]=0x55;p[0x1ff]=0xaa;memcpy(p+0x202,"HdrS",4);TestPut16(p+0x206,0x20c);TestPut16(p+0x236,1);}
    else{memcpy(p+48,"RISCV\0\0\0",8);memcpy(p+56,"RSC\x05",4);TestPut64(p+16,8192);}
}
static inline void TestTranscript(OiText *t,const char *source,int recovery)
{
    OiPrint(t,"[0.000] Linux fixture: not a real guest\nUMICOM_INIT pid=1\n");
    if(!recovery)OiPrint(t,"UMICOM_SERVICE start=platform-check\nUMICOM_SERVICE end=platform-check outcome=none exit=0 signal=0\nUMICOM_SERVICE start=framework-probe\nUMICOM_SERVICE end=framework-probe outcome=none exit=0 signal=0\n");
    OiPrint(t,"UMICOM_REPORT_BEGIN\nUMICOM_BOOT_REPORT 1\nmode=%s\nstate=%s\nplanned=2\ncompleted=%u\nreason=%s\nsource=%s\nUMICOM_REPORT_END\nUMICOM_SHUTDOWN\n[1.000] reboot: Power down\n",
        recovery?"recovery":"normal",recovery?"recovery":"ready",recovery?0U:2U,recovery?"requested":"none",source);
}
static inline const char *TestSource(void){return "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";}
#endif
