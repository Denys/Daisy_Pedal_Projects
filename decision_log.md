# Engineering Decision Log

This is the canonical append-only ADR record for the Delay Pedal, Multi-FX Pedal, shared platform, and Synth Harness program.

## Rules

- Do not delete, renumber, reorder, or silently rewrite an ADR.
- Use a new ADR for every accepted, rejected, deferred, or superseding decision.
- A reversal must identify the superseded ADR and preserve the old rationale.
- Metadata corrections are appended as correction notes.
- `state.md` tracks current progress; this file tracks durable rationale.
- An ADR records a decision, not proof that implementation or validation succeeded.
- Resolve `Supersedes` chains before treating a decision as active; an older immutable `Accepted` heading does not override a later superseding ADR.
- Every ADR includes an explicit validation status. `Required validation` describes a test and does not imply that the test ran.

## ADR-0001 — Accepted — Separate current state from durable decisions

- **Date:** 2026-07-28
- **Lane:** shared-platform
- **Context:** Long-running work spans multiple chats and mixes mutable progress with durable design rationale. Earlier summaries can be stale or inaccessible.
- **Decision:** Use root `state.md` as the primary continuity index and this append-only `decision_log.md` as the durable ADR record. Both must be read before substantive engineering responses. Every material run appends root state; an affected lane state is appended additionally when maintained.
- **Alternatives rejected:** Chat history as the primary state store; one mutable summary file containing both current state and overwritten decision rationale.
- **Evidence:** User directive dated 2026-07-28.
- **Validation status:** `NOT_RUN` — new-chat recovery has not been behaviorally evaluated.
- **Consequences:** Every material run must persist current state; every new or reversed decision must receive a new ADR. Neither file replaces primary implementation/test evidence.
- **Required validation:** A new chat must recover the current lane, accepted decisions, blockers, and next action without relying on the previous transcript.

## ADR-0002 — Accepted — Keep Delay, Multi-FX, and Synth Harness as distinct lanes

- **Date:** 2026-07-28
- **Lane:** shared-platform
- **Context:** Shared Daisy hardware, DSP primitives, UI, diagnostics, and test infrastructure create reuse opportunities but also encourage accidental product conflation.
- **Decision:** The Delay pedal and Multi-FX pedal remain separate products. Synth Harness remains a reusable ecosystem for users, developers, repairers, and small manufacturers, not a third pedal. Shared components are accepted only when they do not weaken product identity, performance, ergonomics, serviceability, or verification.
- **Alternatives rejected:** One oversized everything-pedal; treating Synth Harness as production testing only; copying all Delay features into Multi-FX.
- **Evidence:** User directive dated 2026-07-28.
- **Validation status:** `NOT_RUN` — cross-lane artifact behavior has not been behaviorally evaluated.
- **Consequences:** Every run declares one primary lane. Shared decisions must list their separate impact on all three lanes.
- **Required validation:** Requirements, architecture, UI, BOM, and test artifacts must retain lane ownership and must not silently import obsolete requirements from another lane.

## ADR-0003 — Accepted — English internal artifacts and Italian user conversation

- **Date:** 2026-07-28
- **Lane:** shared-platform
- **Context:** Mixed-language internal artifacts increase terminology drift, while Denys wants the interactive conversation in Italian.
- **Decision:** Use English for internal analysis, prompts, repository files, state, ADRs, reports, specifications, diagrams, tables, and generated artifacts. Use Italian only for visible conversation with Denys. Preserve source-language quotations, filenames, identifiers, and code.
- **Alternatives rejected:** Translating repository artifacts per chat; mixing Italian and English inside state and decision records.
- **Evidence:** User directive dated 2026-07-28.
- **Validation status:** `NOT_RUN` — normal, blocker, persistence-failure, and rollover responses have not been behaviorally evaluated.
- **Consequences:** Automation responses summarize work in Italian while linking English artifacts.
- **Required validation:** Future scheduled outputs contain Italian visible summaries and English persisted artifacts without changing technical identifiers.

## ADR-0004 — Accepted — Use re-fresh when conversation continuity degrades

- **Date:** 2026-07-28
- **Lane:** shared-platform
- **Context:** Long threads risk reviving obsolete decisions, mixing product lanes, promoting proposals to verified status, and carrying historical files as current authority.
- **Decision:** On a context-limit warning or observable continuity degradation, stop new engineering work and invoke the currently available `re-fresh` skill in standard mode. Rebuild a uniquely named, immutable handoff from current repository state, active non-superseded ADRs, lane state, and reachable sources. Do not claim a new chat exists unless the runtime confirms it.
- **Alternatives rejected:** Continuing from transcript fragments; automatic claims that a new chat was created; unlabelled summary-only handoffs.
- **Evidence:** User directive dated 2026-07-28.
- **Validation status:** `NOT_RUN` — skill availability, two same-day rollovers, supersession handling, and new-chat routing have not been behaviorally evaluated.
- **Consequences:** The user receives either a verified new-chat link/identifier or an explicit Italian instruction to open a new chat in the same Project and paste the handoff.
- **Required validation:** The handoff distinguishes `Observed`, `Decision`, `Pending`, and `Unverified`, names the next action and completion test, and lists obsolete paths to avoid.

## ADR-0005 — Accepted — Use an evidence-scoped DeviceCapabilityDescriptor boundary

- **Date:** 2026-07-29
- **Lane:** synth-harness
- **Supersedes:** none.
- **Context:** The current application selects five pedal carriers at compile time, while capability information is distributed across variant selection, hardware-module flags, initialized control vectors, switch metadata, persistent-storage metadata, and UI reachability. Diagnostics, editors, preset tools, fixtures, service tools, and future update tooling need one bounded identity/capability boundary without treating build intent as observed hardware identity.
- **Decision:** Adopt the versioned, immutable-after-publication `DeviceCapabilityDescriptor` v0.1 contract in `work_products/automation_runs/pedal_synth_harness_2026-07-29_device_capability_descriptor.md`. It reports compiled-application behavior separately from observed hardware, preserves explicit unknown states, includes preset-layout identity and fail-closed policy, and defines a bounded 238-byte canonical binary profile. v0.1 does not authorize carrier-dependent fixture actuation, flashing, calibration, pin-level access, or physical signal routing because it cannot provide verified observed-hardware identity/evidence.
- **Alternatives rejected:** Boolean-only capability flags; silent fallback to the default 125B target; using target/carrier build selection as proof of installed hardware; combining pin maps or mutable runtime state with the descriptor; JSON as the normative control format; preset mutation without exact application/settings/layout compatibility.
- **Evidence:** Repository base commit `87caaed73086a7767442aeb572b91fa97f96e5a8`; pinned source/blob inventory in the contract; current five carrier implementations; persistent-storage format/hash implementation; MIT repository license; frozen candidate SHA-256 `d688b30ceaf0b4abc3f7b17af48dceeb5ab393bcbd2616a3dd3f976c740e1bf7`.
- **Independent review:** two earlier frozen revisions returned `ACCEPT_WITH_REQUIRED_CHANGES`; the terminal fresh read-only review returned `ACCEPT`, with 0 critical, 0 major, 3 minor findings and confidence 0.94. The coordinator accepted the decision as a documentation contract and carried the minor items into required implementation validation.
- **Validation status:** `PASS` for the documentation-contract gate; `NOT_RUN` for firmware, parser, serialization, layout-hash execution, transport, and hardware behavior.
- **Consequences:** Synth Harness consumers gain a common fail-closed identity/capability contract. Firmware must centralize carrier constants and provide independent fixtures/tests before any transport exposure. Delay and Multi-FX products are not merged; each may consume the contract through its own target profile.
- **Required validation:** Build all five explicit target macros plus the intentional no-macro 125B default; reject multiple variants; execute parser/policy negative tests; compare any present descriptor layout hash with storage identity and fixed FNV vectors; add an independent 238-byte golden vector; prove all physical operations refuse under v0.1; expose no transport until the host contract passes.
