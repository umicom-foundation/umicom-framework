/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Exercise the public release evidence contract. All receipts below are fictional. */
#include "umicom/distribution/runtime/evidence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { (void)fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while (0)
#define SOURCE "1111111111111111111111111111111111111111"
#define DIGEST "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
static int Append(char *out, size_t capacity, const char *text)
{
    size_t used = strlen(out), added = strlen(text);
    if (used >= capacity || added >= capacity - used) return 1;
    memcpy(out + used,text,added + 1U); return 0;
}
static int MakeContract(char *out, size_t capacity, size_t rows)
{
    const char *category[] = {"signature","checksum","compatibility","tests","frontend","other"};
    out[0] = '\0';
    CHECK(Append(out,capacity,"UMICOM-RELEASE-CONTRACT\t1\ncandidate\tnotes\nsource\t" SOURCE "\n") == 0);
    for (size_t i = 0U; i < rows; ++i) {
        char line[512];
        int n = snprintf(line,sizeof(line),"require\tr%zu\t%s\tlinux\tRelease\tnative\tFW-01\tExercise requirement %zu\n",i,category[i < 5U ? i : 5U],i);
        CHECK(n > 0 && (size_t)n < sizeof(line)); CHECK(Append(out,capacity,line) == 0);
    }
    return 0;
}
static int Record(char *out,size_t capacity,const char *id, const char *candidate,const char *source,
    const char *profile,const char *configuration,const char *kind,const char *outcome,
    const char *total,const char *passed,const char *failed,const char *skipped,const char *notRun,
    const char *digest,const char *reference)
{
    char line[2048];
    int n = snprintf(line,sizeof(line),"result\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\tasserted\t%s\t%s\n",id,candidate,source,profile,configuration,kind,outcome,total,passed,failed,skipped,notRun,digest,reference);
    CHECK(n > 0 && (size_t)n < sizeof(line)); CHECK(Append(out,capacity,line) == 0); return 0;
}
static int Standard(char *out,size_t capacity,size_t rows)
{
    for(size_t i=0U;i<rows;++i) {
        char id[32]; int n=snprintf(id,sizeof(id),"r%zu",i); CHECK(n>0 && (size_t)n<sizeof(id));
        CHECK(Record(out,capacity,id,"notes",SOURCE,"linux","Release","native","passed","3","3","0","0","0",DIGEST,"logs/test.txt")==0);
    }
    return 0;
}
static int EvaluateCase(const char *name)
{
    char ct[20000], et[100000]="UMICOM-RELEASE-EVIDENCE\t1\n";
    CHECK(MakeContract(ct,sizeof(ct),5U)==0);
    UmiReleaseContract *c=NULL;UmiReleaseEvidence *e=NULL;
    CHECK(UmiReleaseContractParse(ct,strlen(ct),&c)==UMI_STATUS_OK);
    UmiReleaseEvidenceAssessment result, original;
    memset(&result,0xA5,sizeof(result));original=result;
    unsigned expected=0U; size_t passed=0U,missing=4U;
    const char *candidate="notes",*source=SOURCE,*profile="linux",*config="Release",*kind="native",*outcome="passed",*total="3",*ok="3",*fail="0",*skip="0",*notRun="0",*digest=DIGEST,*reference="logs/test.txt",*id="r0";
    if(strcmp(name,"missing")==0) missing=5U;
    else if(strcmp(name,"full")==0) { CHECK(Standard(et,sizeof(et),5U)==0);passed=5U;missing=0U; }
    else if(strcmp(name,"subset")==0) { CHECK(Standard(et,sizeof(et),1U)==0);passed=1U; }
    else {
        if(strcmp(name,"candidate")==0) {candidate="different";expected=UMI_RELEASE_EVIDENCE_CONTEXT;}
        else if(strcmp(name,"source")==0) {source="2222222222222222222222222222222222222222";expected=UMI_RELEASE_EVIDENCE_CONTEXT;}
        else if(strcmp(name,"profile")==0) {profile="windows";expected=UMI_RELEASE_EVIDENCE_CONTEXT;}
        else if(strcmp(name,"configuration")==0) {config="Debug";expected=UMI_RELEASE_EVIDENCE_CONTEXT;}
        else if(strcmp(name,"kind")==0) {kind="review";expected=UMI_RELEASE_EVIDENCE_CONTEXT;}
        else if(strcmp(name,"failed")==0) {outcome="failed";ok="2";fail="1";expected=UMI_RELEASE_EVIDENCE_COUNTS|UMI_RELEASE_EVIDENCE_OUTCOME;}
        else if(strcmp(name,"skipped")==0) {outcome="skipped";ok="2";skip="1";expected=UMI_RELEASE_EVIDENCE_COUNTS|UMI_RELEASE_EVIDENCE_OUTCOME;}
        else if(strcmp(name,"not_run")==0) {outcome="not_run";ok="2";notRun="1";expected=UMI_RELEASE_EVIDENCE_COUNTS|UMI_RELEASE_EVIDENCE_OUTCOME;}
        else if(strcmp(name,"false_pass")==0) {ok="2";skip="1";expected=UMI_RELEASE_EVIDENCE_COUNTS;}
        else if(strcmp(name,"zero")==0) {total="0";ok="0";expected=UMI_RELEASE_EVIDENCE_COUNTS;}
        else if(strcmp(name,"digest")==0) {digest="-";expected=UMI_RELEASE_EVIDENCE_REFERENCE;}
        else if(strcmp(name,"reference")==0) {reference="-";expected=UMI_RELEASE_EVIDENCE_REFERENCE;}
        else if(strcmp(name,"unknown")==0) {id="not-required";}
        else { CHECK(false); }
        CHECK(Record(et,sizeof(et),id,candidate,source,profile,config,kind,outcome,total,ok,fail,skip,notRun,digest,reference)==0);
    }
    CHECK(UmiReleaseEvidenceParse(et,strlen(et),&e)==UMI_STATUS_OK);
    UmiStatus status=UmiReleaseEvidenceAssess(c,e,&result);
    if(strcmp(name,"unknown")==0) { CHECK(status==UMI_STATUS_NOT_FOUND);CHECK(memcmp(&result,&original,sizeof(result))==0); }
    else {
        CHECK(status==UMI_STATUS_OK);CHECK(result.required==5U && result.reportedComplete==passed && result.missing==missing);
        CHECK(result.distributionInput.blockers==5U-passed);
        CHECK(result.readyForOwnerReview==(passed==5U));
        if(expected!=0U) CHECK(result.reasons[0]==expected);
    }
    UmiReleaseEvidenceDestroy(e);UmiReleaseContractDestroy(c);return 0;
}
static int ParserCase(const char *name)
{
    char ct[40000],et[110000]="UMICOM-RELEASE-EVIDENCE\t1\n";
    CHECK(MakeContract(ct,sizeof(ct),5U)==0);
    UmiReleaseContract *c=NULL;UmiReleaseEvidence *e=NULL;
    if(strcmp(name,"null")==0) {
        CHECK(UmiReleaseContractParse(NULL,5U,&c)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiReleaseContractParse(ct,strlen(ct),NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiReleaseEvidenceParse(NULL,5U,&e)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiReleaseEvidenceParse(et,strlen(et),NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiReleaseEvidenceAssess(NULL,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiReleaseContractAt(NULL,0U)==NULL && UmiReleaseContractCount(NULL)==0U);
        UmiReleaseContractDestroy(NULL);UmiReleaseEvidenceDestroy(NULL);return 0;
    }
    if(strcmp(name,"capacity")==0 || strcmp(name,"over_capacity")==0) {
        size_t count=strcmp(name,"capacity")==0 ? 128U:129U;
        CHECK(MakeContract(ct,sizeof(ct),count)==0);
        UmiStatus status=UmiReleaseContractParse(ct,strlen(ct),&c);
        CHECK(status==(count==128U?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Standard(et,sizeof(et),count)==0);
        status=UmiReleaseEvidenceParse(et,strlen(et),&e);
        CHECK(status==(count==128U?UMI_STATUS_OK:UMI_STATUS_CAPACITY_EXCEEDED));
        if(count==128U) { UmiReleaseEvidenceAssessment a; CHECK(UmiReleaseEvidenceAssess(c,e,&a)==UMI_STATUS_OK && a.reportedComplete==128U); }
    } else if(strcmp(name,"missing_category")==0) {
        CHECK(MakeContract(ct,sizeof(ct),4U)==0);CHECK(UmiReleaseContractParse(ct,strlen(ct),&c)==UMI_STATUS_PARSE_ERROR && c==NULL);
    } else if(strcmp(name,"duplicate_contract")==0) {
        CHECK(Append(ct,sizeof(ct),"require\tr0\tother\tlinux\tRelease\tnative\tFW-01\tRepeated\n")==0);
        CHECK(UmiReleaseContractParse(ct,strlen(ct),&c)==UMI_STATUS_ALREADY_EXISTS && c==NULL);
    } else if(strcmp(name,"duplicate_evidence")==0) {
        CHECK(Standard(et,sizeof(et),1U)==0 && Standard(et,sizeof(et),1U)==0);
        CHECK(UmiReleaseEvidenceParse(et,strlen(et),&e)==UMI_STATUS_ALREADY_EXISTS && e==NULL);
    } else if(strcmp(name,"nul")==0 || strcmp(name,"control")==0 || strcmp(name,"non_ascii")==0) {
        size_t n=strlen(ct);ct[n-5U]=strcmp(name,"nul")==0?'\0':strcmp(name,"control")==0?'\x1b':(char)0xC0;
        CHECK(UmiReleaseContractParse(ct,n,&c)==UMI_STATUS_PARSE_ERROR && c==NULL);
    } else if(strcmp(name,"large_input")==0) {
        CHECK(UmiReleaseContractParse("x",UMI_RELEASE_TEXT_LIMIT+1U,&c)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiReleaseEvidenceParse("x",UMI_RELEASE_TEXT_LIMIT+1U,&e)==UMI_STATUS_CAPACITY_EXCEEDED);
    } else if(strcmp(name,"crlf")==0 || strcmp(name,"no_final_lf")==0 || strcmp(name,"owned")==0 || strcmp(name,"live_output")==0) {
        if(strcmp(name,"crlf")==0) {
            char expanded[45000];size_t w=0U;for(size_t i=0U;ct[i]!='\0';++i) {if(ct[i]=='\n') expanded[w++]='\r';expanded[w++]=ct[i];} expanded[w]='\0';memcpy(ct,expanded,w+1U);
        }
        if(strcmp(name,"no_final_lf")==0) ct[strlen(ct)-1U]='\0';
        CHECK(UmiReleaseContractParse(ct,strlen(ct),&c)==UMI_STATUS_OK);
        if(strcmp(name,"live_output")==0) { UmiReleaseContract *saved=c;CHECK(UmiReleaseContractParse(ct,strlen(ct),&c)==UMI_STATUS_INVALID_ARGUMENT && c==saved); }
        memset(ct,0,sizeof(ct));CHECK(UmiReleaseContractCount(c)==5U && strcmp(UmiReleaseContractCandidate(c),"notes")==0);
        CHECK(UmiReleaseContractAt(c,5U)==NULL && strcmp(UmiReleaseContractAt(c,0U)->id,"r0")==0);
    } else if(strcmp(name,"long_line")==0) {
        CHECK(Append(ct,sizeof(ct),"#")==0);size_t used=strlen(ct);memset(ct+used,'x',3000U);ct[used+3000U]='\0';
        CHECK(UmiReleaseContractParse(ct,strlen(ct),&c)==UMI_STATUS_CAPACITY_EXCEEDED);
    } else if(strcmp(name,"bad_header")==0) {
        ct[0]='X';CHECK(UmiReleaseContractParse(ct,strlen(ct),&c)==UMI_STATUS_PARSE_ERROR);
        CHECK(UmiReleaseEvidenceParse("WRONG\t1\n",8U,&e)==UMI_STATUS_PARSE_ERROR);
    } else {
        const char *total="3",*passed="3",*failed="0",*digest=DIGEST,*ref="logs/test.txt",*kind="native",*outcome="passed";
        if(strcmp(name,"negative")==0) total="-3";
        else if(strcmp(name,"overflow")==0) total="18446744073709551616";
        else if(strcmp(name,"sum_overflow")==0) {total="18446744073709551615";passed=total;failed="1";}
        else if(strcmp(name,"sum_mismatch")==0) total="4";
        else if(strcmp(name,"bad_hex")==0) digest="bad-hash";
        else if(strcmp(name,"path_parent")==0) ref="logs/../test.txt";
        else if(strcmp(name,"path_absolute")==0) ref="/tmp/test.txt";
        else if(strcmp(name,"unknown_kind")==0) kind="mock";
        else if(strcmp(name,"unknown_outcome")==0) outcome="probably-ok";
        else CHECK(false);
        CHECK(Record(et,sizeof(et),"r0","notes",SOURCE,"linux","Release",kind,outcome,total,passed,failed,"0","0",digest,ref)==0);
        CHECK(UmiReleaseEvidenceParse(et,strlen(et),&e)==UMI_STATUS_PARSE_ERROR && e==NULL);
    }
    UmiReleaseEvidenceDestroy(e);UmiReleaseContractDestroy(c);return 0;
}
static int Mutations(void)
{
    char original[20000];CHECK(MakeContract(original,sizeof(original),5U)==0);size_t n=strlen(original);
    unsigned state=1U;
    for(size_t i=0U;i<5000U;++i) {
        char text[20000];memcpy(text,original,n);state=state*1664525U+1013904223U;size_t at=(size_t)state%n;state=state*1664525U+1013904223U;text[at]=(char)(state&255U);
        UmiReleaseContract *c=NULL;UmiStatus s=UmiReleaseContractParse(text,n,&c);
        if(s==UMI_STATUS_OK) {CHECK(c!=NULL && UmiReleaseContractCount(c)>=5U);UmiReleaseContractDestroy(c);} else CHECK(c==NULL);
    }
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==3);
    if(strcmp(argv[1],"evaluate")==0) return EvaluateCase(argv[2]);
    if(strcmp(argv[1],"parser")==0) return ParserCase(argv[2]);
    if(strcmp(argv[1],"mutate")==0) return Mutations();
    return 1;
}
