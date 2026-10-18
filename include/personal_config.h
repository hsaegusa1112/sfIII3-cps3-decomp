/*
 * PERSONAL_CONFIG.H — personal-mod tunables (see docs/PERSONAL.md)
 *
 * Input buffer: SF6-style generosity. A finished motion stays fireable
 * PBUF_MUL times longer than the arcade original. Applies only to slots
 * >= PBUF_MIN_SLOT (every per-character special / EX / SA command);
 * the universal slots below that (jump, dash, blocking, red-parry,
 * ukemi) keep arcade values so defensive mechanics are untouched.
 */

#define PBUF_ENABLE     1     /* 0 = arcade-original buffer */
#define PBUF_MUL        2     /* buffer window multiplier   */
#define PBUF_MIN_SLOT   16    /* scale slots >= this        */

/*
 * SF6-style motion shortcuts: when a motion step asks for forward, a
 * down-forward counts (so 323 / mirrored 121 triggers the dragon punch);
 * when it asks for down, a down-forward counts too (sloppy qcf starts).
 * Deliberately NOT down-back-for-down: that would let half-circle-forward
 * motions (Ibuki SA) misfire dragon punches.
 */
#define PSC_LENIENT     1     /* 0 = arcade-original motion matching */
