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
