#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "CMAES.h"

//For CSP

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>



//Defining the app's variables. Other variables might be defined as global variables in CMAES.h

MAESAgent sender, receiver;
Agent_Platform AP;
sysVars env;

//Functions related to the writing Behaviour//

void writingsetup(CyclicBehaviour* Behaviour, void* pvParameters) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->add_receiver(Behaviour->msg,receiver.AID(&receiver));
};


CSP_DEFINE_TASK(CSP_writingaction) {
	printf("***Sending Message***\n");
	writingBehaviour.msg->send0(writingBehaviour.msg);
	vTaskDelay(1000);
	return CSP_TASK_RETURN;
};

//Write Wrapper:
void write(void* pvParameters) {
	writingBehaviour.msg = &msg_writing;
	writingBehaviour.setup = &writingsetup;
	writingBehaviour.action = &CSP_writingaction;
	writingBehaviour.execute(&writingBehaviour,&pvParameters);
};


//Functions related to the reading Behaviour

CSP_DEFINE_TASK(CSP_readingaction) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->receive(Behaviour->msg, portMAX_DELAY);
	printf("***Message Received: Hello world***\n\n");
	return CSP_TASK_RETURN;
};

//Read Wrapper:
void read(void* pvParameters) {
	readingBehaviour.msg = &msg_reading;
	readingBehaviour.action = &CSP_readingaction;
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
