#ifndef EXTERN_FRAMES_H_
#define EXTERN_FRAMES_H_

#include "flags.h"

#ifdef SIM_MODE
#include "can_frame.h"
#endif

#ifdef FW_MODE
#include <can_common.h>
#endif

#include "canframes/GS/GS_218.h"
#include "canframes/GS/GS_418.h"
#include "canframes/GS/GS_338.h"

#include "canframes/EWM/EWM_230.h"

#include "canframes/BS/BS_200.h"
#include "canframes/BS/BS_208.h"
#include "canframes/BS/BS_270.h"

#include "canframes/BS/BS_300.h"
#include "canframes/BS/BS_328.h"

#include "canframes/MS/MS_210.h"
#include "canframes/MS/MS_212.h"
#include "canframes/MS/MS_268.h"
#include "canframes/MS/MS_2F3.h"
#include "canframes/MS/MS_308.h"
#include "canframes/MS/MS_312.h"
#include "canframes/MS/MS_608.h"

#include "canframes/ART/ART_250.h"
#include "canframes/ART/ART_258.h"

extern GS_218 gs218;
extern GS_338 gs338;
extern GS_418 gs418;

extern EWM_230 ewm230;

extern BS_200 bs200;
extern BS_208 bs208;
extern BS_270 bs270;
extern BS_300 bs300;
extern BS_328 bs328;

extern MS_210 ms210;
extern MS_212 ms212;
extern MS_268 ms268;
extern MS_2F3 ms2f3;
extern MS_308 ms308;
extern MS_312 ms312;
extern MS_608 ms608;

extern ART_250 art250;
extern ART_258 art258;

#endif