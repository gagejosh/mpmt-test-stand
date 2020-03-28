#ifndef PSEUDO_AXIS_H
#define PSEUDO_AXIS_H

#include "Arduino.h"
#include "PseudoEncoder.h"

typedef struct PseudoAxis
{
    PseudoEncoder encoder;
    uint32_t axis_length;  // specified in counts
    uint32_t motor_position_current;
} PseudoAxis;

extern PseudoAxis pseudo_axis_x;
extern PseudoAxis pseudo_axis_y;

#endif // PSEUDO_AXIS_H