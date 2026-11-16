/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connections/test_fingerprint.c
 *
 * PURPOSE:
 *   Verify canonical connection identity across content edits, padding, invalid inputs and a memory-store round trip.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/provider_connections/connections.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition) do { if(!(condition)) { fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while(0)
static UmiProviderConnection connection(unsigned char fill)
{
    UmiProviderConnection value;memset(&value,fill,sizeof(value));
    strcpy(value.id,"personal");strcpy(value.provider_id,"example");
    strcpy(value.label,"Personal inference");
    strcpy(value.endpoint,"https://api.example.invalid/inference");
    strcpy(value.model,"example/model");strcpy(value.secret_reference,"vault:personal-key");
    value.route=UMI_PROVIDER_CONNECTION_HTTPS;value.timeout_ms=30000U;value.enabled=true;
    return value;
}
static int store_roundtrip(const UmiProviderConnection *value,const unsigned char *expected)
{
    UmiDataServer *server=NULL;UmiProviderConnections *store=NULL;
    UmiProviderConnectionSnapshot *snapshot=calloc(1U,sizeof(*snapshot));CHECK(snapshot!=NULL);
    CHECK(umi_data_server_create_memory(&server)==UMI_STATUS_OK);
    CHECK(UmiProviderConnectionsOpen(server,"media","owner",&store)==UMI_STATUS_OK);
    uint64_t revision=0U;
    CHECK(UmiProviderConnectionsPut(store,value,false,0U,&revision)==UMI_STATUS_OK && revision==1U);
    CHECK(UmiProviderConnectionsRead(store,snapshot)==UMI_STATUS_OK && snapshot->count==1U);
    unsigned char digest[UMI_SHA256_BYTES];
    CHECK(UmiProviderConnectionFingerprint(&snapshot->items[0],digest)==UMI_STATUS_OK);
    CHECK(memcmp(digest,expected,sizeof(digest))==0);
    UmiProviderConnectionsDestroy(store);umi_data_server_destroy(server);free(snapshot);return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    UmiProviderConnection original=connection(0U),changed=original;
    unsigned char expected[UMI_SHA256_BYTES],actual[UMI_SHA256_BYTES];
    CHECK(UmiProviderConnectionFingerprint(&original,expected)==UMI_STATUS_OK);
    char text[UMI_SHA256_HEX_CAPACITY];UmiSha256Hex(expected,text);
    CHECK(strcmp(text,"83a92a56cf944a9c2ca60bfb8ca98f8307f311b9a4ebbd9e1cff7fbe4b0b8da0")==0);
    memset(actual,0xa5,sizeof(actual));unsigned char before[sizeof(actual)];memcpy(before,actual,sizeof(actual));
    const UmiProviderConnection *input=&changed;
    UmiStatus wanted=UMI_STATUS_OK;bool same=false;
    if(strcmp(mode,"oracle")==0) same=true;
    else if(strcmp(mode,"padding")==0) { changed=connection(0xcdU);same=true; }
    else if(strcmp(mode,"id")==0) strcpy(changed.id,"second");
    else if(strcmp(mode,"provider")==0) strcpy(changed.provider_id,"another");
    else if(strcmp(mode,"label")==0) strcpy(changed.label,"Another label");
    else if(strcmp(mode,"unicode")==0) strcpy(changed.label,"Caf\xc3\xa9 connection");
    else if(strcmp(mode,"endpoint")==0) strcpy(changed.endpoint,"https://other.example.invalid/inference");
    else if(strcmp(mode,"model")==0) strcpy(changed.model,"other/model");
    else if(strcmp(mode,"empty-model")==0) changed.model[0]='\0';
    else if(strcmp(mode,"reference")==0) strcpy(changed.secret_reference,"vault:other-key");
    else if(strcmp(mode,"empty-reference")==0) changed.secret_reference[0]='\0';
    else if(strcmp(mode,"timeout")==0) changed.timeout_ms=60000U;
    else if(strcmp(mode,"disabled")==0) changed.enabled=false;
    else if(strcmp(mode,"loopback")==0) {
        changed.route=UMI_PROVIDER_CONNECTION_LOOPBACK;
        strcpy(changed.endpoint,"http://127.0.0.1:8080");changed.secret_reference[0]='\0';
    } else if(strcmp(mode,"null-connection")==0) { input=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"null-output")==0) {
        CHECK(UmiProviderConnectionFingerprint(&original,NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    } else if(strcmp(mode,"unterminated")==0) {
        memset(changed.model,'x',sizeof(changed.model));wanted=UMI_STATUS_INVALID_ARGUMENT;
    } else if(strcmp(mode,"bad-endpoint")==0) {
        strcpy(changed.endpoint,"https://host.invalid/?token=forbidden");wanted=UMI_STATUS_INVALID_ARGUMENT;
    } else if(strcmp(mode,"bad-reference")==0) {
        strcpy(changed.secret_reference,"plaintext-secret");wanted=UMI_STATUS_INVALID_ARGUMENT;
    } else if(strcmp(mode,"bad-label")==0) {
        strcpy(changed.label,"bad\nlabel");wanted=UMI_STATUS_INVALID_ARGUMENT;
    } else if(strcmp(mode,"bad-timeout")==0) { changed.timeout_ms=0U;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"bad-route")==0) { changed.route=(UmiProviderConnectionRoute)99;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(mode,"store-roundtrip")==0) return store_roundtrip(&original,expected);
    else return 2;
    CHECK(UmiProviderConnectionFingerprint(input,actual)==wanted);
    if(wanted!=UMI_STATUS_OK) CHECK(memcmp(actual,before,sizeof(actual))==0);
    else CHECK((memcmp(actual,expected,sizeof(actual))==0)==same);
    /* Hashing never changes input settings or resolves the referenced secret. */
    CHECK(original.enabled && original.timeout_ms==30000U);
    return 0;
}
