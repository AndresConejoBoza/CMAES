#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>




//Get Task Environment: This function locates an Agent in the environment using its AID. Then, it returns a pointer to the Agent. 
//Inputs: The environment used and the AID.
//Outputs: Pointer to the Agent or NULL in case that the Agent was not found.
MAESAgent* get_taskEnvFunction(sysVars* Vars, Agent_AID aid) {
	//printf("Entre a get task env y recibi este aid: %s\n", aid);;
	MAESUBaseType_t i = 0;
	while (i < AGENT_LIST_SIZE)
	{
		if (Vars->environment[i].first == aid && aid!=NULL)
		{
			//printf("Nombre de la entrada %i revisada en el ambiente en la func gettaskenv: %s\n",i, Vars->environment[i].second->agent.agent_name);
			//printf("Retorne algo diferente de nulo en gettaskenv con la direccion: %p\n", Vars->environment[i].second);
			return Vars->environment[i].second;
		}
		i++;
	}
	//printf("Retorne nulo en gettaskenv\n");
	return NULL;
	//return environment[aid];
};


//Set Task Environment Function: This function registers an Agent in the environment.
//Inputs: The environment, the AID and a pointer to the Agent.
//Outputs: None.
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

//Erase Task Environment Function: This function erases an Agent in the environment.
//Inputs: The environment and the AID.
//Outputs: None.
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

//Get Environment Function: This function returns the struct in which the environment is held.
//Inputs: The environment.
//Outputs: a sysVar struct that contains the environment.
sysVar* getEnvFunction(sysVars* Vars) {
	return Vars->environment;  //Revisar este porque normalmente devuelve el environment, en este caso est� devolviendo el puntero, no s� si afectar�.
	//No tira errores si le quito el & del puntero.
};



//SysVars Constructor: This function assigns the class pointers to its corresponding function.
//Inputs: Ponter to the SysVars class.
//Outputs: None.
void ConstructorSysVars(sysVars* Vars) {
	Vars->get_taskEnv = &get_taskEnvFunction;
	Vars->set_TaskEnv = &set_TaskEnvFunction;
	Vars->erase_TaskEnv = &erase_TaskEnvFunction;
	Vars->getEnv = &getEnvFunction;
}