<!--
Umicom Applications Workspace Template
File: docs/adr/ADR-0008-broker-adapter-and-execution-staging.md

PURPOSE:
Record the safe boundary and delivery stages for optional broker adapters.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# ADR-0008: Broker adapter and staged execution

Status: Accepted

External brokers are integrated behind a broker-neutral stable C contract.
Vendor classes remain inside optional private adapters and third-party SDKs are
not redistributed. Delivery proceeds from read-only connection and market data
to paper execution, reconciliation and independent risk. Live trading remains
disabled until explicit operational gates are satisfied.
