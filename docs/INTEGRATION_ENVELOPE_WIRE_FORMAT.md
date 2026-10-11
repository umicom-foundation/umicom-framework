# Umicom Framework — Bounded Inter-Application Envelope Transport

**Author / project originator:** Sammy Hegab  
**Organisation:** Umicom Foundation  
**Primary language:** C23  
**Source owner:** `umicom-framework`  
**Typical Windows development checkout:** `C:\umicom\Umicom-Applications\framework`

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
