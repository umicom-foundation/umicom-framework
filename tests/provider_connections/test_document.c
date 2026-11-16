/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connections/test_document.c
 *
 * PURPOSE:
 *   Verify independent connection-document bytes, revision boundaries and refusal preservation without storage or network effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/provider_connections/document.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;}}while(0)
static const char golden[]="umicom-provider-connection\n1\n7\nconnection\n2\n30000\n1\n636f6d66792d6578616d706c65\n636f6d66797569\n436f6d66795549206c6f63616c\n687474703a2f2f3132372e302e302e313a38313838\n\n\n";
static UmiProviderConnection Connection(void)
{
    UmiProviderConnection c={0};strcpy(c.id,"comfy-example");strcpy(c.provider_id,"comfyui");
    strcpy(c.label,"ComfyUI local");strcpy(c.endpoint,"http://127.0.0.1:8188");
    c.route=UMI_PROVIDER_CONNECTION_LOOPBACK;c.timeout_ms=30000;c.enabled=true;return c;
}
static int Refused(const void *bytes,size_t length,UmiStatus expected)
{
    UmiProviderConnection out;unsigned char before[sizeof out];uint64_t revision=99U;
    memset(&out,0xa5,sizeof out);memcpy(before,&out,sizeof out);
    CHECK(UmiProviderConnectionDocumentImport(bytes,length,&out,&revision)==expected);
    CHECK(!memcmp(before,&out,sizeof out) && revision==99U);return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    UmiProviderConnection c=Connection(),decoded;uint64_t revision=7U,actual=0U;
    char document[UMI_PROVIDER_CONNECTION_DOCUMENT_MAX_BYTES],before[sizeof document];size_t length=79U;
    memset(document,0xa5,sizeof document);memcpy(before,document,sizeof before);
    if(!strcmp(mode,"golden")){
        CHECK(UmiProviderConnectionDocumentExport(&c,7U,document,sizeof document,&length)==UMI_STATUS_OK);
        CHECK(length==sizeof golden-1U && !memcmp(document,golden,sizeof golden));
        CHECK(UmiProviderConnectionDocumentImport(golden,sizeof golden-1U,&decoded,&actual)==UMI_STATUS_OK);
        CHECK(actual==7U && !strcmp(decoded.endpoint,c.endpoint) && !strcmp(decoded.label,c.label));
    }else if(!strcmp(mode,"maximum-revision") || !strcmp(mode,"unicode") || !strcmp(mode,"disabled") ||
             !strcmp(mode,"remote-reference") || !strcmp(mode,"fingerprint") || !strcmp(mode,"trailing-storage")){
        if(!strcmp(mode,"maximum-revision"))revision=UINT64_MAX;
        if(!strcmp(mode,"unicode"))strcpy(c.label,"本地模型");
        if(!strcmp(mode,"disabled"))c.enabled=false;
        if(!strcmp(mode,"remote-reference")){
            c.route=UMI_PROVIDER_CONNECTION_HTTPS;strcpy(c.provider_id,"openai");
            strcpy(c.endpoint,"https://api.openai.com/v1/responses");strcpy(c.model,"selected-model");strcpy(c.secret_reference,"vault:personal-key");
        }
        if(!strcmp(mode,"trailing-storage")){
            size_t n=strlen(c.label);memset(c.label+n+1U,0xa5,sizeof c.label-n-1U);
        }
        CHECK(UmiProviderConnectionDocumentExport(&c,revision,document,sizeof document,&length)==UMI_STATUS_OK);
        CHECK(UmiProviderConnectionDocumentImport(document,length,&decoded,&actual)==UMI_STATUS_OK && actual==revision);
        char again[sizeof document];size_t again_length=0U;
        CHECK(UmiProviderConnectionDocumentExport(&decoded,actual,again,sizeof again,&again_length)==UMI_STATUS_OK);
        CHECK(length==again_length && !memcmp(document,again,length+1U));
        unsigned char a[UMI_SHA256_BYTES],b[UMI_SHA256_BYTES];
        CHECK(UmiProviderConnectionFingerprint(&c,a)==UMI_STATUS_OK && UmiProviderConnectionFingerprint(&decoded,b)==UMI_STATUS_OK);
        CHECK(!memcmp(a,b,sizeof a));
        if(!strcmp(mode,"remote-reference"))CHECK(!strcmp(decoded.secret_reference,"vault:personal-key"));
    }else if(!strcmp(mode,"exact-capacity")){
        CHECK(UmiProviderConnectionDocumentExport(&c,7U,document,sizeof golden,&length)==UMI_STATUS_OK && length==sizeof golden-1U);
    }else if(!strcmp(mode,"small-capacity") || !strcmp(mode,"zero-capacity")){
        size_t capacity=!strcmp(mode,"zero-capacity")?0U:sizeof golden-1U;
        CHECK(UmiProviderConnectionDocumentExport(&c,7U,document,capacity,&length)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(length==79U && !memcmp(document,before,sizeof document));
    }else if(!strcmp(mode,"zero-export-revision") || !strcmp(mode,"invalid-connection")){
        if(!strcmp(mode,"invalid-connection"))c.label[0]=0;else revision=0U;
        CHECK(UmiProviderConnectionDocumentExport(&c,revision,document,sizeof document,&length)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(length==79U && !memcmp(document,before,sizeof document));
    }else if(!strcmp(mode,"truncated")){
        for(size_t i=0U;i<sizeof golden-1U;++i)CHECK(!Refused(golden,i,UMI_STATUS_PARSE_ERROR));
    }else if(!strcmp(mode,"not-terminated")){
        unsigned char exact[sizeof golden-1U];memcpy(exact,golden,sizeof exact);
        CHECK(UmiProviderConnectionDocumentImport(exact,sizeof exact,&decoded,&actual)==UMI_STATUS_OK && actual==7U);
    }else if(!strcmp(mode,"embedded-nul")){
        memcpy(document,golden,sizeof golden);document[sizeof golden-3U]=0;
        CHECK(!Refused(document,sizeof golden-1U,UMI_STATUS_PARSE_ERROR));
    }else if(!strcmp(mode,"trailing-data")){
        memcpy(document,golden,sizeof golden);document[sizeof golden-1U]='x';
        CHECK(!Refused(document,sizeof golden,UMI_STATUS_PARSE_ERROR));
    }else if(!strcmp(mode,"maximum-bytes")){
        CHECK(!Refused(golden,UMI_PROVIDER_CONNECTION_DOCUMENT_MAX_BYTES,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(!Refused(golden,SIZE_MAX,UMI_STATUS_CAPACITY_EXCEEDED));
    }else if(!strcmp(mode,"raw-wire")){
        const char *wire=strstr(golden,"connection\n2\n");CHECK(wire);
        CHECK(!Refused(wire,strlen(wire),UMI_STATUS_PARSE_ERROR));
    }else if(!strcmp(mode,"upper-hex") || !strcmp(mode,"nul-hex") || !strcmp(mode,"odd-hex") || !strcmp(mode,"crlf")){
        memcpy(document,golden,sizeof golden);size_t mutated_length=sizeof golden-1U;
        char *field=strstr(document,"636f6d66792d6578616d706c65");CHECK(field);
        if(!strcmp(mode,"upper-hex"))field[3]='F';
        if(!strcmp(mode,"nul-hex")){field[0]='0';field[1]='0';}
        if(!strcmp(mode,"odd-hex"))field[1]='\n';
/* Exercise an actual CRLF sequence rather than merely corrupting a prefix character. The former implementation is retained for owner review. */
#if 0
        if(!strcmp(mode,"crlf"))document[25]='\r';
#endif
        if(!strcmp(mode,"crlf")){
            char *lf=strchr(document,'\n');CHECK(lf);
            size_t offset=(size_t)(lf-document);
            memmove(lf+1U,lf,sizeof golden-offset);*lf='\r';++mutated_length;
        }
        CHECK(!Refused(document,mutated_length,UMI_STATUS_PARSE_ERROR));
    }else if(!strcmp(mode,"unknown-version") || !strcmp(mode,"zero-version") || !strcmp(mode,"version-leading-zero") ||
             !strcmp(mode,"revision-leading-zero") || !strcmp(mode,"revision-sign") || !strcmp(mode,"revision-overflow") || !strcmp(mode,"zero-import-revision")){
        const char *version="1",*number="7";UmiStatus expected=UMI_STATUS_PARSE_ERROR;
        if(!strcmp(mode,"unknown-version")){version="2";expected=UMI_STATUS_NOT_IMPLEMENTED;}
        if(!strcmp(mode,"zero-version")){version="0";expected=UMI_STATUS_NOT_IMPLEMENTED;}
        if(!strcmp(mode,"version-leading-zero"))version="01";
        if(!strcmp(mode,"revision-leading-zero"))number="07";
        if(!strcmp(mode,"revision-sign"))number="+7";
        if(!strcmp(mode,"revision-overflow"))number="18446744073709551616";
        if(!strcmp(mode,"zero-import-revision"))number="0";
        const char *wire=strstr(golden,"connection\n2\n");CHECK(wire);
        int n=snprintf(document,sizeof document,"umicom-provider-connection\n%s\n%s\n%s",version,number,wire);
        CHECK(n>0 && (size_t)n<sizeof document);CHECK(!Refused(document,(size_t)n,expected));
    }else if(!strcmp(mode,"nulls")){
        CHECK(UmiProviderConnectionDocumentExport(NULL,7U,document,sizeof document,&length)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderConnectionDocumentExport(&c,7U,NULL,sizeof document,&length)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderConnectionDocumentExport(&c,7U,document,sizeof document,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(document,before,sizeof document) && length==79U);
        CHECK(!Refused(NULL,sizeof golden-1U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(UmiProviderConnectionDocumentImport(golden,sizeof golden-1U,NULL,&actual)==UMI_STATUS_INVALID_ARGUMENT && actual==0U);
        CHECK(UmiProviderConnectionDocumentImport(golden,sizeof golden-1U,&decoded,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }else return 2;
    return 0;
}
