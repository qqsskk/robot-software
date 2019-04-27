#include <CppUTest/TestHarness.h>
#include <vector>
#include <array>

#include "strategy/goals.h"
#include "strategy/actions.h"

bool dummy_execute(goap::Action<RobotState>* action, RobotState& state)
{
    action->plan_effects(state);
    return true;
}

struct IndexArms : actions::IndexArms {
    bool execute(RobotState& state) { return dummy_execute(this, state); }
};
struct RetractArms : actions::RetractArms {
    bool execute(RobotState& state) { return dummy_execute(this, state); }
};
struct TakePuck : actions::TakePuck {
    TakePuck(size_t id) : actions::TakePuck(id) {}
    bool execute(RobotState& state) { return dummy_execute(this, state); }
};
struct DepositPuck : actions::DepositPuck {
    DepositPuck(size_t id) : actions::DepositPuck(id) {}
    bool execute(RobotState& state) {
        pucks_in_area++;
        return dummy_execute(this, state);
    }
};
struct LaunchAccelerator : actions::LaunchAccelerator {
    bool execute(RobotState& state) { return dummy_execute(this, state); }
};
struct TakeGoldonium : actions::TakeGoldonium {
    bool execute(RobotState& state) { return dummy_execute(this, state); }
};

TEST_GROUP (Strategy) {
    RobotState state = initial_state();

    IndexArms index_arms;
    RetractArms retract_arms;
    TakePuck take_pucks[4] = {{0}, {1}, {2}, {6}};
    DepositPuck deposit_puck[3] = {{0}, {1}, {2}};
    LaunchAccelerator launch_accelerator;
    TakeGoldonium take_goldonium;

    std::vector<goap::Action<RobotState>*> availableActions()
    {
        return std::vector<goap::Action<RobotState>*>{
            &index_arms,
            &retract_arms,
            &take_pucks[0],
            &take_pucks[1],
            &take_pucks[2],
            &take_pucks[3],
            &deposit_puck[0],
            &deposit_puck[1],
            &deposit_puck[2],
            &launch_accelerator,
            &take_goldonium,
        };
    }

    int compute_and_execute_plan(goap::Goal<RobotState> & goal, RobotState & state)
    {
        const int max_path_len = 40;
        goap::Action<RobotState>* path[max_path_len] = {nullptr};
        goap::Planner<RobotState, GOAP_SPACE_SIZE> planner;
        auto actions = availableActions();

        int len = planner.plan(state, goal, actions.data(), actions.size(), path, max_path_len);
        for (int i = 0; i < len; i++) {
            path[i]->execute(state);
        }

        return len;
    }
};

TEST(Strategy, CanInitArms)
{
    InitGoal init_goal;

    int len = compute_and_execute_plan(init_goal, state);

    CHECK_TRUE(len > 0);
    CHECK_TRUE(init_goal.is_reached(state));
}

TEST(Strategy, CanFillRedPuckArea)
{
    RedPucksGoal goal;

    int len = compute_and_execute_plan(goal, state);

    CHECK_TRUE(len > 0);
    CHECK_TRUE(goal.is_reached(state));
}

TEST(Strategy, CanFillGreenPuckArea)
{
    GreenPucksGoal goal;

    int len = compute_and_execute_plan(goal, state);

    CHECK_TRUE(len > 0);
    CHECK_TRUE(goal.is_reached(state));
}

TEST(Strategy, CanFillBluePuckArea)
{
    BluePucksGoal goal;

    int len = compute_and_execute_plan(goal, state);

    CHECK_TRUE(len > 0);
    CHECK_TRUE(goal.is_reached(state));
}

TEST(Strategy, CanRunAllGoals)
{
    RedPucksGoal red_pucks;
    GreenPucksGoal green_pucks;
    AcceleratorGoal accelerator;
    goap::Goal<RobotState>* goals[] = {
        &red_pucks,
        &green_pucks,
        &accelerator,
    };

    for (auto& goal : goals) {
        int len = compute_and_execute_plan(*goal, state);
        CHECK_TRUE(len > 0);
        CHECK_TRUE(goal->is_reached(state));
    }
}
