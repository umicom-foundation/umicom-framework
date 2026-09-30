/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/release_inspector/pe_fixture.c
 * PURPOSE:
 *   Synthetic PE metadata, not a functioning Windows executable or DLL.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
 * Synthetic PE metadata, not a functioning Windows executable or DLL. */
#include "test_support.h"
void TestWord(unsigned char *p,uint16_t v){p[0]=(unsigned char)v;p[1]=(unsigned char)(v>>8U);}
void TestDword(unsigned char *p,uint32_t v){for(unsigned i=0U;i<4U;++i)p[i]=(unsigned char)(v>>(i*8U));}
void TestPe(unsigned char b[TEST_PE_BYTES],int dll,const char *normal,const char *delay)
{
    memset(b,0,TEST_PE_BYTES);b[0]='M';b[1]='Z';TestDword(b+60,128U);
    memcpy(b+128,"PE\0\0",4);TestWord(b+132,0x8664U);TestWord(b+134,1U);
    TestWord(b+148,240U);TestWord(b+150,(uint16_t)(dll?0x2022U:0x22U));
    unsigned char *o=b+152;TestWord(o,0x20bU);TestDword(o+16,0x1180U);
    TestDword(o+24,0x40000000U);TestDword(o+28,1U);
    TestDword(o+32,4096U);TestDword(o+36,512U);TestDword(o+56,8192U);
    TestDword(o+60,512U);TestWord(o+68,3U);TestDword(o+108,16U);
    unsigned char *section=b+392;memcpy(section,".text",5U);
    TestDword(section+8,1536U);TestDword(section+12,4096U);
    TestDword(section+16,1536U);TestDword(section+20,512U);
    TestDword(section+36,0x60000020U);
    if(normal) {
        TestDword(o+120,0x1000U);TestDword(o+124,40U);
        TestDword(b+512,0x1100U);TestDword(b+524,0x1140U);TestDword(b+528,0x1110U);
        (void)snprintf((char *)b+832,256U,"%s",normal);
    }
    if(delay) {
        TestDword(o+216,0x1300U);TestDword(o+220,64U);
        TestDword(b+1280,1U);TestDword(b+1284,0x1400U);
        TestDword(b+1292,0x14f0U);TestDword(b+1296,0x14e0U);
        (void)snprintf((char *)b+1536,200U,"%s",delay);
    }
}
