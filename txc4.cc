/*
 * txc4.cc
 *
 *  Created on: 4 Sep 2021
 *      Author: Leifh
 */


#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

/*
 * Derive the Txc1 class from CSimpleModule. In the tictoc1 network,
 * both the 'tic' and 'toc' modules are Txc1 objects, created by OMNeT++ at the beginning
 * of the simulation
 */

class Txc4 : public cSimpleModule
{
    private:
        int counter;

    protected:
        //The following redefined virtual functions holds the algorithm
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

//The module class need to be registered with OMNeT++
Define_Module(Txc4);

void Txc4::initialize()
{
    //Initialize is called at the beginning of the simulation.
    //To bootstrap the tic-toc-tic-toc process, one of the models needs
    //to send the first message. Let this be 'tic'

    WATCH(counter);
    //Read input marameter specified in tictoc4.ned
    counter = par("limit");
    if (par("sendMsgOnInit").boolValue() == true) {
        //Create and send first message on gate 'out'. 'tictocMsg' is an arbitary string
        // which will be the name of the message object'
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
        EV << "Sending initial message\n";
    }
}

void Txc4::handleMessage(cMessage *msg){
    //The handleMessage() method is called whenever a message arrives at the module. Here
    //we just send it to the other module, through gate 'out' because both 'tic' and 'toc' does the same,
    //the message will bounce between the two.
    counter--;
    if (counter == 0){
        EV << "Final message retrieved.";
        delete msg;
    }
    else{
        EV << "Received message `" << msg->getName() << "'. Counter is: "<< counter << ".  sending it out again\n";
        send(msg, "out");
    }

}

