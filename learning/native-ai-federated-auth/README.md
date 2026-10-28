# Authenticated Federated Frames — Native C23 Laboratory

**Umicom Framework · Sammy Hegab · Umicom Foundation · MIT**

This optional laboratory composes existing Framework **FederatedWire**, **FederatedJob**, **FederatedRound**, typed message and streaming SHA-256 components. Its only new cryptographic primitive is RFC 2104 **HMAC-SHA-256 built from the Framework SHA-256**. It deliberately does not add a second SHA-256 implementation or an application-local communication stack.

The standalone entry is `learning/native-ai-federated-auth/CMakeLists.txt`. It is opt-in and does not alter the Framework's root target graph. The test cases cover RFC 4231 HMAC vectors, valid assignment/result round-trips, tampering, rejected credentials/epochs, malformed frames, short output capacities, existing job-ticket replay rejection, and borrowed message-envelope semantics.

## Data flow

```text
Existing FederatedWireEncode
        ↓
Canonical UMFW frame + SHA-256 checksum
        ↓
FederatedAuthSeal (UMFA header, epoch, HMAC)
        ↓
Approved Framework owned message / authenticated channel (future adapter)
        ↓
FederatedAuthOpen → FederatedWireDecode
        ↓
External security identity and dataset authorisation
        ↓
FederatedWireMatchResult → FederatedJobComplete
        ↓
FederatedRoundFinalise → independent quality evaluation
```

**Security boundary:** HMAC detects alteration by parties without the shared secret under SHA-256/HMAC assumptions. It does not encrypt data, authenticate a particular human if keys are shared, prevent credential theft, prohibit replay after restart, establish a trustworthy dataset licence, prove local training happened, or certify useful learning. For real remote workers, use per-worker high-entropy secrets from approved Framework secret-provider facilities and TLS/mTLS or equivalent authenticated channels. Keep key material off public training ledgers and public fixtures.

The example uses a *synthetic*, public, deterministic demonstration key. Never use it in a deployed system. The full application suite and Windows UCRT64 build have not been qualified merely because this standalone laboratory passes on another host.

## Compatibility

The previous `federated_wire.*`, `federated_job.*`, `federated_round.*`, tokeniser, optimizer, sampler and training tape remain untouched. Existing script-based research tooling is preserved as an alternative where installed. All new functionality is native C23.

See `FRAME_CONTRACT.md` for the exact byte format and failure conditions.
