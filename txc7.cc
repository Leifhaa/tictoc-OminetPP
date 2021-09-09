/*
 * txc7.cc
 *
 *  Created on: 9 Sep 2021
 *      Author: Leifh
 */


#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;


class Txc7 : public cSimpleModule
{
    private:
        cMessage *event; // pointer to the event object which we'll use for timing
        cMessage *tictocMsg; // variable to remember the message until we send it back

    protected:
        //The following redefined virtual functions holds the algorithm
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

//The module class need to be registered with OMNeT++
Define_Module(Txc7);

void Txc7::initialize()
{
    //Initialize is called at the beginning of the simulation.
    //To bootstrap the tic-toc-tic-toc process, one of the models needs
    //to send the first message. Let this be 'tic'
    event = new cMessage("event");

    //no tic toc message yet.
    tictocMsg = nullptr;

    if (strcmp("tic", getName()) == 0) {
        // We don't start right away, but instead send an message to ourselves
        // (a "self-message") -- we'll do the first sending when it arrives
         // back to us, at t=5.0s simulated time.
        EV << "Scheduling first send to t=5.0s\n";
        tictocMsg = new cMessage("tictocMsg");
        scheduleAt(5.0, event);
    }
}

void Txc7::handleMessage(cMessage *msg){
    if (msg == event){
        //Retrieved a self-message.
        EV << "Wait period is over, sending back message\n";
        send(tictocMsg, "out");
        tictocMsg = nullptr;
    }
    else{
        //Simulate sometimes "losing" a packet
        if (uniform(0, 1) < 0.1){
            EV << "\"Losing\" message\n";
            delete msg;
        }
        else{
            //Set delay time
            simtime_t delay = par("delayTime");

            //Retrieved message from partner. We remember its
            // pointer in the tictocMsg variable, then schedule our self-message
                // to come back to us in 1s simulated time.
            EV << "Message arrived, starting to wait " << delay << " sec...\n";

            tictocMsg = msg;
            scheduleAt(simTime()+ delay, event);
        }
    }
}
