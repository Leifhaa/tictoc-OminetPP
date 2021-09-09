/*
 * txc9.cc
 *
 *  Created on: 9 Sep 2021
 *      Author: Leifh
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <string.h>

using namespace omnetpp;


class Tic9 : public cSimpleModule
{
    private:
        simtime_t timeout; //timeout
        cMessage *timeoutEvent;
        int seq;
        cMessage *message;  // message that has to be re-sent on timeout

    protected:
        //The following redefined virtual functions holds the algorithm
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual cMessage *generateNewMessage();
        virtual void sendCopyOf(cMessage *msg);

    public:
        Tic9();
        virtual ~Tic9();
};

//The module class need to be registered with OMNeT++
Define_Module(Tic9);


Tic9::Tic9()
{
    timeoutEvent = nullptr;
    message = nullptr;
}

Tic9::~Tic9()
{
    cancelAndDelete(timeoutEvent);
    delete message;
}

void Tic9::initialize()
{
    seq = 0;
    timeout = 1.0;
    timeoutEvent = new cMessage("event");


    EV << "Sending initial message\n";
    message = generateNewMessage();
    sendCopyOf(message);
    scheduleAt(simTime()+timeout, timeoutEvent);
}

cMessage *Tic9::generateNewMessage(){
    //Generate a message with a different name every time
    char msgName[20];
    sprintf(msgName, "tic-%d", ++seq);
    cMessage *msg = new cMessage(msgName);
    return msg;
}

void Tic9::sendCopyOf(cMessage *msg){
    //Duplicate message
    cMessage *copy = (cMessage *)msg->dup();
    //send the message
    send(copy, "out");
}

void Tic9::handleMessage(cMessage *msg){
    if (msg == timeoutEvent){
        //Retrieved a timeout event, which means packet has not arrived in time and we have to resend it
        EV << "Timeout expired, resending message and restarting timer \n";
        sendCopyOf(message);
        scheduleAt(simTime() + timeout, timeoutEvent);
    }
    else{
        delete msg;

        //Message arrived. Acknowledge recieved -- delete the recieved message and cancel the timeout event
        EV << "Timer cancelled.\n";
        cancelEvent(timeoutEvent);
        delete message;



        //Ready to send another one
        message = generateNewMessage();
        sendCopyOf(message);
        scheduleAt(simTime()+timeout, timeoutEvent);
    }
}

/**
 * Sends back an acknowledgement -- or not.
 */
class Toc9 : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Toc9);

void Toc9::handleMessage(cMessage *msg)
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
