#include <ch.h>
#include <stdio.h>
#include "robot_parameters.h"
#include "config.h"

parameter_namespace_t global_config;
struct slave_config_s slave_configs[SLAVE_CONFIG_COUNT];

static parameter_namespace_t master_config;
static parameter_namespace_t slave_config_root;

static parameter_t foo;

static parameter_namespace_t odometry_config;
static parameter_t odometry_wheel_base;
static parameter_t odometry_left_radius;
static parameter_t odometry_right_radius;

static void pid_register(struct pid_parameter_s *pid,
                         parameter_namespace_t *parent, const char *name)
{

    parameter_namespace_declare(&pid->root, parent, name);
    parameter_scalar_declare(&pid->kp, &pid->root, "kp");
    parameter_scalar_declare(&pid->ki, &pid->root, "ki");
    parameter_scalar_declare(&pid->kd, &pid->root, "kd");
    parameter_scalar_declare(&pid->ilimit, &pid->root, "ilimit");
}


void config_init(void)
{
    int i;
    parameter_namespace_declare(&global_config, NULL, NULL);
    parameter_namespace_declare(&master_config, &global_config, "master");

    parameter_namespace_declare(&odometry_config, &master_config, "odometry");
    parameter_scalar_declare_with_default(&odometry_wheel_base,
                                          &odometry_config,
                                          "wheelbase",
                                          ROBOT_EXTERNAL_WHEELBASE);

    parameter_scalar_declare_with_default(&odometry_right_radius,
                                          &odometry_config,
                                          "radius_right",
                                          ROBOT_RIGHT_EXTERNAL_WHEEL_RADIUS);

    parameter_scalar_declare_with_default(&odometry_left_radius,
                                          &odometry_config,
                                          "radius_left",
                                          ROBOT_LEFT_EXTERNAL_WHEEL_RADIUS);

    parameter_namespace_declare(&slave_config_root, &global_config, "slaves");

    for (i = 0; i < SLAVE_CONFIG_COUNT; ++i) {
        sprintf(slave_configs[i].name, "%d", i);

        parameter_namespace_declare(&slave_configs[i].root,
                                    &slave_config_root,
                                    slave_configs[i].name);

        parameter_namespace_declare(&slave_configs[i].pid_root,
                                    &slave_configs[i].root, "pid");

        pid_register(&slave_configs[i].speed_pid, &slave_configs[i].pid_root, "speed");
        pid_register(&slave_configs[i].position_pid, &slave_configs[i].pid_root, "position");
        pid_register(&slave_configs[i].current_pid, &slave_configs[i].pid_root, "current");
    }

    parameter_scalar_declare(&foo, &master_config, "foo");
}

float config_get_scalar(const char *id)
{
    parameter_t *p;

    p = parameter_find(&global_config, id);

    if (p == NULL) {
        char err_msg[64];
        snprintf(err_msg, sizeof err_msg, "Unknown param %s", id);
        chSysHalt(err_msg);
    }

    return parameter_scalar_get(p);
}
