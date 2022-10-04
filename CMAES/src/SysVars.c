#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>





MAESAgent* get_taskEnv0(sysVars* Vars, Agent_AID aid) {
	MAESUBaseType_t i = 0;
	while (i < AGENT_LIST_SIZE)
	{
		if (Vars->environment[i].first == aid)
		{
			return Vars->environment[i].second;
		}
		i++;
	}
	return NULL;
	//return environment[aid];
};

void set_TaskEnv0(sysVars* Vars, Agent_AID aid, MAESAgent* agent_ptr) {
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


void erase_TaskEnv0(sysVars* Vars, Agent_AID aid) {
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


sysVar* getEnv0(sysVars* Vars) {
	return Vars->environment;  //Revisar este porque normalmente devuelve el enviroment, en este caso está devolviendo el puntero, no sé si afectará.
	//No tira errores si le quito el & del puntero.
};




void InicializadorSysVars(sysVars* Vars) {
	Vars->get_taskEnv = &get_taskEnv0;
	Vars->set_TaskEnv = &set_TaskEnv0;
	Vars->erase_TaskEnv = &erase_TaskEnv0;
	Vars->getEnv = &getEnv0;
}