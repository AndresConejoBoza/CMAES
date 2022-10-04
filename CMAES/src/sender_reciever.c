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

void writingsetup(CyclicBehaviour* behaviour) {
	msg_writing.add_receiver(&msg_writing,receiver.AID(&receiver));
};

void writingaction(CyclicBehaviour* behaviour) {
	printf("Enviando mensaje... \n");
	msg_writing.send0(&msg_writing);
	vTaskDelay(pdMS_TO_TICKS(1000));
};

void write(CyclicBehaviour* Behaviour) {
	printf("\nEntreeeeee al write\n");
	Behaviour->setup = &writingsetup;
	Behaviour->action = &writingaction;
	Behaviour->execute(Behaviour);
};


//Funciones referentes al reading Behaviour


void readingaction(CyclicBehaviour* behaviour) {
	printf("Esperando... \n");
	msg_reading.receive(&msg_reading,portMAX_DELAY);
	printf("Mensaje recibido: ¡Hola MAES! \n");
};

void read(CyclicBehaviour* Behaviour) {
	printf("Entre??????????????");
	Behaviour->action = &readingaction;
	Behaviour->execute(Behaviour);
};



int main() {
	ConstructorAgente(&sender);
	ConstructorAgente(&receiver);
	InicializadorSysVars(&env);
	ConstructorAgent_Platform(&AP, &env);
	InicializadorAgent_Msg(&msg_writing, &env);
	InicializadorAgent_Msg(&msg_reading, &env);
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
