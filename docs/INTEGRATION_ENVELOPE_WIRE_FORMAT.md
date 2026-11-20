![Umicom](../resources/brand/umicom-logo.svg)

# Umicom Framework — Bounded Inter-Application Envelope Transport

**Author / project originator:** Sammy Hegab  
**Organisation:** Umicom Foundation  
**Primary language:** C23  
**Source owner:** `umicom-framework`  
**Typical Windows development checkout:** `C:\umicom\Umicom-Applications\framework`

[Umicom SVG icon](../resources/brand/umicom-icon.svg) · [Windows icon](../resources/brand/umicom.ico)

## Purpose

The Framework already owns `UmiIntegrationEnvelope`, the Integration Registry,
route resolution, the dispatcher, the Integration Suite Runtime and the
process-instance readiness gate. Those components must not be copied into
Studio, Trader, Bank, TMS or Umicom OS. The missing connective capability is a
**bounded, architecture-independent envelope representation** for transferring
one of those existing application messages over a future local IPC channel.

This update adds such a representation to the *existing* `envelope.c` and
`envelope.h`. It also closes a pre-existing validation gap: the old envelope
validator checked whether three fields were empty but did not check whether
all fixed-capacity arrays terminated. The existing dispatcher calls that
validator before string comparison, so the strengthened contract protects the
current in-memory route as well as the future wire route.

**This is an actual wire codec and incremental stream decoder. It is NOT an
IPC transport, a cryptographic authentication layer or a claim that Bank/TMS
can already exchange external money/credentials over the network.** Real
Windows named-pipe or Linux Unix-socket connections, trusted peer identity,
process-token checks, message permissions and connection lifecycle remain
separate Framework-owned work.

## Existing architecture and new boundary

```text
     Umicom Studio         Umicom Bank          Umicom TMS
          |                    |                    |
          +--------------------+--------------------+
                               |
                   shared Umicom Framework
                               |
       Existing Integration Registry / Routes / Dispatcher
                               |
         Existing UmiIntegrationEnvelope in-memory model
                               |
               NEW bounded wire encode/decode
                   + sequence acceptance
                               |
               TRUSTED TRANSPORT HOST (future)
               /                            \
       Windows named pipes            Linux Unix sockets
       ACL + peer PID/token          SO_PEERCRED + process owner
               \                            /
                  Existing readiness gate
                 Existing Desk supervision
```

The wire data does not authenticate its claimed `source_application`. The
transport owner **must** verify the peer's operating-system credentials and
current tracked child generation *before* accepting the sequence or delivering
the decoded envelope to the current dispatcher. The application's own access
policy must authorise the topic and request type. A caller-supplied Boolean,
process ID, app ID or challenge echo is not sufficient authentication.

## Wire format

The representation has a fixed 32-byte header, followed by exactly five
length-delimited byte strings. Every numeric multi-byte field uses network
(big-endian) byte order, so a future x86-64, AArch64 or RISC-V host interprets
the same bytes. Native structure padding, pointers, machine-endian enum size,
NUL terminators and user passwords are never written implicitly.

| Byte offset | Size | Meaning |
|---|---:|---|
| 0 | 4 | Literal `UMIE` framing marker. |
| 4 | 1 | Format discriminator; current implementation writes `1`. This is not a development-batch suffix. |
| 5 | 1 | Message kind: command, event, query or response. |
| 6 | 2 | Reserved flags; zero and strictly checked. |
| 8 | 8 | Positive per-channel message sequence, big-endian. |
| 16 | 10 | Five big-endian 16-bit lengths in field order. |
| 26 | 6 | Reserved; zero and strictly checked. |
| 32 | Variable | Source, target, topic, correlation ID, payload; lengths dictate their boundaries. |

Every frame must end *exactly* after the payload; trailing bytes are handled as
the next frame by the stream decoder, never ignored within a one-frame parse.
Required source/target/topic IDs cannot be empty. Every identifier and payload
must fit in the **existing public envelope capacities**, including space for a
terminating NUL when decoded. Embedded NULs, unterminated fixed arrays,
non-printable control bytes in identities, unsupported kinds/flags, invalid
lengths, invalid format markers and unrecognised trailing data are refused.

**Format compatibility:** The format byte is an explicit protocol discriminator.
Future incompatible formats must have new, separately reviewed decoders. Do
not silently reinterpret older bytes as new wire semantics. This is not a
checksum, signature, tamper-evident ledger or authenticated transport.

## Public API ownership

The original `umi_integration_envelope_init` and
`umi_integration_envelope_validate` entry points and public struct layout are
preserved. The new C23 APIs use descriptive names without numerical suffixes:

| Function | What it does |
|---|---|
| `UmiIntegrationEnvelopeWireEncode` | Validates and encodes one complete frame; can measure needed space without writing. |
| `UmiIntegrationEnvelopeWireDecode` | Requires exactly one complete frame; copies an envelope and sequence atomically on success. |
| `UmiIntegrationWireSequenceWindowInit` | Initialises a replay window for a **new authenticated channel**. |
| `UmiIntegrationWireSequenceWindowAccept` | Rejects duplicate or decreasing sequence values without advancing state. |
| `UmiIntegrationWireDecoderInit` | Creates an empty, stack-allocated bounded streaming decoder. |
| `UmiIntegrationWireDecoderFeed` | Consumes up to one complete frame from a read chunk; handles partial and concatenated frames. |
| `UmiIntegrationWireDecoderFinish` | Checks for a complete frame boundary after the transport reports end of stream. |
| `UmiIntegrationWireDecoderReset` | Wipes buffered bytes and explicitly recovers from a poisoned decode session. |

No heap allocations or shell commands occur in these functions. They do not
call any network, process, GTK4, database or broker API. They reuse the
canonical `Umicom::integration` library target. No new runtime registry or
transport implementation is duplicated.

## Using a decoder in the transport host

```c
/* Conceptual integration: assume the transport host has already authenticated
 * a peer using OS facilities and associated it with an authorised process.
 * This example only decodes; it NEVER establishes peer authentication. */
UmiIntegrationWireDecoder decoder;
UmiIntegrationWireSequenceWindow window;
UmiIntegrationWireDecoderInit(&decoder);
UmiIntegrationWireSequenceWindowInit(&window);

/* For each set of bytes returned by an established trusted channel: */
size_t offset = 0U;
while (offset < readByteCount) {
    size_t consumed = 0U;
    bool complete = false;
    UmiIntegrationEnvelope envelope;
    uint64_t sequence = 0U;
    UmiStatus status = UmiIntegrationWireDecoderFeed(
        &decoder, readBytes + offset, readByteCount - offset,
        &consumed, &complete, &envelope, &sequence);
    if (status != UMI_STATUS_OK) {
        /* Abort this channel. Reset alone is not authorisation to resume a
         * poisoned connection; the owner must re-establish peer trust. */
        break;
    }
    offset += consumed;
    if (complete) {
        status = UmiIntegrationWireSequenceWindowAccept(&window, sequence);
        if (status != UMI_STATUS_OK) break;
        /* ALSO check this peer's authoritative app/process identity, its
         * allowed target/topic/kind, and any user authorisation BEFORE
         * calling umi_integration_dispatcher_dispatch(). */
    }
}
```

Identifiers and payloads remain **text fields**. Do not use this representation
for arbitrary binary attachments or secret-bearing bank documents. A separate
attachment/object stream should use the canonical Data Server's owned storage
and reference-based access controls. The codec does not validate JSON schemas,
UTF-8 semantics or application-specific amounts; those belong to their
appropriate typed domain handlers and parser contracts.

### Decide what happened when the byte stream ends

1. Feed all bytes returned by a successful read. A zero-length chunk is valid
   and preserves any partial frame. Keep reading when the transport reports
   that more bytes can arrive.
2. When the transport positively reports end of stream, call
   `UmiIntegrationWireDecoderFinish(&decoder)`. `UMI_STATUS_OK` means the input
   ended on a frame boundary. `UMI_STATUS_PARSE_ERROR` means an incomplete or
   rejected frame remained. The caller can still inspect its earlier copied
   replies and record the transport's exact close result.
3. Close the current channel and resolve any pending request through its
   supervisor. A clean boundary can occur before any reply arrives. If the
   worker may have acted before disappearing, keep the request's outcome
   unknown until the service can reconcile it. Reset the decoder before using
   it with a newly verified connection.

To exercise the decoder, use a configured Framework build with testing
enabled. Replace `$buildDir` with its existing build directory.

```powershell
cmake --build $buildDir --target umicom-integration-dispatcher-tests
ctest --test-dir $buildDir -R '^framework[.]integration[.]dispatcher$' --verbose --output-on-failure --no-tests=error
```

Expect output beginning with `Checked clean EOF` and a passing test. The test
stops after each possible incomplete prefix, then resumes a live decoder with
the remaining bytes. If CTest finds no matching test, check that the build
directory was configured with `BUILD_TESTING=ON`. A failure on the final
partial-frame check is a reason to look at decoder progress before retrying a
worker request.

### Keep a stable request identifier

A correlation ID is the text a requester uses to join a reply to its outstanding request. Use a session identity unique to the verified connection and keep one session counter for its requests. The counter starts at `1`. The allocator writes only a complete ID and never rolls over to use an earlier counter value. If the counter is exhausted, settle the current requests and establish a newly identified session.

1. Initialise `UmiIntegrationSession` with the connection's session identity and an identity for the local requester. If either string does not fit, the original session stays available.
2. Pass the request envelope's `correlation_id` array and its `sizeof` to `umi_integration_session_next_correlation`. If a long session name leaves insufficient room for its decimal suffix, the request and counter stay unchanged. Use a shorter unique session identity before opening a fresh connection.
3. Carry the original ID into the response, then check it together with the expected process instance, source, target and topic. A matching ID alone cannot authenticate a peer or authorise its operation. Keep sequence validation at the transport boundary.

Run the existing session test to check exact fit and exhaustion:

```powershell
cmake --build $buildDir --target umicom-integration-session-tests
ctest --test-dir $buildDir -R '^framework[.]integration[.]session$' --verbose --output-on-failure --no-tests=error
```

Expect the test to report complete correlation IDs and terminal counter exhaustion. If a short buffer changes the request text, inspect the caller's output capacity and check whether it writes into the session's own storage. Give the allocator a separate output buffer so future requests keep the original session identity.

### Accept the reply for the current connection

After checking the operating-system peer and confirming the target with the existing readiness gate, record the verified connection identity, process token and generation in `UmiIntegrationChannelPeer`. Assign a fresh connection identity on reconnect, even if the process still has the same generation.

1. Initialize one `UmiIntegrationReplyBinding` for the outstanding request. Call `UmiIntegrationReplyBindingBegin` before sending it. The binding copies the routing data and correlation ID so the sender can release the request buffer.
2. Decode the incoming frame and accept its sequence before calling `UmiIntegrationReplyBindingAccept`. Pass the live readiness gate and the verified connection metadata for the incoming data. Deliver the reply to the application only when the function returns `UMI_STATUS_OK`.
3. If the process exits or the connection closes, cancel any requests you cannot resolve. A late reply to a cancelled binding returns `UMI_STATUS_CANCELLED`. If the operation may already have changed data, use the application's reconciliation flow to find out what happened.

An unrelated response returns `UMI_STATUS_NOT_FOUND` and leaves the request pending. The wrong connection identity or an obsolete process generation returns `UMI_STATUS_PERMISSION_DENIED`. Look up the currently verified peer and readiness proof if a response is rejected after reconnect. Avoid replaying an unresolved request until its application can confirm the outcome.

Use `framework.integration.suite_runtime` to exercise a matching in-memory and encoded response, an unsupported wire version, local cancellation and a member restart. The fixture supplies synthetic peer evidence to the real readiness gate. Transport-level credential checks require their own run.

### Try the owned local child channel

The native fixture starts one known C worker with `UmiProcessChannelOpen`. Its dedicated standard input and output carry the same frames used by the in-memory test. The host retains the child identity, sends a fresh challenge from the operating system's random source and confirms the current member only after the exact response arrives on the owned channel. The child exposes two fixed queries for readiness and status. It also observes end-of-input so the host can qualify an ordinary exit.

From a configured native Windows or Linux Framework build with testing enabled, run:

```powershell
cmake --build $buildDir --target umicom-integration-native-channel-test
ctest --test-dir $buildDir -R '^framework[.]integration[.]native_channel[.]' --output-on-failure --no-tests=error
```

Expect five results for a valid reply, an unsupported wire version, a cancelled request, an incomplete frame at actual end of stream and a response for a different topic. If the readiness exchange times out, inspect whether the known fixture executable opened with its dedicated streams and stayed alive while serving the challenge. Check the test and worker were built together. If decoding fails after the child closes, look at the frame length before you retry a request. Use the known fixture built by this target because arbitrary executable paths could start unrelated programs.

The tests exercise owned child pipes. A separate transport with independently connecting clients needs its own authorization and peer-credential checks.

## Completion requirements and testing

- Legacy in-memory dispatcher use remains source-compatible.
- Malformed fixed-size envelope fields fail before dispatcher string comparison.
- Encoder output and length are unchanged when a frame cannot be published.
- Decoder output envelope and sequence are unchanged on malformed input.
- Oversized lengths, hidden NULs, unsupported message kinds and reserved bits
  fail before any message is routed.
- Partial header/body and concatenated messages are correctly handled.
- A malformed stream leaves its decoder poisoned until an explicit reset.
- Sequence-window acceptance is monotonic per authenticated connection; replay
  cannot advance the accepted sequence.
- Big-endian representation behaves identically across native architectures.
- GCC and Clang strict-warnings, memory/UB sanitizers and the registered
  `framework.integration.dispatcher` CTest are used to validate source behavior.

This update exercises the existing dispatcher test target; it does not create a
new executable or modify the Framework CMake graph. **A full Windows integrated
build, an actual Windows named-pipe session, an actual Linux Unix-socket session
and a cross-architecture replay exchange are not proven by the focused tests.**

## Follow-on implementation

The next Framework-owned integration increment should implement **trusted
local transport adapters** (with Windows pipe ACLs/peer verification and Linux
socket peer credentials), then bind their accepted peer identities to the
existing Desk process token/generation and the existing one-use readiness gate.
The native launcher remains the process owner, not the application module or
the new wire codec.

Never use a decoded `source_application`, wire sequence or other client-supplied
claim as proof of identity or permission. A Bank message is never permission to
initiate a payment, and a Trader message is never permission to place an order.

## Repository update policy

The sole source owner is the Framework checkout at
`C:\umicom\Umicom-Applications\framework`. Build and test there via the
Applications root, commit/push the Framework repository on `main`, and run the
existing native `umicom repo verify .` / `umicom repo lock .` from the parent
Applications repository before committing its gitlink.

Synchronise `C:\umicom\umicomOS\framework` with a clean fast-forward
`git pull --ff-only origin main`, then build/test and record the new Framework
revision in the OS parent with the same native lock command. Do not copy
Framework source into the OS checkout. Independently cloned standalone
applications with their own Framework submodules require their own pins;
`umicom-kernel` does not depend on Framework.
