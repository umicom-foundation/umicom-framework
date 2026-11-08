/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/android_profile_name/test_profile_name.c
 *
 * PURPOSE:
 *   Check bounded JNI calls and exception preservation with real JNI declarations and a controlled function table.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/android/profile_name.h"
#include <stdio.h>
#include <string.h>

static struct {
    jchar units[64];
    jsize length;
    unsigned lengthCalls, regionCalls, newCalls, exceptionCalls;
    int pending, lengthFailure, regionFailure, newFailure, newNull, badCall;
    char output[64];
} state;
static char inputTag, outputTag;
static jstring Input(void) { return (jstring)(void *)&inputTag; }
static jstring Output(void) { return (jstring)(void *)&outputTag; }
static jboolean JNICALL HasException(JNIEnv *env)
{
    (void)env;
    ++state.exceptionCalls;
    return state.pending ? JNI_TRUE : JNI_FALSE;
}
static jsize JNICALL Length(JNIEnv *env, jstring input)
{
    (void)env;
    ++state.lengthCalls;
    if (input != Input() || state.pending) state.badCall = 1;
    if (state.lengthFailure) state.pending = 1;
    return state.length;
}
static void JNICALL Region(JNIEnv *env, jstring input, jsize start, jsize length, jchar *out)
{
    (void)env;
    ++state.regionCalls;
    if (input != Input() || state.pending || start != 0 || length != state.length ||
        length < 0 || length > 48 || out == NULL) { state.badCall = 1; return; }
    if (state.regionFailure) { state.pending = 1; return; }
    memcpy(out, state.units, (size_t)length * sizeof *out);
}
static jstring JNICALL NewText(JNIEnv *env, const char *text)
{
    (void)env;
    ++state.newCalls;
    if (state.pending || text == NULL || strlen(text) >= sizeof state.output) {
        state.badCall = 1; return NULL;
    }
    if (state.newFailure) { state.pending = 1; return NULL; }
    if (state.newNull) return NULL;
    strcpy(state.output, text);
    return Output();
}
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    struct JNINativeInterface_ calls = {0};
    calls.ExceptionCheck = HasException;
    calls.GetStringLength = Length;
    calls.GetStringRegion = Region;
    calls.NewStringUTF = NewText;
    JNIEnv environment = &calls;
    const char *initial = "Sam_007";
    state.length = (jsize)strlen(initial);
    for (jsize i = 0; i < state.length; ++i) state.units[i] = (jchar)(unsigned char)initial[i];
    jstring input = Input();
    int accepted = 0, exceptionExpected = 0;
    unsigned expectedReads = 1U, expectedCreates = 0U;
    const char *expected = "sam_007";
    if (!strcmp(name, "valid")) accepted = 1;
    else if (!strcmp(name, "minimum")) { state.length = 3; accepted = 1; expected = "sam"; }
    else if (!strcmp(name, "maximum")) {
        state.length = 48;
        for (jsize i = 0; i < state.length; ++i) state.units[i] = 'A';
        accepted = 1;
    } else if (!strcmp(name, "long")) { state.length = 49; expectedReads = 0U; }
    else if (!strcmp(name, "empty")) { state.length = 0; expectedReads = 0U; }
    else if (!strcmp(name, "short")) { state.length = 2; expectedReads = 0U; }
    else if (!strcmp(name, "negative-length")) { state.length = -1; expectedReads = 0U; }
    else if (!strcmp(name, "null-input")) { input = NULL; expectedReads = 0U; }
    else if (!strcmp(name, "first-digit")) state.units[0] = '1';
    else if (!strcmp(name, "space")) state.units[3] = ' ';
    else if (!strcmp(name, "embedded-nul")) state.units[3] = 0;
    else if (!strcmp(name, "non-ascii")) state.units[3] = 0x00e9U;
    else if (!strcmp(name, "surrogate")) state.units[3] = 0xd800U;
    else if (!strcmp(name, "length-exception")) {
        state.lengthFailure = 1; expectedReads = 0U; exceptionExpected = 1;
    } else if (!strcmp(name, "region-exception")) { state.regionFailure = 1; exceptionExpected = 1; }
    else if (!strcmp(name, "new-exception")) { state.newFailure = 1; expectedCreates = 1U; exceptionExpected = 1; }
    else if (!strcmp(name, "new-null")) { state.newNull = 1; expectedCreates = 1U; }
    else if (!strcmp(name, "pending-exception")) {
        state.pending = 1; expectedReads = 0U; exceptionExpected = 1;
    } else if (!strcmp(name, "null-environment")) {
        REQUIRE(UmiAndroidProfileNameCanonical(NULL, input) == NULL);
        REQUIRE(state.exceptionCalls == 0U && state.lengthCalls == 0U && state.regionCalls == 0U && state.newCalls == 0U);
        return 0;
    } else return 2;
    jstring result = UmiAndroidProfileNameCanonical(&environment, input);
    REQUIRE(!state.badCall && state.pending == exceptionExpected);
    REQUIRE(state.regionCalls == expectedReads);
    REQUIRE(state.newCalls == (accepted ? 1U : expectedCreates));
    if (!strcmp(name, "pending-exception") || !strcmp(name, "null-input")) REQUIRE(state.lengthCalls == 0U);
    else REQUIRE(state.lengthCalls == 1U);
    REQUIRE(result == (accepted ? Output() : NULL));
    if (accepted) {
        if (!strcmp(name, "maximum")) {
            REQUIRE(strlen(state.output) == 48U);
            for (size_t i = 0U; i < 48U; ++i) REQUIRE(state.output[i] == 'a');
        } else REQUIRE(!strcmp(state.output, expected));
    }
    return 0;
}
