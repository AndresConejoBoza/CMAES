#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>




//MAESAgent* MAESAgent0(MAESAgent* MAESagent) {//Probablemente se vaya a borrar
//	return MAESagent;
//};


Agent_AID* AID0(MAESAgent* MAESagent) {
	//printf("Se devuelve el aid del agente exitosamente:");
	return MAESagent->agent.aid;
};


void MAESAgent1(MAESAgent* MAESagent, const char* name, MAESUBaseType_t pri, uint16_t sizeStack) {
	//if (MAESagent->agent.priority >= (MAESUBaseType_t)(configMAX_PRIORITIES - 1))
	//	MAESagent->agent.priority = configMAX_PRIORITIES - 2;
	MAESagent->agent.aid = NULL;
	MAESagent->agent.mailbox_handle = NULL;
	MAESagent->agent.agent_name = name;
	MAESagent->agent.priority = pri;
	MAESagent->resources.stackSize = sizeStack;
	MAESagent->agent.AP = NULL;
	MAESagent->agent.org = NULL;
	MAESagent->agent.affiliation = NON_MEMBER;
	MAESagent->agent.role = VISITOR;
	//MAESagent->AID = AID0; //SEGÚN VIDEO
	//printf("Se creo el agente exitosamente: %d\n",sizeStack);
	//printf(MAESagent->agent.agent_name);
};


void ConstructorAgente(MAESAgent* agente) {
	agente->Iniciador = &MAESAgent1;
	//agente->IniciadorVacio = &MAESAgent0;
	agente->AID = &AID0;
}
//