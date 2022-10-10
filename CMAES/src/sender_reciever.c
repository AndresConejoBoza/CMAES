#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeMAES.h"

/* Demo includes. */
#include "supporting_functions.h"

MAESAgent sender, receiver;
Agent_Platform AP;
sysVars env;
CyclicBehaviour writingBehaviour, readingBehaviour;
Agent_Msg msg_writing, msg_reading;







//Funciones referentes al writing Behaviour//

void writingsetup(void* behaviour) {
	msg_writing.Agent_Msg(&msg_writing);
	msg_writing.add_receiver(&msg_writing,receiver.AID(&receiver));
};

void writingaction(CyclicBehaviour* behaviour) {
	printf("Enviando mensaje... \n");
	msg_writing.send0(&msg_writing);
	vTaskDelay(pdMS_TO_TICKS(1000));
};

void write(void* Behaviour) {
	//printf("\nEntreeeeee al write\n");
	writingBehaviour.setup = &writingsetup;
	writingBehaviour.action = &writingaction;
	writingBehaviour.execute(&writingBehaviour);
};


//Funciones referentes al reading Behaviour


void readingaction(CyclicBehaviour* behaviour) {
	//printf("Entre a readingaction\n");
	printf("Esperando... \n");
	msg_reading.Agent_Msg(&msg_reading);
	msg_reading.receive(&msg_reading,portMAX_DELAY);
	printf("Mensaje recibido: ¡Hola MAES! \n");
};

void read(void* Behaviour) {
	/*
	printf("direccion recibida del behaviour: %p\n", &Behaviour);
	CyclicBehaviour* readingBehaviour = &(CyclicBehaviour*)Behaviour;
	printf("Direccion del readingbehaviour: %p\n", &readingBehaviour);
	printf("Direccion del puntero hacia readingbehaviouraction: %p\n", &readingBehaviour->action);
	printf("Entre??????????????");
	ConstructorCyclicBehaviour(readingBehaviour);
	*/
	//printf("Entre a read behaviour\n");
	readingBehaviour.action = &readingaction;
	readingBehaviour.execute(&readingBehaviour);
};



int sender_receiver() {
	ConstructorAgente(&sender);
	ConstructorAgente(&receiver);
	ConstructorSysVars(&env);
	ConstructorAgent_Platform(&AP, &env);
	ConstructorAgent_Msg(&msg_writing, &env);
	ConstructorAgent_Msg(&msg_reading, &env);
	ConstructorCyclicBehaviour(&writingBehaviour);
	ConstructorCyclicBehaviour(&readingBehaviour);
	sender.Iniciador(&sender, "Agent Sender", 1, 1024);
	receiver.Iniciador(&receiver, "Agent Receiver", 3, 1024);
	
	
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
