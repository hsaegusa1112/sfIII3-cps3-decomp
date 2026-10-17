/*
 * TRAINING.C  Personal training mode
 *
 * Entered from the character select (hold 3P while confirming a 1P game).
 * Game2_1 calls training_frame() once per fight frame: it reads P1 pad
 * combos, keeps the match un-endable (no KO, timer frozen via Game_Main,
 * refills, full SA gauge, no stun), draws the dummy-mode label on the
 * text tilemap and drives the dummy's auto-guard. PLMAIN swaps the dummy
 * side's CPU_Sub call for train_dummy_sw() while a behavior is active.
 */

#include "structs.h"
#include "work.h"
#include "romdata.h"
#include "extern.h"
#include "PLSGAUGE.h"
#include "PLCNTDAT.h"
#include "textsound.h"
#include "training.h"

static const s8* Train_Dummy_Name[TD_COUNT] = {
    "STAND", "CROUCH", "BLOCK HI", "BLOCK LO", "RAND GRD", "JUMP", "CPU",
};

s8 Train_Mode = 0;
s8 Train_Dummy = TD_STAND;
static s16 Train_Frame = 0;
static s16 Train_Pad_Wait = 0;

static void training_refill(void) {
    setup_vitality(&plw[0].wu, 0);
    setup_vitality(&plw[1].wu, 1);
}

static void training_reset(void) {
    reset_fight_status();
    training_refill();
    clear_kizetsu_point(&plw[0]);
    clear_kizetsu_point(&plw[1]);
    erase_extra_plef_work();
}

void training_frame(void) {
    s16 i;
    Train_Frame++;

    /* P1 pad combos: hold 3P = cycle dummy, hold 3K = reset (both ~45f) */
    if ((p1sw_0 & 0x70) == 0x70) {
        if (Train_Pad_Wait == 45) {
            Train_Pad_Wait = -60;
            Train_Dummy++;
            if (Train_Dummy >= TD_COUNT) {
                Train_Dummy = TD_STAND;
            }
            training_refill();
        } else if (Train_Pad_Wait >= 0) {
            Train_Pad_Wait++;
        }
    } else if ((p1sw_0 & 0x380) == 0x380) {
        if (Train_Pad_Wait == 45) {
            Train_Pad_Wait = -60;
            if (plw[0].wu.routine_no[1] == 0 && plw[1].wu.routine_no[1] == 0) {
                training_reset();
            }
        } else if (Train_Pad_Wait >= 0) {
            Train_Pad_Wait++;
        }
    } else if (Train_Pad_Wait < 0) {
        Train_Pad_Wait++;
    } else {
        Train_Pad_Wait = 0;
    }

    /* the match can never end */
    PL_Wins[0] = 0;
    PL_Wins[1] = 0;

    /* dummy guard: auto_guard skips the hold-back check in HITCHECK, the
     * lever still decides high vs low */
    plw[1].auto_guard = 0;
    if (Train_Dummy == TD_BLOCK_HI || Train_Dummy == TD_BLOCK_LO) {
        plw[1].auto_guard = 1;
    }
    if (Train_Dummy == TD_RAND_GUARD && ((Train_Frame >> 5) & 1) == 0) {
        plw[1].auto_guard = 1;
    }

    /* health floor: when either side is about to die, refill both */
    if (plw[0].wu.vital_new <= 1 || plw[1].wu.vital_new <= 1) {
        training_refill();
    }

    /* SA always ready, stun never sticks */
    for (i = 0; i < 2; i++) {
        plw[i].sa->store = plw[i].sa->store_max;
        plw[i].sa->gauge.s.h = plw[i].sa->gauge_len;
        plw[i].sa->gauge.s.l = -1;
        clear_kizetsu_point(&plw[i]);
    }

    /* dummy-mode label, top center of the text tilemap */
    tilemap_clear_rect(16, 0, 10, 1);
    tilemap_print_string_attr(16, 0, 16, Train_Dummy_Name[Train_Dummy]);
}

u16 train_dummy_sw(PLW* wk) {
    switch (Train_Dummy) {
    case TD_CROUCH:
    case TD_BLOCK_LO:
        return 2;
    case TD_JUMP:
        if ((Train_Frame % 90) < 14) {
            return 1;
        }
        return 0;
    case TD_RAND_GUARD:
        if ((Train_Frame >> 5) & 1) {
            return 2;
        }
        return 0;
    default:
        return 0;
    }
}
