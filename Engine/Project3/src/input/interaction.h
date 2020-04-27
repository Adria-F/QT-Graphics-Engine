#ifndef INTERACTION_H
#define INTERACTION_H

class Interaction
{
public:

    bool update();

    void postUpdate();


private:

    bool idle();
    bool navigate();
    bool focus();


    enum State { Idle, Navigating, Focusing };
    State state = State::Idle;
    State nextState = State::Idle;
};

#endif // INTERACTION
