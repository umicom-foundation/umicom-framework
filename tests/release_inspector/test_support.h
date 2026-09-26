/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_RELEASE_TEST_SUPPORT_H
#define UMICOM_RELEASE_TEST_SUPPORT_H
#include "umicom/release_inspector/inspection.h"
#include "internal.h"
#define CHECK(x) do { if(!(x)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);return 1;} }while(0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
#define TEST_PE_BYTES 2048U
void TestWord(unsigned char *p,uint16_t value);
void TestDword(unsigned char *p,uint32_t value);
void TestPe(unsigned char bytes[TEST_PE_BYTES],int dll,const char *normal,const char *delay);
int TestPeCase(const char *name);
int TestFilesCase(const char *name);
int TestPathsCase(const char *name);
int TestLinkedPe(const char *path,int delayed);
int TestPackLinkedPe(const char *app, const char *dll, const char *bootstrap);
#endif
