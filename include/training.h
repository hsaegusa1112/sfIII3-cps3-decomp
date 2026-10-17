/*
 * TRAINING.H — personal training mode (see docs/PERSONAL.md)
 *
 * Enter: coin in, pick a character, hold 3P (all three punches) while
 * confirming — the fight becomes you vs a mirror dummy with the timer
 * frozen, no KO, refills, full SA gauge and no stun.
 *
 * In fight (P1): hold 3P ~3/4 second = cycle dummy behavior,
 *                hold 3K ~3/4 second = reset positions and health.
 */

#define TD_STAND       0
#define TD_CROUCH      1
#define TD_BLOCK_HI    2
#define TD_BLOCK_LO    3
#define TD_RAND_GUARD  4
#define TD_JUMP        5
#define TD_CPU         6
#define TD_COUNT       7

extern s8 Train_Mode;    /* 1 while a training fight is running */
extern s8 Train_Dummy;   /* current dummy behavior index        */

void training_frame(void);          /* per-fight-frame upkeep (Game2_1) */
u16 train_dummy_sw(PLW* wk);        /* dummy pad word (PLMAIN CPU path) */
