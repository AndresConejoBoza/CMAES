/*Ejemplo de demostración MAES número 2: Competencia de Papel Piedra o Tijera*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeMAES.h"

/* Demo includes. */
#include "supporting_functions.h"

/*
void iniciadores(MAESAgent* agenteainiciar) {
	agenteainiciar->IniciadorVacio = MAESAgent0();
	agenteainiciar->Iniciador = MAESAgent1();
	//agenteainiciar.AID = AID0();
}
*/

/*
MAESAgent* CreateAgent(const char* name, MAESUBaseType_t pri, uint16_t sizeStack) {
	MAESAgent* New = calloc(1, sizeof(MAESAgent));
	New->IniciadorVacio=MAESAgent0();
	New->Iniciador =MAESAgent1();
	New->Iniciador=(New, name, pri, sizeStack);
	return New;
};
*/


int main() {

	
	//Inician pruebas de la clase agente//

	printf("HOLA empezo el main\n\n");
	//MAESAgent* agente1 = calloc(1, sizeof(MAESAgent));
	//MAESAgent* agente2 = calloc(1, sizeof(MAESAgent));
	//sysVars* env = calloc(1, sizeof(sysVars));
	//Agent_Msg* Mensaje = calloc(1, sizeof(Agent_Msg));

	MAESAgent Agent;
	MAESAgent agente1, agente2;
	sysVars env;
	Agent_Msg Mensaje;


	ConstructorAgente(&Agent);
	ConstructorAgente(&agente1);
	ConstructorAgente(&agente2);
	InicializadorSysVars(&env);
	InicializadorAgent_Msg(&Mensaje,&env);

	Agent.Iniciador(&Agent, "Agent", 4, 1000);
	agente1.Iniciador(&agente1, "PrimerAgente", 2, 1000);
	agente2.Iniciador(&agente2, "SegundoAgente", 3, 1500);
	//Agent_AID* AIDobtenido= agente1->AID(agente1);

	//Este es el que está sirviendo por el momento
	//MAESAgent1(agente1, "PrimerAgente", 2, 1000);
	//MAESAgent1(agente2, "SegundoAgente", 2, 1000);


	
	//MAESAgent* agente1 = CreateAgent("Primer Agente", 2, 1000);
	//MAESAgent* agente2 = CreateAgent("Segundo Agente", 3, 1000);

	//printf("HOLA MUNDOOOOOO");
	
	int i = 0;
	if (agente1.agent.role == VISITOR) {
		i = 1;
	};
	printf("\nRol de agente == visitor: %d\n", i);
	printf(agente1.agent.agent_name);
	printf("\nNombre del Agent: %s\n", Agent.agent.agent_name);
	printf("\nAddress del Agent: %p\n", &Agent);
	printf("\nAddress del segundo agente: %p\n", &agente1);
	printf("\nAddress del segundo agente: %p\n", &agente2);
	//iniciadores((MAESAgent*)agente1);
	//agente1.Iniciador0 = MAESAgent0();
	//agente1.Iniciador = MAESAgent1();
	//agente1.AID = AID0();
	//agente1.Iniciador(&agente1, "PrimerAgente", 2, 1000);
	//printf("\nPara este punto ya se ejecutó el iniador del Primer agente, debe de haberse generado un print de ello\n");

	//Termina Pruebas de la clase agente//



	//Inician Pruebas de la clase sysVars//



	vTaskStartScheduler();
	for (;;);
	return 0;
};