/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Real canonical Data Server, not a substitute map. Paths are never opened. */
#include "umicom/vm_manager/manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#define PROFILE_PID GetCurrentProcessId()
#else
#include <unistd.h>
#define PROFILE_PID getpid()
#endif
#define CHECK(c) do{if(!(c)){fprintf(stderr,"line %d: %s\n",__LINE__,#c);return 1;}}while(0)
static UmiStatus Count(const UmiVmProfile*p,void*c){
    if(strcmp(p->id,"notes"))return UMI_STATUS_INVALID_STATE;
    (*(unsigned*)c)++;
    return UMI_STATUS_OK;
}
static void Profile(UmiVmProfile*p){
    UmiVmProfileInit(p);
    strcpy(p->id,"notes");
    strcpy(p->name,"Notes practice");
#ifdef _WIN32
    strcpy(p->runtimeDirectory,"C:/Practice/runtime");
    strcpy(p->imageBundle,"C:/Practice/image");
#else
    strcpy(p->runtimeDirectory,"/tmp/practice/runtime");
    strcpy(p->imageBundle,"/tmp/practice/image");
#endif
}
int main(int argc,char**argv){
    CHECK(argc>=2);
    UmiDataServer*server=NULL;
    UmiVmProfile p,loaded;
    uint64_t revision=0;
    Profile(&p);
    const char*test=argv[1];
    if(!strcmp(test,"validation")){
        CHECK(UmiVmProfileValidate(&p)==UMI_STATUS_OK);
        p.memoryMiB=1;
        CHECK(UmiVmProfileValidate(&p)!=UMI_STATUS_OK);
        Profile(&p);
        p.processors=17;
        CHECK(UmiVmProfileValidate(&p)!=UMI_STATUS_OK);
        Profile(&p);
        strcpy(p.id,"../bad");
        CHECK(UmiVmProfileValidate(&p)!=UMI_STATUS_OK);
        Profile(&p);
        memset(p.name,'a',sizeof p.name);
        CHECK(UmiVmProfileValidate(&p)!=UMI_STATUS_OK);
        Profile(&p);
        strcpy(p.imageBundle,"relative");
        CHECK(UmiVmProfileValidate(&p)!=UMI_STATUS_OK);
        return 0;
    }
    if(!strcmp(test,"sqlite")){
        CHECK(argc==3);
        char db[2048];
        int len=snprintf(db,sizeof db,"%s-%lu.db",argv[2],(unsigned long)PROFILE_PID);
        CHECK(len>0&&(size_t)len<sizeof db);
        UmiStatus s=umi_data_server_create_sqlite(db,&server);
        if(s==UMI_STATUS_UNAVAILABLE||s==UMI_STATUS_NOT_IMPLEMENTED)return 77;
        CHECK(s==UMI_STATUS_OK);
        CHECK(UmiVmProfileSave(server,&p,0,&revision)==UMI_STATUS_OK);
        umi_data_server_destroy(server);
        CHECK(umi_data_server_create_sqlite(db,&server)==UMI_STATUS_OK);
        CHECK(UmiVmProfileLoad(server,p.id,&loaded)==UMI_STATUS_OK);
        CHECK(loaded.revision==1&&!strcmp(p.name,loaded.name));
        umi_data_server_destroy(server);
        return 0;
    }
    CHECK(umi_data_server_create_memory(&server)==UMI_STATUS_OK);
    CHECK(umi_data_server_set(server,"unrelated","keep me")==UMI_STATUS_OK);
    CHECK(UmiVmProfileSave(server,&p,0,&revision)==UMI_STATUS_OK&&revision==1);
    if(!strcmp(test,"save_load")){
        CHECK(UmiVmProfileLoad(server,p.id,&loaded)==UMI_STATUS_OK);
        CHECK(!strcmp(loaded.name,p.name)&&loaded.revision==1);
    }
    else if(!strcmp(test,"conflict")){
        CHECK(UmiVmProfileSave(server,&p,0,&revision)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiVmProfileSave(server,&p,1,&revision)==UMI_STATUS_OK&&revision==2);
        CHECK(UmiVmProfileSave(server,&p,1,&revision)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiVmProfileLoad(server,p.id,&loaded)==UMI_STATUS_OK&&loaded.revision==2);
    }
    else if(!strcmp(test,"caller_transaction")){
        CHECK(umi_data_server_begin(server)==UMI_STATUS_OK);
        CHECK(UmiVmProfileSave(server,&p,1,&revision)==UMI_STATUS_BUSY);
        CHECK(UmiVmProfileRemove(server,p.id,1)==UMI_STATUS_BUSY);
        CHECK(umi_data_server_in_transaction(server));
        CHECK(umi_data_server_rollback(server)==UMI_STATUS_OK);
    }
    else if(!strcmp(test,"visit_remove")){
        unsigned count=0;
        CHECK(UmiVmProfileVisit(server,Count,&count)==UMI_STATUS_OK&&count==1);
        CHECK(UmiVmProfileRemove(server,p.id,2)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiVmProfileRemove(server,p.id,1)==UMI_STATUS_OK);
        CHECK(UmiVmProfileLoad(server,p.id,&loaded)==UMI_STATUS_NOT_FOUND);
        char value[32];
        CHECK(umi_data_server_get(server,"unrelated",value,sizeof value)==UMI_STATUS_OK&&!strcmp(value,"keep me"));
    }
    else if(!strcmp(test,"corrupt")){
        CHECK(umi_data_server_set(server,"vm.profile.notes","broken")==UMI_STATUS_OK);
        CHECK(UmiVmProfileLoad(server,p.id,&loaded)==UMI_STATUS_PARSE_ERROR);
        CHECK(UmiVmProfileSave(server,&p,1,&revision)==UMI_STATUS_PARSE_ERROR);
        CHECK(!umi_data_server_in_transaction(server));
    }
    else if(!strcmp(test,"identity_mismatch")){
        char record[16384];
        CHECK(umi_data_server_get(server,"vm.profile.notes",record,sizeof record)==UMI_STATUS_OK);
        CHECK(umi_data_server_set(server,"vm.profile.other",record)==UMI_STATUS_OK);
        CHECK(UmiVmProfileLoad(server,"other",&loaded)==UMI_STATUS_INVALID_STATE);
    }
    else if(!strcmp(test,"capacity")){
        for(unsigned i=1;i<64;++i){
            snprintf(p.id,sizeof p.id,"profile-%u",i);
            CHECK(UmiVmProfileSave(server,&p,0,&revision)==UMI_STATUS_OK);
        }
        strcpy(p.id,"one-too-many");
        CHECK(UmiVmProfileSave(server,&p,0,&revision)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!umi_data_server_in_transaction(server));
    }
    else if(!strcmp(test,"unicode")){
        strcpy(p.name,"Notes \xc3\xa9 \xf0\x9f\x93\x92");
        CHECK(UmiVmProfileSave(server,&p,1,&revision)==UMI_STATUS_OK);
        CHECK(UmiVmProfileLoad(server,p.id,&loaded)==UMI_STATUS_OK&&!strcmp(p.name,loaded.name));
    }
    else if(!strcmp(test,"overflow")){
        CHECK(UmiVmProfileSave(server,&p,UINT64_MAX,&revision)==UMI_STATUS_INVALID_ARGUMENT);
    }
    else if(!strcmp(test,"mutation")){
        char valid[16384],data[16384];
        CHECK(umi_data_server_get(server,"vm.profile.notes",valid,sizeof valid)==UMI_STATUS_OK);
        size_t n=strlen(valid);
        for(size_t i=0;i<n;++i){
            memcpy(data,valid,n+1);
            data[i]=0;
            CHECK(umi_data_server_set(server,"vm.profile.notes",data)==UMI_STATUS_OK);
            CHECK(UmiVmProfileLoad(server,p.id,&loaded)!=UMI_STATUS_OK);
        }
        CHECK(umi_data_server_set(server,"vm.profile.notes",valid)==UMI_STATUS_OK);
    }
    else return 2;
    umi_data_server_destroy(server);
    return 0;
}
