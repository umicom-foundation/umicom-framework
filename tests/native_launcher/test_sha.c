/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
 * Standard SHA-256 vectors and incremental lifecycle/size-boundary tests. */
#include "test_support.h"
int TestShaEmpty(void){char h[65];CHECK(UmiNativeSha256Buffer(NULL,0U,h)==UMI_STATUS_OK);CHECK(strcmp(h,"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")==0);return 0;}
int TestShaAbc(void){char h[65];CHECK(UmiNativeSha256Buffer("abc",3U,h)==UMI_STATUS_OK);CHECK(strcmp(h,"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")==0);return 0;}
int TestShaLong(void)
{
    const char *value="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";char h[65];
    CHECK(UmiNativeSha256Buffer(value,strlen(value),h)==UMI_STATUS_OK);
    CHECK(strcmp(h,"248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1")==0);return 0;
}
int TestShaMillion(void)
{
    UmiNativeSha256 c;UmiNativeSha256Init(&c);unsigned char block[1000];memset(block,'a',sizeof block);
    for(unsigned i=0U;i<1000U;++i)CHECK(UmiNativeSha256Update(&c,block,sizeof block)==UMI_STATUS_OK);
    unsigned char digest[32];char h[65];CHECK(UmiNativeSha256Final(&c,digest)==UMI_STATUS_OK);UmiNativeSha256Hex(digest,h);
    CHECK(strcmp(h,"cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0")==0);return 0;
}
int TestShaSegments(void)
{
    unsigned char input[4097];for(size_t i=0U;i<sizeof input;++i)input[i]=(unsigned char)(i*17U+i/13U);
    char expected[65];CHECK(UmiNativeSha256Buffer(input,sizeof input,expected)==UMI_STATUS_OK);
    for(size_t step=1U;step<=129U;++step) {
        UmiNativeSha256 c;UmiNativeSha256Init(&c);
        for(size_t i=0U;i<sizeof input;){size_t n=sizeof input-i;if(n>step)n=step;CHECK(UmiNativeSha256Update(&c,input+i,n)==UMI_STATUS_OK);i+=n;}
        unsigned char digest[32];char h[65];CHECK(UmiNativeSha256Final(&c,digest)==UMI_STATUS_OK);UmiNativeSha256Hex(digest,h);CHECK(strcmp(h,expected)==0);
    }return 0;
}
int TestShaLifecycle(void)
{
    UmiNativeSha256 c;UmiNativeSha256Init(&c);unsigned char digest[32];
    CHECK(UmiNativeSha256Update(&c,NULL,1U)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiNativeSha256Final(&c,digest)==UMI_STATUS_OK);
    CHECK(UmiNativeSha256Final(&c,digest)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiNativeSha256Update(&c,"a",1U)==UMI_STATUS_INVALID_STATE);
    UmiNativeSha256Init(&c);CHECK(UmiNativeSha256Update(&c,"a",1U)==UMI_STATUS_OK);return 0;
}
int TestShaOverflow(void)
{
    UmiNativeSha256 c;UmiNativeSha256Init(&c);c.totalBytes=UINT64_MAX/8U;
    CHECK(UmiNativeSha256Update(&c,"a",1U)==UMI_STATUS_CAPACITY_EXCEEDED);CHECK(c.totalBytes==UINT64_MAX/8U);
    c.used=64U;CHECK(UmiNativeSha256Update(&c,NULL,0U)==UMI_STATUS_INVALID_STATE);return 0;
}
int TestShaNulls(void)
{
    unsigned char digest[32];char h[65];UmiNativeSha256Init(NULL);
    CHECK(UmiNativeSha256Update(NULL,"a",1U)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiNativeSha256Final(NULL,digest)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiNativeSha256Buffer("a",1U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiNativeSha256Buffer(NULL,1U,h)==UMI_STATUS_INVALID_ARGUMENT);CHECK(h[0]=='\0');return 0;
}
