#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>





MAESAgent* get_taskEnvFunction(sysVars* Vars, Agent_AID aid) {
	//printf("Entre a get task env y recibi este aid: %s\n", aid);;
	MAESUBaseType_t i = 0;
	while (i < AGENT_LIST_SIZE)
	{
		if (Vars->environment[i].first == aid && aid!=NULL)
		{
			//printf("Retorne algo diferente de nulo en gettaskenv con la direccion: %p\n", Vars->environment[i].second);
			return Vars->environment[i].second;
		}
		i++;
	}
	//printf("Retorne nulo en gettaskenv\n");
	return NULL;
	//return environment[aid];
};

void set_TaskEnvFunction(sysVars* Vars, Agent_AID aid, MAESAgent* agent_ptr) {
	MAESUBaseType_t i = 0;
	while (i < AGENT_LIST_SIZE)
	{
		if (Vars->environment[i].first == NULL)
		{
			Vars->environment[i].first = aid;
			Vars->environment[i].second = agent_ptr;
			break;
		}
		i++;
	}
	//environment.insert(make_pair(aid, agent_ptr));
};


void erase_TaskEnvFunction(sysVars* Vars, Agent_AID aid) {
	MAESUBaseType_t i = 0;
	while (i < AGENT_LIST_SIZE)
	{
		if (Vars->environment[i].first == aid)
		{
			Vars->environment[i].first = NULL;
			Vars->environment[i].second = NULL;

			while (i < AGENT_LIST_SIZE - 1)
			{
				Vars->environment[i] = Vars->environment[i + 1];
				i++;
			}
			break;
		}
		i++;
	}
	//environment.erase(aid);
};


sysVar* getEnvFunction(sysVars* Vars) {
	return Vars->environment;  //Revisar este porque normalmente devuelve el enviroment, en este caso está devolviendo el puntero, no sé si afectará.
	//No tira errores si le quito el & del puntero.
};




void ConstructorSysVars(sysVars* Vars) {
	Vars->get_taskEnv = &get_taskEnvFunction;
	Vars->set_TaskEnv = &set_TaskEnvFunction;
	Vars->erase_TaskEnv = &erase_TaskEnvFunction;
	Vars->getEnv = &getEnvFunction;
}