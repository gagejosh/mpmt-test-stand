#include "Arduino.h"

#include "Debug.h"
#include "macros.h"

#include "ArduinoSerialDevice.h"
#include "TestStandCommController.h"
#include "Messages.h"
#include "Axis.h"
#include "Movement.h"
#include "Kinematics.h"
#include "Thermistor10k.h"

#define COMM_BAUD_RATE 115200

ArduinoSerialDevice serial_device(Serial);
TestStandCommController comm(serial_device);

//will migrate pin assignment to separate file
static const int thermistor1Pin = A0; 
static const int thermistor2Pin = A1; 
static const int thermistor3Pin = A2; 
static const int thermistor4Pin = A3;
static const int thermistor5Pin = A4;

void setup()
{
    // put your setup code here, to run once:
    pinMode(LED_BUILTIN, OUTPUT);
    // thermistor pin configurations
    pinMode(thermistor1Pin, INPUT);
    pinMode(thermistor2Pin, INPUT);
    pinMode(thermistor3Pin, INPUT);
    pinMode(thermistor4Pin, INPUT);
    pinMode(thermistor5Pin, INPUT);

    DEBUG_INIT;
    serial_device.ser_connect(COMM_BAUD_RATE);

    setup_axis(&axis_x_config, &axis_x);
    setup_axis(&axis_y_config, &axis_y);

    analogReadResolution(12); //enable 12 bit resolution mode in Arduino Due. Default is 10 bit.
    Thermistor10k thermistor_ambient(thermistor1Pin);
    Thermistor10k thermistor_motor1(thermistor2Pin);
    Thermistor10k thermistor_mpmt(thermistor3Pin);
    Thermistor10k thermistor_motor2(thermistor4Pin);
    Thermistor10k thermistor_optical_box(thermistor5Pin);  
    //example usage: double temp1 = thermistor_ambient.readTemperature(); 
}

void handle_home()
{
    
}

void handle_move()
{
    uint32_t accel, hold_vel, dist;
    uint8_t axis, dir;

    uint8_t *data = comm.received_message().data;

    accel    = NTOHL(data);
    hold_vel = NTOHL(data + 4);
    dist     = NTOHL(data + 8);

    axis = data[12];
    dir = data[13];

    Axis *axis_ptr = (axis == AXIS_X ? &axis_x : &axis_y);
    VelProfile profile;
    generate_vel_profile(accel, axis_ptr->vel_min, hold_vel, dist, &profile);

    axis_trapezoidal_move_rel(axis_ptr, profile.counts_accel, profile.counts_hold, profile.counts_decel, (Direction)dir);
    
    // TODO delete this log message:
    comm.log(LL_INFO, "cts_a = %d, cts_h = %d, dist = %d, axis = %c, dir = %s",
        profile.counts_accel,
        profile.counts_hold,
        dist,
        (axis == AXIS_X ? 'x' : 'y'),
        (dir == DIR_POSITIVE ? "pos" : "neg"));
}

int count = 0;

void loop()
{
    if (comm.check_for_message()) {
        switch (comm.received_message().id) {
            case MSG_ID_HOME:
                break;
            case MSG_ID_MOVE:
                handle_move();
                break;
        }
    }
}
