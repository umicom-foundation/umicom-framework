/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/android/profile_name.c
 *
 * PURPOSE:
 *   Bound Java text conversion and delegate all profile-name decisions to Framework C.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/android/profile_name.h"
#include "umicom/security/profile_name.h"

jstring UmiAndroidProfileNameCanonical(JNIEnv *env, jstring input)
{
    if (env == NULL || (*env)->ExceptionCheck(env)) return NULL;
    if (input == NULL) return NULL;
    jsize length = (*env)->GetStringLength(env, input);
    if ((*env)->ExceptionCheck(env)) return NULL;
    if (length < 3 || (size_t)length >= UMI_LOCAL_PROFILE_NAME_CAPACITY) return NULL;
    jchar units[UMI_LOCAL_PROFILE_NAME_CAPACITY - 1U];
    (*env)->GetStringRegion(env, input, 0, length, units);
    if ((*env)->ExceptionCheck(env)) return NULL;
    char bytes[UMI_LOCAL_PROFILE_NAME_CAPACITY], canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY];
    /* GetStringUTFChars uses modified UTF-8, not a general UTF-8 byte span.
     * Read bounded UTF-16 instead. The existing policy is ASCII-only, so a
     * non-ASCII unit cannot be represented and is refused without narrowing. */
    for (jsize i = 0; i < length; ++i) {
        if (units[i] > 0x7fU) return NULL;
        bytes[i] = (char)units[i];
    }
    if (UmiLocalProfileNameBytes(bytes, (size_t)length, canonical) != UMI_STATUS_OK) return NULL;
    /* Successful policy output contains only ASCII and a terminating NUL.
     * This limited output is safe for NewStringUTF's modified UTF-8 contract. */
    return (*env)->NewStringUTF(env, canonical);
}
