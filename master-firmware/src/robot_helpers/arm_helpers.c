#include <ch.h>

#include <math.h>

#include <error/error.h>

#include "motor_manager.h"
#include "motor_helpers.h"
#include "main.h"

#include "arm_helpers.h"

static motor_driver_t* get_motor_driver(const char* name)
{
    motor_driver_t* motor = (motor_driver_t*)bus_enumerator_get_driver(motor_manager.bus_enumerator, name);
    if (motor == NULL) {
        ERROR("Motor \"%s\" doesn't exist", name);
    }
    return motor;
}

static void set_index_stream_frequency(const char* motor, float freq)
{
    motor_driver_t* driver = get_motor_driver(motor);

    if (parameter_defined(&(driver->config.index_stream))) {
        parameter_scalar_set(&(driver->config.index_stream), freq);
    } else {
        ERROR("Undefined motor %s", motor);
    }
}

static void set_motor_position(const char* name, float pos)
{
    motor_driver_set_position(get_motor_driver(name), pos);
}

static void set_motor_velocity(const char* name, float vel)
{
    motor_driver_set_velocity(get_motor_driver(name), vel);
}

static void set_motor_torque(const char* name, float torque)
{
    motor_driver_set_torque(get_motor_driver(name), torque);
}

void arm_motors_index(const char** motors, const float* directions, const float* speeds, float* offsets)
{
    /* Enable index stream over CAN */
    for (size_t i = 0; i < 3; i++) {
        set_index_stream_frequency(motors[i], 100);
    }
    chThdSleepSeconds(2); // wait for motors to start streaming

    offsets[0] = motor_auto_index(motors[0], 1.f * directions[0], speeds[0]);
    offsets[1] = motor_auto_index(motors[1], -1.f * directions[1], speeds[1]);
    offsets[2] = motor_auto_index(motors[2], 1.f * directions[2], speeds[2]);

    /* Disable index stream over CAN */
    for (size_t i = 0; i < 3; i++) {
        set_motor_torque(motors[i], 0);
        set_index_stream_frequency(motors[i], 0);
    }

    float references[3] = {
        M_PI_2, // theta 1 indexes: at 90deg
        -M_PI, // theta 2 indexes: at -180deg
        M_PI, // theta 2 indexes: at 180deg
    };

    // axis are decoupled
    references[1] += references[0];
    references[2] += references[1];

    // We don't index at angle 0
    offsets[0] = directions[0] * offsets[0] - references[0];
    offsets[1] = directions[1] * offsets[1] - references[1];
    offsets[2] = directions[2] * offsets[2] - references[2];
}
