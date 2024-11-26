#ifndef SENSORCONTROL_H
#define SENSORCONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sensorControlConfig.h"

enum {
    MENU_SET_TIME,
    MENU_SET_DATE,
};

enum {
    CANCEL,
    APPLY,
};

void sensorControlInit(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
