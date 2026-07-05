# Croonies Chat Safety Backend Notes

Current in-game build includes:
- World, team, and private chat routing through Unreal server RPCs.
- Local profanity masking before delivery.
- Parental toggles for chat, private chat, reporting, and profanity filtering.
- `/report PlayerName reason` command with recent chat context.
- Local report file output under `Saved/Reports`.
- Identity status labels: Local Guest, Pending Platform Validation, Verified, Failed.

Backend/platform work still required before public release:
- Google Play Games Services token validation against a trusted backend.
- Apple Game Center identity validation against a trusted backend.
- Server-side profanity/moderation service with audit logs.
- Report upload endpoint with authenticated reporter/target ids.
- Parental consent/account age rules tied to platform account or parent account.
- Abuse tooling for review, mute, suspend, and ban actions.

The game currently treats non-local platform sign-in as `Pending Platform Validation` until real mobile app IDs, entitlements, and backend validation are configured.
