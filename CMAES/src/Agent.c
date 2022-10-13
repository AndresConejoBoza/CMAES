#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>


//AID Function: It returns the Agent ID of the Agent given.
//Inputs: Pointer to an Agent
//Output: Pointer to the Agent's ID.
Agent_AID* AID0(MAESAgent* MAESagent) {
	//printf("Se devuelve el aid del agente exitosamente:");
	return MAESagent->agent.aid;
};

//Agent Inicialization: The function sets the default value for each parameter.
//Inputs: Pointer to an Agent, String that contains the name you want to give to the Agent, priority and the size of the stack
//Outputs: None
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

//Agent Constructor: This function assigns the class pointers to its corresponding function.
//Inputs: Ponter to the Agent.
//Outputs: None.
void ConstructorAgente(MAESAgent* agente) {
	agente->Iniciador = &MAESAgent1;
	//agente->IniciadorVacio = &MAESAgent0;
	agente->AID = &AID0;
}
