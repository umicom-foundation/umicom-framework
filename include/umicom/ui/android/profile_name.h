/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/android/profile_name.h
 *
 * PURPOSE:
 *   Adapt Java strings to the portable local-profile policy without retaining JNI state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ANDROID_PROFILE_NAME_H
#define UMICOM_UI_ANDROID_PROFILE_NAME_H
#include <jni.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Called on a JVM-attached thread with that thread's JNIEnv. The input and
 * environment are borrowed for this call only. Returns a new local Java string
 * on success. Invalid names or NULL input return NULL without throwing; a VM
 * exception is preserved and also returns NULL. The caller must distinguish
 * refusal from allocation/VM failure using ordinary Java exception handling.
 *
 * Reads at most 48 UTF-16 code units. ASCII conversion is representation work;
 * all naming and canonicalisation rules remain in UmiLocalProfileNameBytes.
 * Does not authenticate, save, access the network or create global references. */
jstring UmiAndroidProfileNameCanonical(JNIEnv *env, jstring input);
#ifdef __cplusplus
}
#endif
#endif
