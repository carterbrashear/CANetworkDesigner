#ifndef CANETWORKDESIGNER_STATEMACHINE_HPP
#define CANETWORKDESIGNER_STATEMACHINE_HPP

/**
 * @brief Main application state machine.
 */
class StateMachine {
    // Database State Machine
    enum class DBState {
        UNINITIALIZED,
        INITIALIZING,
        AVAILABLE,
        BUSY,
        ERROR
    };
    // Initial State = UNINITIALIZED
    DBState db_state = DBState::UNINITIALIZED;
public:
};

#endif //CANETWORKDESIGNER_STATEMACHINE_HPP
