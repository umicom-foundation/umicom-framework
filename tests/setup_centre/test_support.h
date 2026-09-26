/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_SETUP_TEST_SUPPORT_H
#define UMICOM_SETUP_TEST_SUPPORT_H
#include "internal.h"
#include <inttypes.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#c);return 1;}}while(0)
#define OK(c) CHECK((c)==UMI_STATUS_OK)
typedef struct Fixture {
    char root[4096],bundle[4096],destination[4096],bank[4096],studio[4096],runtime[4096],list[4096];
}
Fixture;

int FixtureCreate(Fixture *fixture);

int TestPaths(const char *name);

int TestCatalogue(const char *name);

int TestInstall(const char *name);

int TestVm(const char *name);

int TestMedia(const char *name);

extern const char *TestProgram;

#endif
