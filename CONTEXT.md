# Sword Duel

A first-person, single-player 1v1 melee sword duel against an AI opponent. No RPG progression, no networked multiplayer — the focus is the physical feel of a single sword exchange.

## Language

**Duel**:
The full combat encounter between the Player and the AI Opponent, running until one Combatant's Health reaches zero.
_Avoid_: Match, fight, round (there are no rounds — a Duel is single-life)

**Combatant**:
Either participant in a Duel — the Player or the AI Opponent. Both are bound by identical rules; neither has mechanics the other lacks.
_Avoid_: Player, character (when the point being made applies to both sides equally)

**Attack**:
An offensive action: a Combatant drives a Target Point across their Reach Sphere in real time, and their arm physically follows it. Speed and direction are read continuously, not chosen from a list of moves.
_Avoid_: Swing (as a noun for the action itself — use Attack; "swing" is fine only when describing the blade's physical motion)

**Block**:
A defensive action: a Combatant continuously positions a Target Point to intercept an incoming Attack's path. Unlike an Attack, a Block has no Commit point — it can be freely repositioned for as long as it's held.
_Avoid_: Parry, deflect (not yet part of this domain — those imply timing-window mechanics that are explicitly out of scope for now)

**Guard**:
The neutral stance a Combatant returns to when performing neither an Attack nor a Block.
_Avoid_: Idle, rest, stance (used generically) — Guard is the specific named state

**Target Point**:
The point in space a Combatant's blade tip is currently being driven toward — set by live mouse/pen input for the Player, and by the AI's decision-making for the AI Opponent. The same concept drives both Attack and Block.

**Reach Sphere**:
The reachable volume, anchored at a Combatant's shoulder, across which a Target Point can move. Its radius is bounded by that Combatant's arm and weapon length — it defines what's physically reachable, not what's a legal move.

**Commit**:
The moment an Attack's traced speed crosses the threshold beyond which it can no longer be aborted back to Guard. Before Commit, an Attack is still being shaped; after Commit, it will play out to resolution (hit, miss, or Blocked).
_Avoid_: Wind-up (that's the phase before Commit, not the moment itself)

**Wind-up**:
The portion of an Attack before Commit, where a Combatant is still shaping direction and speed and no hit can register yet.

**Recovery**:
The period after an Attack resolves during which a Combatant's arm settles back toward Guard. Recovery has no fixed duration — a faster, more extended Attack takes longer to recover from.

**Exposure**:
The vulnerability a Combatant incurs while in Recovery, particularly after a missed Attack. A fast, missed Attack produces more Exposure than a slow one, because it commits to a larger motion to settle out of.

**Health**:
The numeric state that ends the Duel for whichever Combatant it reaches zero for first. Currently shown as a simple bar; not intended as the final presentation.
