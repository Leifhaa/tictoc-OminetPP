/*
 * txc8.cc
 *
 *  Created on: 9 Sep 2021
 *      Author: Leifh
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;


class Tic8 : public cSimpleModule
{
    private:
        simtime_t timeout; //timeout
        cMessage *timeoutEvent;

    protected:
        //The following redefined virtual functions holds the algorithm
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

    public:
        Tic8();
        virtual ~Tic8();
};

//The module class need to be registered with OMNeT++
Define_Module(Tic8);


Tic8::Tic8()
{
    timeoutEvent = nullptr;
}

Tic8::~Tic8()
{
    cancelAndDelete(timeoutEvent);
}

void Tic8::initialize()
{
    //Initialize is called at the beginning of the simulation.
    //To bootstrap the tic-toc-tic-toc process, one of the models needs
    //to send the first message. Let this be 'tic'
    timeout = 1.0;
    timeoutEvent = new cMessage("event");


    EV << "Scheduling first send to t=5.0s\n";
    cMessage *msg = new cMessage("tictocMsg");
    send(msg, "out");
    scheduleAt(simTime()+timeout, timeoutEvent);
}

void Tic8::handleMessage(cMessage *msg){
    if (msg == timeoutEvent){
        //Retrieved a timeout event, which means packet has not arrived in time and we have to resend it
        EV << "Timeout expired, resending message and restarting timer \n";
        cMessage *newMsg = new cMessage("tictocMsg");
        send(newMsg, "out");
        scheduleAt(simTime() + timeout, timeoutEvent);
    }
    else{
        //Message arrived. Acknowledge recieved -- delete the recieved message and cancel the timeout event
        EV << "Timer cancelled.\n";
        cancelEvent(timeoutEvent);
        delete msg;


        //Ready to send another one
        cMessage *newMsg = new cMessage("tictocMsg");
        send(newMsg, "out");
        scheduleAt(simTime()+timeout, timeoutEvent);
    }
}

/**
 * Sends back an acknowledgement -- or not.
 */
class Toc8 : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Toc8);

void Toc8::handleMessage(cMessage *msg)
{
    if (uniform(0, 1) < 0.1) {
        EV << "\"Losing\" message.\n";
        bubble("message lost");  // making animation more informative...
        delete msg;
    }
    else {
        EV << "Sending back same message as acknowledgement!!\n";
        send(msg, "out");
    }
}

