# Native thread lifetime — engineering boundaries

## Corrected paths

The original control block was freed by `umi_thread_destroy` even when its entry
wrapper still needed to store the result. A deterministic ASan reproduction reports
heap-use-after-free in the POSIX wrapper. The corrected source retains separate
controller and worker references; one allocation remains, with no new global registry.

The POSIX sleep resumes `nanosleep` after EINTR using the remaining interval. Self-join
is refused before entering a native wait. All existing public names remain present.
The new checked release and nonblocking result observation are additive APIs.

## Responsibilities not transferred to Framework

* `user_data`, pointed-to buffers, windows and native TLS resources remain caller-owned.
  The reference count protects only the opaque Framework control block.
* One controller owns a handle. Concurrent join/release/poll through unsynchronised
  aliases remains invalid. This is not a public general-purpose reference-count API.
* Result availability establishes callback completion, not all native TLS teardown.
  A successful join establishes native termination. No bounded native-join claim is made.
* Release is not cancellation. Cooperative stop plus join is preferred for product
  shutdown. Forced Windows termination and direct ExitThread bypass the C wrapper and
  may leak its worker reference; these remain unsupported. POSIX cleanup cannot make
  arbitrary cancellation safe for callback locks or user-owned resources.
* Native release failure is visible through UmiThreadRelease and leaves its pointer
  owned. The void legacy destroy wrapper cannot report that error. A caller that drops
  its last pointer despite failure can still leak resources.
* The existing mutex recursion and condition-variable clock behaviours are unchanged.
  Thread identifiers are not durable identities. No ABI changes to public structures
  are introduced because all native thread state is opaque.

## Validation scope

31 focused CTest cases in Ninja builds: 28 native cases and three composition checks.
The Clang parent native tests are ASan/UBSan instrumented. Its three composition cases
compile and run separate Release child builds without sanitizer instrumentation.
On Windows, four POSIX cases and seven Linux wrapping cases are not registered;
20 focused cases are expected with Ninja. Other generators omit the three Ninja
composition cases and keep the native cases supported by the host.

The retained market-tape, replay and broker native selection passes 187/187. It uses
its earlier delivered dependency fixture with this thread implementation overlaid,
not the complete current repository. Two attempts to run its entire 198-case matrix
were stopped by the outer execution time limits during nested builds. Those partial
runs are retained but are NOT claimed as completed acceptance. No tests were removed
from the repositories or disabled to claim that larger suite passed.

Windows compilation/execution, GUI journeys, the full Applications/Framework/OS
builds, target-machine installation, ThreadSanitizer and forced shutdown testing
were not performed. This batch does not complete all platform qualification work.

## Preservation and merge

Only threading.c, threading.h and UmicomDesktopSystem.cmake replace existing files.
Every original line remains in order. Superseded functions/statements are preserved
in explained disabled blocks. No Umicoin development handoff is a prerequisite.
Framework hosts all implementation; Applications and umicomOS add only CMake entries.
