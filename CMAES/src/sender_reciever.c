#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeMAES.h"

/* Demo includes. */
#include "supporting_functions.h"



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
	printf("Enviando mensaje... \n");
	Behaviour->msg->send0(Behaviour->msg);
	vTaskDelay(pdMS_TO_TICKS(1000));
};

void write(void* pvParameters) {
	//printf("\nEntreeeeee al write\n");
	writingBehaviour.msg = &msg_writing;
	writingBehaviour.setup = &writingsetup;
	writingBehaviour.action = &writingaction;
	writingBehaviour.execute(&writingBehaviour,&pvParameters);
};


//Functions related to the reading Behaviour


void readingaction(CyclicBehaviour* Behaviour, void* pvParameters) {
	//printf("Entre a readingaction\n");
	printf("Esperando... \n");
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->receive(Behaviour->msg,portMAX_DELAY);
	printf("Mensaje recibido: ¡Hola MAES! \n");
};

void read(void* pvParameters) {
	/*
	printf("direccion recibida del behaviour: %p\n", &Behaviour);
	CyclicBehaviour* readingBehaviour = &(CyclicBehaviour*)Behaviour;
	printf("Direccion del readingbehaviour: %p\n", &readingBehaviour);
	printf("Direccion del puntero hacia readingbehaviouraction: %p\n", &readingBehaviour->action);
	printf("Entre??????????????");
	ConstructorCyclicBehaviour(readingBehaviour);
	*/
	//printf("Entre a read behaviour\n");
	readingBehaviour.msg = &msg_reading;
	readingBehaviour.action = &readingaction;
	readingBehaviour.execute(&readingBehaviour,&pvParameters);
};


//Main
int sender_receiver() {

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
	printf("MAES DEMO \n");
	AP.agent_init(&AP,&sender, &write);
	printf("Mensajero Listo \n");
	//printf("\nDireccion de writingsetup: %p\n", &writingsetup);
	//printf("\nDireccion asignada dentro del writingBehaviour de writingsetup: %p\n", &writingBehaviour.setup);
	AP.agent_init(&AP,&receiver, &read);
	printf("Receptor Listo \n");
	AP.boot(&AP);
	printf("Boot exitoso \n");

	// Start the scheduler so the created tasks start executing.
	vTaskStartScheduler();

	for (;;);

	return 0;


};
