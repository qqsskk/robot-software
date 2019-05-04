#ifndef STRATEGY_IMPL_H
#define STRATEGY_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct _robot* robot;
    void (*wait_ms)(int);
} strategy_impl_t;

/** Stop moving */
void strategy_stop_robot(strategy_impl_t* strat);

/** Go to x,y,a position, avoiding any obstacle/opponent on the way
  * Returns false on failure, true otherwise
  */
bool strategy_goto_avoid(strategy_impl_t* strat, int x_mm, int y_mm, int a_deg, int traj_end_flags);

/** Same as above, but can be asked to retry a few times */
bool strategy_goto_avoid_retry(strategy_impl_t* strat, int x_mm, int y_mm, int a_deg, int traj_end_flags, int num_retries);

/** Put the robot 90 degrees from the wall facing the front sensors */
void strategy_align_front_sensors(strategy_impl_t* strat);

#ifdef __cplusplus
}
#endif

#endif /* STRATEGY_IMPL_H */
