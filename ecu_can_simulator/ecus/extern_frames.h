//
// Created by ashcon on 3/3/21.
//

#ifndef ECU_CAN_SIMULATOR_EXTERN_FRAMES_H
#define ECU_CAN_SIMULATOR_EXTERN_FRAMES_H

#include "can_frame.h"

#include "bs/abs_esp.h"
#include "ewm/ewm.h"
#include "gs/nag52.h"
#include "ms/engine.h"


// Engine frames
#include <MS_210.h>
#include <MS_212.h>
#include <MS_268.h>
#include <MS_2F3.h>
#include <MS_308.h>
#include <MS_312.h>
#include <MS_608.h>

// GS frames
#include <GS_218.h>
#include <GS_338.h>
#include <GS_418.h>

// ABS/ESP frames
#include <BS_200.h>
#include <BS_208.h>
#include <BS_270.h>
#include <BS_300.h>
#include <BS_328.h>

#include <EWM_230.h>

// For cruise control
#include <ART_258.h>

// Engine frames
extern MS_210 ms210;
extern MS_212 ms212;
extern MS_268 ms268;
extern MS_2F3 ms2F3;
extern MS_308 ms308;
extern MS_312 ms312;
extern MS_608 ms608;

// GS frames
extern GS_218 gs218;
extern GS_338 gs338;
extern GS_418 gs418;

extern EWM_230  ewm230;

// ABS/ESP frames
extern BS_200 bs200;
extern BS_208 bs208;
extern BS_270 bs270;
extern BS_300 bs300;
extern BS_328 bs328;

extern ART_258 art258;

#endif //ECU_CAN_SIMULATOR_EXTERN_FRAMES_H
