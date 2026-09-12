# Physics-driven blade motion, but trace-based hit and block detection

An Attack's arm motion is driven by physics (joint drives chasing a live Target Point) so weight and momentum emerge naturally, rather than kinematic IK snapping the arm to position each frame. However, whether an Attack actually connects — and whether a Block actually intercepts it — is decided by a geometric trace/segment check against the blade's live position, not by literal rigid-body collision between the two blades.

We considered making it physics all the way down (real Chaos collision between the two simulated blades, no separate hit check), matching the "hyperrealistic" goal most literally. We rejected it: a thin, fast-moving blade can tunnel through another thin object within a single physics tick, and rigid-body sword-on-sword contact is a known source of unpredictable jank (clipping, sticking, wild ragdoll flails) in games that have tried it. Trace-based detection sampling the physically-simulated blade's true position each tick gets the weight and feel of real physics without inheriting its reliability problems.

There is deliberately no separate synthetic aim-check or distance-check layer on top of the trace: "too far" and "off target" are purely emergent from where the physically-driven blade tip actually travels.
