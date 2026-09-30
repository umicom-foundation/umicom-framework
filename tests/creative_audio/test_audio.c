/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_audio/test_audio.c
 * PURPOSE:
 *   Native clip, boundary, independent sample and ordinary-file regression tests.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native clip, boundary, independent sample and ordinary-file regression tests. */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/creative_workspace/audio.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
static void U16(unsigned char *p,unsigned value) {p[0]=(unsigned char)(value&255U);p[1]=(unsigned char)((value>>8U)&255U);}
static void U32(unsigned char *p,uint32_t value) {for(unsigned i=0;i<4;++i)p[i]=(unsigned char)((value>>(8U*i))&255U);}
static int16_t Expected(uint64_t i,unsigned c)
{
    static const int16_t samples[]={INT16_MIN,INT16_MAX,0,10000,-10000,1,-1,12345,-23456};
    return samples[(i*5U+c*3U)%9U];
}
static UmiCreativeExport Fixture(size_t frames,unsigned channels)
{
    UmiCreativeExport e={0};e.size=44U+frames*channels*2U;e.bytes=calloc(e.size,1U);
    if(!e.bytes){e.size=0U;return e;}
    memcpy(e.bytes,"RIFF",4);U32(e.bytes+4,(uint32_t)(e.size-8));memcpy(e.bytes+8,"WAVEfmt ",8);
    U32(e.bytes+16,16);U16(e.bytes+20,1);U16(e.bytes+22,channels);U32(e.bytes+24,48000);
    U32(e.bytes+28,48000U*channels*2U);U16(e.bytes+32,channels*2U);U16(e.bytes+34,16);
    memcpy(e.bytes+36,"data",4);U32(e.bytes+40,(uint32_t)(e.size-44));
    for(size_t f=0;f<frames;++f)for(unsigned c=0;c<channels;++c)U16(e.bytes+44+(f*channels+c)*2U,(uint16_t)Expected(f,c));
    return e;
}
static bool Rejected(const UmiCreativeExport *e)
{
    UmiCreativeAudioClip *clip=NULL;
    UmiStatus status=UmiCreativeAudioDecode(e->bytes,e->size,&clip);
    bool result=status!=UMI_STATUS_OK&&clip==NULL;UmiCreativeAudioDestroy(clip);return result;
}
static int DecodeCase(const char *name)
{
    UmiCreativeExport e=Fixture(19U,2U);CHECK(e.bytes);
    if(!strcmp(name,"truncated_header")){size_t n=e.size;for(size_t i=0;i<44U;++i){e.size=i;CHECK(Rejected(&e));}e.size=n;}
    else if(!strcmp(name,"wrong_magic")){e.bytes[0]='X';CHECK(Rejected(&e));e.bytes[0]='R';e.bytes[8]='X';CHECK(Rejected(&e));}
    else if(!strcmp(name,"riff_length")){U32(e.bytes+4,(uint32_t)e.size-7U);CHECK(Rejected(&e));U32(e.bytes+4,(uint32_t)e.size-9U);CHECK(Rejected(&e));}
    else if(!strcmp(name,"format_tag")){U16(e.bytes+20,3U);CHECK(Rejected(&e));U16(e.bytes+20,65534U);CHECK(Rejected(&e));}
    else if(!strcmp(name,"sample_bits")){U16(e.bytes+34,24);CHECK(Rejected(&e));U16(e.bytes+34,8);CHECK(Rejected(&e));}
    else if(!strcmp(name,"channels")){U16(e.bytes+22,0);CHECK(Rejected(&e));U16(e.bytes+22,3);CHECK(Rejected(&e));}
    else if(!strcmp(name,"sample_rate")){U32(e.bytes+24,7999);CHECK(Rejected(&e));U32(e.bytes+24,192001);CHECK(Rejected(&e));}
    else if(!strcmp(name,"block_alignment")){U16(e.bytes+32,2);CHECK(Rejected(&e));}
    else if(!strcmp(name,"byte_rate")){U32(e.bytes+28,1);CHECK(Rejected(&e));}
    else if(!strcmp(name,"partial_frame")){--e.size;U32(e.bytes+4,(uint32_t)e.size-8);U32(e.bytes+40,(uint32_t)e.size-44);CHECK(Rejected(&e));}
    else if(!strcmp(name,"empty_data")){e.size=44;U32(e.bytes+4,36);U32(e.bytes+40,0);CHECK(Rejected(&e));}
    else if(!strcmp(name,"oversized_chunk")){U32(e.bytes+40,UINT32_MAX);CHECK(Rejected(&e));}
    else if(!strcmp(name,"missing_chunks")){memcpy(e.bytes+12,"JUNK",4);CHECK(Rejected(&e));memcpy(e.bytes+12,"fmt ",4);memcpy(e.bytes+36,"JUNK",4);CHECK(Rejected(&e));}
    else if(!strcmp(name,"truncated_payload")){for(size_t i=44;i<e.size;++i){UmiCreativeAudioClip *c=NULL;CHECK(UmiCreativeAudioDecode(e.bytes,i,&c)!=UMI_STATUS_OK&&c==NULL);}}
    else {UmiCreativeExportFree(&e);return 2;}
    UmiCreativeExportFree(&e);return 0;
}
static int Chunks(const char *name)
{
    UmiCreativeExport base=Fixture(8,1), e={0};CHECK(base.bytes);
    size_t extra=!strcmp(name,"chunk_limit")?8U*255U:!strcmp(name,"fmt_extension")?2U:!strcmp(name,"duplicate_format")?24U:!strcmp(name,"duplicate_data")?24U:10U;
    e.size=base.size+extra;e.bytes=calloc(e.size,1);CHECK(e.bytes);
    memcpy(e.bytes,base.bytes,12);U32(e.bytes+4,(uint32_t)e.size-8);
    if(!strcmp(name,"fmt_extension")){
        memcpy(e.bytes+12,base.bytes+12,24);U32(e.bytes+16,18);memcpy(e.bytes+38,base.bytes+36,base.size-36);
        UmiCreativeAudioClip *clip=NULL;OK(UmiCreativeAudioDecode(e.bytes,e.size,&clip));UmiCreativeAudioDestroy(clip);
        U16(e.bytes+36,1);CHECK(Rejected(&e));
    }else if(!strcmp(name,"chunk_limit")){
        for(size_t i=0;i<255U;++i)memcpy(e.bytes+12+i*8U,"JUNK",4);
        memcpy(e.bytes+12+extra,base.bytes+12,base.size-12);CHECK(Rejected(&e));
    }else if(!strcmp(name,"duplicate_format")){
        memcpy(e.bytes+12,base.bytes+12,24);memcpy(e.bytes+36,base.bytes+12,base.size-12);CHECK(Rejected(&e));
    }else if(!strcmp(name,"duplicate_data")){
        memcpy(e.bytes+12,base.bytes+36,24);memcpy(e.bytes+36,base.bytes+12,base.size-12);CHECK(Rejected(&e));
    }else {
        memcpy(e.bytes+12,"JUNK",4);U32(e.bytes+16,1);e.bytes[20]=200;e.bytes[21]=255;
        memcpy(e.bytes+22,base.bytes+12,base.size-12);
        UmiCreativeAudioClip *clip=NULL;OK(UmiCreativeAudioDecode(e.bytes,e.size,&clip));
        UmiCreativeAudioInfo info;OK(UmiCreativeAudioGetInfo(clip,&info));CHECK(info.ignoredChunks==1&&info.frames==8);
        UmiCreativeAudioEdit edit={0,8,1000,0,0};UmiCreativeExport out={0};OK(UmiCreativeAudioRender(clip,&edit,&out));
        CHECK(out.size==base.size&&!memcmp(out.bytes,base.bytes,base.size));
        UmiCreativeExportFree(&out);UmiCreativeAudioDestroy(clip);
        /* Removing the odd-chunk pad leaves an invalid chunk stream. */
        memmove(e.bytes+21,e.bytes+22,e.size-22);--e.size;U32(e.bytes+4,(uint32_t)e.size-8);CHECK(Rejected(&e));
    }
    UmiCreativeExportFree(&e);UmiCreativeExportFree(&base);return 0;
}
static int General(const char *name)
{
    UmiCreativeExport e=Fixture(1003,2), rendered={0};UmiCreativeAudioClip *clip=NULL,*second=NULL;CHECK(e.bytes);
    OK(UmiCreativeAudioDecode(e.bytes,e.size,&clip));UmiCreativeAudioInfo info;OK(UmiCreativeAudioGetInfo(clip,&info));
    if(!strcmp(name,"stereo_samples")){
        CHECK(info.channels==2&&info.frames==1003&&info.sampleRate==48000);
        for(uint64_t f=0;f<1003;++f)for(unsigned c=0;c<2;++c){int16_t s;OK(UmiCreativeAudioSample(clip,f,c,&s));CHECK(s==Expected(f,c));}
    }else if(!strcmp(name,"copied_ownership")){
        memset(e.bytes+44,0,e.size-44);UmiCreativeExportFree(&e);
        int16_t s;OK(UmiCreativeAudioSample(clip,0,0,&s));CHECK(s==INT16_MIN);
    }else if(!strcmp(name,"unchanged_outputs")){
        int16_t s=123;CHECK(UmiCreativeAudioSample(clip,1003,0,&s)==UMI_STATUS_INVALID_ARGUMENT&&s==123);
        CHECK(UmiCreativeAudioSample(clip,0,2,&s)==UMI_STATUS_INVALID_ARGUMENT&&s==123);
        UmiCreativeAudioOverview o,old;memset(&o,0xAC,sizeof(o));old=o;
        CHECK(UmiCreativeAudioInspect(clip,0,1004,10,&o)!=UMI_STATUS_OK&&!memcmp(&old,&o,sizeof(o)));
        CHECK(UmiCreativeAudioInspect(clip,1,1,10,&o)!=UMI_STATUS_OK&&!memcmp(&old,&o,sizeof(o)));
        CHECK(UmiCreativeAudioInspect(clip,0,1,0,&o)!=UMI_STATUS_OK);
        CHECK(UmiCreativeAudioInspect(clip,0,1,513,&o)!=UMI_STATUS_OK);
        UmiCreativeAudioClip *before=clip;CHECK(UmiCreativeAudioDecode(e.bytes,e.size,&clip)==UMI_STATUS_INVALID_STATE&&clip==before);
    }else if(!strcmp(name,"full_range_exact")){
        UmiCreativeAudioEdit edit={0,info.frames,1000,0,0};OK(UmiCreativeAudioRender(clip,&edit,&rendered));CHECK(e.size==rendered.size&&!memcmp(e.bytes,rendered.bytes,e.size));
    }else if(!strcmp(name,"trim_boundaries")){
        UmiCreativeAudioEdit edit={1,1002,1000,0,0};OK(UmiCreativeAudioRender(clip,&edit,&rendered));OK(UmiCreativeAudioDecode(rendered.bytes,rendered.size,&second));
        for(uint64_t f=0;f<1001;++f)for(unsigned c=0;c<2;++c){int16_t s;OK(UmiCreativeAudioSample(second,f,c,&s));CHECK(s==Expected(f+1,c));}
    }else if(!strcmp(name,"invalid_edits")){
        UmiCreativeAudioEdit edit={0,info.frames,1001,0,0};CHECK(UmiCreativeAudioRender(clip,&edit,&rendered)!=UMI_STATUS_OK);
        edit.gainPermille=1000;edit.fadeInFrames=info.frames+1;CHECK(UmiCreativeAudioRender(clip,&edit,&rendered)!=UMI_STATUS_OK);
        edit.fadeInFrames=0;edit.fadeOutFrames=info.frames+1;CHECK(UmiCreativeAudioRender(clip,&edit,&rendered)!=UMI_STATUS_OK);
        edit.fadeOutFrames=0;edit.beginFrame=info.frames;CHECK(UmiCreativeAudioRender(clip,&edit,&rendered)!=UMI_STATUS_OK);
        edit.beginFrame=1;edit.endFrame=0;CHECK(UmiCreativeAudioRender(clip,&edit,&rendered)!=UMI_STATUS_OK);
        edit.beginFrame=0;edit.endFrame=UINT64_MAX;CHECK(UmiCreativeAudioRender(clip,&edit,&rendered)!=UMI_STATUS_OK);CHECK(!rendered.bytes&&!rendered.size);
    }else if(!strcmp(name,"live_export_refused")){
        UmiCreativeAudioEdit edit={0,1,1000,0,0};OK(UmiCreativeAudioRender(clip,&edit,&rendered));unsigned char *old=rendered.bytes;
        CHECK(UmiCreativeAudioRender(clip,&edit,&rendered)==UMI_STATUS_INVALID_STATE&&old==rendered.bytes);
    }else if(!strcmp(name,"gain_and_fades")){
        const unsigned gains[]={0,1,333,700,1000};const uint64_t fades[]={0,1,2,51,999,1003};
        for(size_t g=0;g<5;++g)for(size_t a=0;a<6;++a)for(size_t b=0;b<6;++b){
            UmiCreativeAudioEdit edit={0,1003,gains[g],fades[a],fades[b]};OK(UmiCreativeAudioRender(clip,&edit,&rendered));
            OK(UmiCreativeAudioDecode(rendered.bytes,rendered.size,&second));
            for(uint64_t f=0;f<1003;++f)for(unsigned c=0;c<2;++c){
                /* Independent integer reference, with the documented truncation order. */
                int64_t v=(int64_t)Expected(f,c)*gains[g]/1000;
                if(f<fades[a])v=fades[a]==1?0:v*(int64_t)f/(int64_t)(fades[a]-1);
                uint64_t tail=1002-f;if(tail<fades[b])v=fades[b]==1?0:v*(int64_t)tail/(int64_t)(fades[b]-1);
                int16_t s;OK(UmiCreativeAudioSample(second,f,c,&s));CHECK(s==v);
            }
            UmiCreativeAudioDestroy(second);second=NULL;UmiCreativeExportFree(&rendered);
        }
    }else if(!strcmp(name,"overview_reference")){
        const size_t binCounts[]={1,2,7,64,509,512};
        for(size_t n=0;n<6;++n){UmiCreativeAudioOverview o;OK(UmiCreativeAudioInspect(clip,3,997,binCounts[n],&o));
            CHECK(o.binCount==binCounts[n]);uint64_t cursor=3;long double squares[2]={0,0};
            for(size_t b=0;b<o.binCount;++b){CHECK(o.bins[b].beginFrame==cursor&&o.bins[b].endFrame>cursor);
                for(unsigned c=0;c<2;++c){int16_t lo=INT16_MAX,hi=INT16_MIN;
                    for(uint64_t f=cursor;f<o.bins[b].endFrame;++f){int16_t s=Expected(f,c);if(s<lo)lo=s;if(s>hi)hi=s;squares[c]+=(long double)s*s;}
                    CHECK(o.bins[b].minimum[c]==lo&&o.bins[b].maximum[c]==hi);
                }cursor=o.bins[b].endFrame;
            }CHECK(cursor==997);for(unsigned c=0;c<2;++c){CHECK(o.peakMagnitude[c]==32768);CHECK(fabs(o.rms[c]-(double)sqrtl(squares[c]/994))<0.000001);}
        }
    }else if(!strcmp(name,"single_frame")){
        UmiCreativeAudioOverview o;OK(UmiCreativeAudioInspect(clip,99,100,512,&o));CHECK(o.binCount==1&&o.bins[0].endFrame==100);
        UmiCreativeAudioEdit edit={99,100,1000,1,1};OK(UmiCreativeAudioRender(clip,&edit,&rendered));CHECK(rendered.size==48);
        for(size_t i=44;i<48;++i)CHECK(rendered.bytes[i]==0);
    }else if(!strcmp(name,"data_before_format")){
        unsigned char temp[24];memcpy(temp,e.bytes+12,24);memmove(e.bytes+12,e.bytes+36,e.size-36);memcpy(e.bytes+e.size-24,temp,24);
        OK(UmiCreativeAudioDecode(e.bytes,e.size,&second));int16_t s;OK(UmiCreativeAudioSample(second,0,0,&s));CHECK(s==INT16_MIN);
    }else {UmiCreativeAudioDestroy(clip);UmiCreativeExportFree(&e);return 2;}
    UmiCreativeAudioDestroy(second);UmiCreativeAudioDestroy(clip);UmiCreativeExportFree(&rendered);UmiCreativeExportFree(&e);return 0;
}
static int MonoRate(const char *name)
{
    (void)name;
    const uint32_t rates[]={8000,11025,44100,48000,96000,192000};
    for(size_t r=0;r<6;++r){UmiCreativeExport e=Fixture(4,1),out={0};CHECK(e.bytes);U32(e.bytes+24,rates[r]);U32(e.bytes+28,rates[r]*2U);
        UmiCreativeAudioClip *c=NULL;OK(UmiCreativeAudioDecode(e.bytes,e.size,&c));UmiCreativeAudioOverview o;OK(UmiCreativeAudioInspect(c,0,4,512,&o));
        CHECK(o.source.sampleRate==rates[r]&&o.source.channels==1&&o.bins[0].minimum[1]==0&&o.peakMagnitude[1]==0&&o.rms[1]==0);
        UmiCreativeAudioEdit edit={0,4,1000,0,0};OK(UmiCreativeAudioRender(c,&edit,&out));CHECK(out.size==e.size&&!memcmp(out.bytes,e.bytes,e.size));
        UmiCreativeAudioDestroy(c);UmiCreativeExportFree(&e);UmiCreativeExportFree(&out);
    }return 0;
}
static int Limits(const char *name)
{
    if(!strcmp(name,"all_pcm_values")){
        UmiCreativeExport e=Fixture(65536,1),out={0};CHECK(e.bytes);
        for(uint32_t i=0;i<65536U;++i)U16(e.bytes+44+(size_t)i*2U,i);
        UmiCreativeAudioClip *c=NULL;OK(UmiCreativeAudioDecode(e.bytes,e.size,&c));
        for(uint32_t i=0;i<65536U;++i){int16_t v;OK(UmiCreativeAudioSample(c,i,0,&v));
            CHECK((int32_t)v==(i<=32767U?(int32_t)i:(int32_t)i-65536));}
        UmiCreativeAudioEdit edit={0,65536,1000,0,0};OK(UmiCreativeAudioRender(c,&edit,&out));
        CHECK(e.size==out.size&&!memcmp(e.bytes,out.bytes,e.size));
        UmiCreativeAudioDestroy(c);UmiCreativeExportFree(&e);UmiCreativeExportFree(&out);return 0;
    }
    if(!strcmp(name,"maximum_file")){
        size_t frames=(UMI_CREATIVE_EXPORT_LIMIT-44U)/2U;UmiCreativeExport e=Fixture(frames,1);CHECK(e.bytes&&e.size==UMI_CREATIVE_EXPORT_LIMIT);
        UmiCreativeAudioClip *c=NULL;OK(UmiCreativeAudioDecode(e.bytes,e.size,&c));UmiCreativeAudioOverview o;OK(UmiCreativeAudioInspect(c,0,frames,512,&o));CHECK(o.peakMagnitude[0]==32768);
        UmiCreativeAudioEdit edit={0,frames,1000,frames,frames};UmiCreativeExport out={0};OK(UmiCreativeAudioRender(c,&edit,&out));CHECK(out.size==e.size);
        UmiCreativeAudioDestroy(c);UmiCreativeExportFree(&out);UmiCreativeExportFree(&e);return 0;
    }
    if(!strcmp(name,"oversized_file")){
        unsigned char dummy=0;UmiCreativeAudioClip *c=NULL;CHECK(UmiCreativeAudioDecode(&dummy,(size_t)UMI_CREATIVE_EXPORT_LIMIT+1,&c)==UMI_STATUS_CAPACITY_EXCEEDED&&c==NULL);return 0;
    }
    UmiCreativeExport base=Fixture(31,2);CHECK(base.bytes);uint32_t rng=12345;
    for(size_t k=0;k<5000;++k){UmiCreativeExport e={malloc(base.size),base.size};CHECK(e.bytes);memcpy(e.bytes,base.bytes,base.size);
        rng=rng*1664525U+1013904223U;e.bytes[rng%e.size]^=(unsigned char)(1U<<(rng%8));UmiCreativeAudioClip *c=NULL;
        UmiStatus s=UmiCreativeAudioDecode(e.bytes,e.size,&c);
        if(s==UMI_STATUS_OK){UmiCreativeAudioInfo i;OK(UmiCreativeAudioGetInfo(c,&i));CHECK(i.frames>0&&i.channels<=2&&i.sampleRate>=8000);
            UmiCreativeAudioOverview o;OK(UmiCreativeAudioInspect(c,0,i.frames,17,&o));UmiCreativeExport out={0};UmiCreativeAudioEdit edit={0,i.frames,1000,0,0};OK(UmiCreativeAudioRender(c,&edit,&out));UmiCreativeExportFree(&out);
        }else CHECK(c==NULL);
        UmiCreativeAudioDestroy(c);UmiCreativeExportFree(&e);
    }UmiCreativeExportFree(&base);return 0;
}
static void RemoveFixture(const char *path)
{
#ifdef _WIN32
    wchar_t wide[4096];
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, 4096) > 0)
        (void)DeleteFileW(wide);
#else
    (void)remove(path);
#endif
}
static int FileCase(const char *name,const char *root)
{
    char path[4096],other[4096];CHECK(strlen(root)<3500);
    CHECK(snprintf(path,sizeof(path),"%s/clip-%s.wav",root,name)>0);
    CHECK(snprintf(other,sizeof(other),"%s/other-%s.wav",root,name)>0);
    RemoveFixture(path);RemoveFixture(other);
    UmiCreativeAudioClip *c=NULL;UmiCreativeExport e=Fixture(23,2);CHECK(e.bytes);
    if(!strcmp(name,"file_missing")){CHECK(UmiCreativeAudioLoadFile(path,&c)==UMI_STATUS_IO_ERROR&&c==NULL);}
    else if(!strcmp(name,"file_relative")){CHECK(UmiCreativeAudioLoadFile("relative.wav",&c)==UMI_STATUS_INVALID_ARGUMENT&&c==NULL);}
#ifndef _WIN32
    else if(!strcmp(name,"file_symlink")){OK(UmiCreativeExportWriteNew(&e,other));CHECK(symlink(other,path)==0);CHECK(UmiCreativeAudioLoadFile(path,&c)!=UMI_STATUS_OK&&c==NULL);}
    else if(!strcmp(name,"file_fifo")){CHECK(mkfifo(path,0600)==0);CHECK(UmiCreativeAudioLoadFile(path,&c)!=UMI_STATUS_OK&&c==NULL);}
    else if(!strcmp(name,"file_directory")){CHECK(UmiCreativeAudioLoadFile(root,&c)!=UMI_STATUS_OK&&c==NULL);}
#endif
    else if(!strcmp(name,"file_empty")){FILE *f=fopen(path,"wbx");CHECK(f);CHECK(fclose(f)==0);CHECK(UmiCreativeAudioLoadFile(path,&c)==UMI_STATUS_PARSE_ERROR&&c==NULL);}
    else if(!strcmp(name,"file_unicode")){
        CHECK(snprintf(path,sizeof(path),"%s/Umicom-\xD9\x86\xD9\x88\xD8\xAA\xD8\xA9-\xE2\x99\xAB.wav",root)>0);RemoveFixture(path);
        OK(UmiCreativeExportWriteNew(&e,path));OK(UmiCreativeAudioLoadFile(path,&c));
    }else {
        OK(UmiCreativeExportWriteNew(&e,path));OK(UmiCreativeAudioLoadFile(path,&c));
        CHECK(UmiCreativeExportWriteNew(&e,path)==UMI_STATUS_ALREADY_EXISTS);
        UmiCreativeAudioEdit edit={2,20,600,3,4};UmiCreativeExport out={0};OK(UmiCreativeAudioRender(c,&edit,&out));OK(UmiCreativeExportWriteNew(&out,other));
        UmiCreativeAudioClip *unchanged=NULL;OK(UmiCreativeAudioLoadFile(path,&unchanged));int16_t s;OK(UmiCreativeAudioSample(unchanged,0,0,&s));CHECK(s==INT16_MIN);
        UmiCreativeAudioDestroy(unchanged);UmiCreativeExportFree(&out);
    }
    UmiCreativeAudioDestroy(c);UmiCreativeExportFree(&e);RemoveFixture(path);RemoveFixture(other);return 0;
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    const char *name=argv[1];
    if(!strncmp(name,"file_",5))return FileCase(name,argv[2]);
    if(!strcmp(name,"mono_rates"))return MonoRate(name);
    if(!strcmp(name,"all_pcm_values")||!strcmp(name,"maximum_file")||!strcmp(name,"oversized_file")||!strcmp(name,"mutations"))return Limits(name);
    if(!strcmp(name,"metadata_padding")||!strcmp(name,"fmt_extension")||!strcmp(name,"chunk_limit")||!strcmp(name,"duplicate_format")||!strcmp(name,"duplicate_data"))return Chunks(name);
    int result=DecodeCase(name);if(result!=2)return result;return General(name);
}
