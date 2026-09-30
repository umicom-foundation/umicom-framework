/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vm_manager/inert_qmp.c
 * PURPOSE:
 *   INERT TEST CHILD. It speaks a tiny fixed QMP-shaped dialogue and can write a synthetic
 *   qcow2 header. It is not QEMU, a guest or a usable disk-image tool.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * INERT TEST CHILD. It speaks a tiny fixed QMP-shaped dialogue and can write a
 * synthetic qcow2 header. It is not QEMU, a guest or a usable disk-image tool.
 *---------------------------------------------------------------------------*/
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
static void Delay(unsigned ms){
    struct timespec t={
        (time_t)(ms/1000U),(long)((ms%1000U)*1000000U)
    };
    nanosleep(&t,NULL);
}
static void Be32(unsigned char*p,uint32_t n){
    p[0]=(unsigned char)(n>>24);
    p[1]=(unsigned char)(n>>16);
    p[2]=(unsigned char)(n>>8);
    p[3]=(unsigned char)n;
}
static void Be64(unsigned char*p,uint64_t n){
    Be32(p,(uint32_t)(n>>32));
    Be32(p+4,(uint32_t)n);
}
int main(int argc,char**argv){
    const char*mode=argc>1?argv[1]:"normal";
    if(!strcmp(mode,"create")){
        if(argc!=9)return 20;
        unsigned char h[4096]={
            0
        };
        Be32(h,0x514649fbU);
        Be32(h+4,3);
        Be32(h+20,16);
        Be64(h+24,strtoull(argv[8],NULL,10));
        Be32(h+96,4);
        Be32(h+100,104);
        FILE*f=fopen(argv[7],"wbx");
        if(!f)return 21;
        int result=fwrite(h,1,sizeof h,f)==sizeof h?0:22;
        if(fclose(f))result=23;
        return result;
    }
    if(!strcmp(mode,"check"))return 0;
    if(!strcmp(mode,"arguments")){
        for(int i=2;i<argc;++i)printf("%zu:%s\n",strlen(argv[i]),argv[i]);
        printf("injected=%s\n",getenv("UMICOM_INJECTED_SECRET")?"present":"absent");
        return 0;
    }
    if(!strcmp(mode,"silent")){
        Delay(15000);
        return 0;
    }
    if(!strcmp(mode,"exit"))return 17;
    if(!strcmp(mode,"descendant")){
        pid_t p=fork();
        if(p<0)return 1;
        if(p==0){
            Delay(15000);
            return 0;
        }
        printf("%ld\n",(long)p);
        fflush(stdout);
        return 0;
    }
    if(!strcmp(mode,"bad-greeting")){
        puts("{\"QMP\":false}");
        fflush(stdout);
        Delay(15000);
        return 0;
    }
    const char*greeting="{\"QMP\":{\"version\":{\"qemu\":{\"major\":10,\"minor\":1,\"micro\":0},\"package\":\"inert test fixture\"},\"capabilities\":[]}}\r\n";
    if(!strcmp(mode,"split")){
        for(size_t i=0;greeting[i];++i){
            putchar(greeting[i]);
            fflush(stdout);
        }
    }
    else{
        fputs(greeting,stdout);
        fflush(stdout);
    }
    fputs("INERT protocol fixture; not a guest boot.\n",stderr);
    fflush(stderr);
    int running=0;
    char line[8192];
    while(fgets(line,sizeof line,stdin)){
        char*id=strstr(line,"\"id\":");
        if(!id)return 30;
        uint64_t n=strtoull(id+5,NULL,10);
        if(strstr(line,"qmp_capabilities")){
            if(!strcmp(mode,"negotiate-error"))printf("{\"error\":{\"class\":\"GenericError\",\"desc\":\"fixture denial\"},\"id\":%" PRIu64 "}\r\n",n);
            else printf("{\"return\":{},\"id\":%" PRIu64 "}\r\n",n);
        }
        else if(strstr(line,"query-status")){
            if(!strcmp(mode,"wrong-id"))printf("{\"return\":{},\"id\":9999}\r\n");
            if(!strcmp(mode,"query-timeout")){
                Delay(15000);
                return 0;
            }
            if(!strcmp(mode,"oversize")){
                for(unsigned i=0;i<70000;++i)putchar('x');
                putchar('\n');
            }
            else if(!strcmp(mode,"malformed"))printf("{\"return\":{},\"id\":%" PRIu64 ",\"id\":9}\r\n",n);
            else printf("{\"event\":\"FIXTURE\"}\r\n{\"return\":{\"running\":%s,\"status\":\"%s\"},\"id\":%" PRIu64 "}\r\n",running?"true":"false",running?"running":"paused",n);
        }
        else if(strstr(line,"\"cont\"")){
            if(!strcmp(mode,"resume-error"))printf("{\"error\":{\"class\":\"GenericError\",\"desc\":\"fixture resume refused\"},\"id\":%" PRIu64 "}\r\n",n);
            else{
                running=1;
                printf("{\"event\":\"RESUME\"}\r\n{\"return\":{},\"id\":%" PRIu64 "}\r\n",n);
            }
        }
        else if(strstr(line,"\"stop\"")){
            running=0;
            printf("{\"event\":\"STOP\"}\r\n{\"return\":{},\"id\":%" PRIu64 "}\r\n",n);
        }
        else if(strstr(line,"ringbuf-read")){
            if(!strcmp(mode,"console-type"))printf("{\"return\":{},\"id\":%" PRIu64 "}\r\n",n);
            else printf("{\"return\":\"VW1pY29tIE5vdGVzIGZpeHR1cmUK\",\"id\":%" PRIu64 "}\r\n",n);
        }
        else {
            printf("{\"return\":{},\"id\":%" PRIu64 "}\r\n",n);
            if(strstr(line,"\"quit\"")){
                fflush(stdout);
                return 0;
            }
        }
        fflush(stdout);
    }
    return 0;
}
