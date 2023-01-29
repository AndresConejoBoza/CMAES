#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "CMAES.h"



//Defining the app's variables.

MAESAgent sender, receiver;
Agent_Platform AP;
sysVars env;
CyclicBehaviour writingBehaviour, readingBehaviour;
Agent_Msg msg_writing, msg_reading;


//Functions related to the writing Behaviour//

void writingsetup(CyclicBehaviour* Behaviour, void* pvParameters) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->add_receiver(Behaviour->msg,receiver.AID(&receiver));
};

void writingaction(CyclicBehaviour* Behaviour, void* pvParameters) {
	printf("***Sending Message***\n");
	Behaviour->msg->send0(Behaviour->msg);
	vTaskDelay(1000);
};

//Write Wrapper:
void write(void* pvParameters) {
	writingBehaviour.msg = &msg_writing;
	writingBehaviour.setup = &writingsetup;
	writingBehaviour.action = &writingaction;
	writingBehaviour.execute(&writingBehaviour,&pvParameters);
};


//Functions related to the reading Behaviour

void readingaction(CyclicBehaviour* Behaviour, void* pvParameters) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->receive(Behaviour->msg,portMAX_DELAY);
	printf("***Message Received: Hello world***\n\n");
};

//Read Wrapper:
void read(void* pvParameters) {
	readingBehaviour.msg = &msg_reading;
	readingBehaviour.action = &readingaction;
	readingBehaviour.execute(&readingBehaviour,&pvParameters);
};


//Main
int sender_receiver() {
	printf("------Sender Receiver APP------ \n");
	//Constructors for each initialized class

	ConstructorAgente(&sender);
	ConstructorAgente(&receiver);
	ConstructorSysVars(&env);
	ConstructorAgent_Platform(&AP, &env);
	ConstructorAgent_Msg(&msg_writing, &env);
	ConstructorAgent_Msg(&msg_reading, &env);
	ConstructorCyclicBehaviour(&writingBehaviour);
	ConstructorCyclicBehaviour(&readingBehaviour);

	//Initializing the Agents and the Platform.
	sender.Iniciador(&sender, "Agent Sender", 1, 512);
	receiver.Iniciador(&receiver, "Agent Receiver", 3, 512);
	AP.Agent_Platform(&AP, "sender_receiver_platform");
	
	//Registering the Agents and their respective behaviour into the Platform
	AP.agent_init(&AP,&sender, &write);
	AP.agent_init(&AP,&receiver, &read);
	AP.boot(&AP);
	printf("CMAES booted successfully \n");
	printf("Initiating APP\n\n");

	// Start the scheduler so the created tasks start executing.
	vTaskStartScheduler(); //Might have to be changed because CSP Library takes control of the FreeRTOS scheduler
	for (;;);

	return 0;


};
