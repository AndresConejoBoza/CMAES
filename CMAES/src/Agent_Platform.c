#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


void Agent_PlatformFunction(Agent_Platform* platform, const char* name) {
	platform->agentAMS.agent.agent_name = name;
	platform->description.AP_name = name;
	platform->description.subscribers = 0;
	platform->ptr_cond = &platform->cond;

	platform->agentAMS.agent.priority = configMAX_PRIORITIES - 1;
	for (MAESUBaseType_t i = 0; i < AGENT_LIST_SIZE; i++)
	{
		platform->Agent_Handle[i] = (Agent_AID)NULL;
	}
};

void Agent_PlatformWithCondFunction(Agent_Platform* platform, const char* name, USER_DEF_COND* user_cond) {
	platform->agentAMS.agent.agent_name = name;
	platform->ptr_cond = user_cond;
	platform->description.subscribers = 0;
	for (MAESUBaseType_t i = 0; i < AGENT_LIST_SIZE; i++)
	{
		platform->Agent_Handle[i] = (Agent_AID)NULL;
	}
};

static AMSparameter parameters;

bool bootFunction(Agent_Platform* platform) {
	if (xTaskGetCurrentTaskHandle() == NULL)
	{
		AMSparameter* parametersForTask = &parameters;

		// Mailbox = Queue

		platform->agentAMS.agent.mailbox_handle = xQueueCreate(1, sizeof(MsgObj));

		// Task
		parametersForTask->services = platform; //Esto era = this. Según entendí, esto hace referencia a esta misma instancia. 
		parametersForTask->cond = platform->ptr_cond;
		platform->agentAMS.resources.stackSize = configMINIMAL_STACK_SIZE; /*--------------------------------------------------------*/

		xTaskCreate(parametersForTask->AMS_task, platform->agentAMS.agent.agent_name, platform->agentAMS.resources.stackSize, (void*)parametersForTask, (configMAX_PRIORITIES - 1), &platform->agentAMS.agent.aid);

		
		platform->description.AMS_AID = platform->agentAMS.agent.aid;
		platform->ptr_env->set_TaskEnv(platform->ptr_env,platform->agentAMS.agent.aid, &platform->agentAMS);

		if (platform->agentAMS.agent.aid != NULL)
		{
			sysVar* element;
			UBaseType_t i = 0;

			while (i < AGENT_LIST_SIZE) {

				element = platform->ptr_env->getEnv(platform->ptr_env);

				if ((element[i].first == NULL) || (platform->agentAMS.agent.aid == NULL))
				{
					break;
				}
				platform->register_agent(platform,element[i].first);
				i++;
			}
			return NO_ERRORS;
		}
		else
		{
			/* System_abort */
			return INVALID;
		}
	}
	else
	{
		return INVALID;
	}
};

void agent_initFunction(Agent_Platform* platform, MAESAgent* agent, void behaviour(void* pvParameters)) {
	if (xTaskGetCurrentTaskHandle() == 0)
	{
		// Mailbox
		agent->agent.mailbox_handle = xQueueCreate(1, sizeof(MsgObj));

		// Task
		agent->resources.function = behaviour;
		agent->resources.taskParameters = NULL;


		xTaskCreate(behaviour, agent->agent.agent_name, agent->resources.stackSize, agent->resources.taskParameters, 0, &agent->agent.aid);

		
		platform->ptr_env->set_TaskEnv(platform->ptr_env,agent->agent.aid, agent);
		vTaskSuspend(agent->agent.aid);
	}
};

void agent_initConParamFunction(Agent_Platform* platform, MAESAgent* agent, void behaviour(void* pvParameters), void* pvParameters) {
	if (xTaskGetCurrentTaskHandle() == 0)
	{
		// Mailbox
		agent->agent.mailbox_handle = xQueueCreate(1, sizeof(MsgObj));

		// Task
		agent->resources.function = behaviour;
		agent->resources.taskParameters = pvParameters;


		xTaskCreate(behaviour, agent->agent.agent_name, agent->resources.stackSize, agent->resources.taskParameters, 0, &agent->agent.aid);

		
		platform->ptr_env->set_TaskEnv(platform->ptr_env,agent->agent.aid, agent);
		vTaskSuspend(agent->agent.aid);
	}
};

bool agent_searchFunction(Agent_Platform* platform, Agent_AID aid) {
	for (MAESUBaseType_t i = 0; i < platform->description.subscribers; i++)
	{
		if (platform->Agent_Handle[i] == aid)
		{
			return true;
		}
	}
	return false;
};

void agent_waitFunction(Agent_Platform* platform, MAESTickType_t ticks) {
	vTaskDelay(ticks);
};

void agent_yieldFunction(Agent_Platform* platform) {
	taskYIELD();
};

Agent_AID* get_running_agentFunction(Agent_Platform* platform) {
	return xTaskGetCurrentTaskHandle();
};

#ifdef tskKERNEL_VERSION_MAJOR
AGENT_MODE get_stateFunction(Agent_Platform* platform, Agent_AID aid) {
	if (platform->agent_search(platform,aid))
	{
		eTaskState state;
		state = eTaskGetState(aid);
		switch (state)
		{
		case eReady:
			return ACTIVE;

		case eBlocked:
			return WAITING;

		case eSuspended:
			return SUSPENDED;

		case eDeleted:
			return TERMINATED;

		default:
			return NO_MODE;
		}
	}
	else
	{
		return NO_MODE;
	}
};
#endif

Agent_info get_Agent_descriptionFunction(Agent_Platform* platform, Agent_AID aid) {
	MAESAgent* a;
	a = platform->ptr_env->get_taskEnv(platform->ptr_env, aid);

	return a->agent;
};

AP_Description get_AP_descriptionFunction(Agent_Platform* platform) {
	return platform->description;
};

ERROR_CODE register_agentFunction(Agent_Platform* platform, Agent_AID aid) {
	if (aid == NULL)
	{
		return HANDLE_NULL;
	}
	else if (xTaskGetCurrentTaskHandle() == NULL || uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
	{
		if (!platform->agent_search(platform,aid))
		{
			if (platform->description.subscribers < AGENT_LIST_SIZE)
			{
				MAESAgent* a;
				a = platform->ptr_env->get_taskEnv(platform->ptr_env, aid);
				a->agent.AP = platform->agentAMS.agent.aid;
				platform->Agent_Handle[platform->description.subscribers] = aid;
				platform->description.subscribers++;
				vTaskPrioritySet(aid, a->agent.priority);
				vTaskResume(aid);
				return NO_ERRORS;
			}
			else
			{
				return LIST_FULL;
			}
		}
		else
		{
			return DUPLICATED;
		}
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE deregister_agentFunction(Agent_Platform* platform, Agent_AID aid) {
	if (uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
	{
		MAESUBaseType_t i = 0;
		while (i < AGENT_LIST_SIZE)
		{
			if (platform->Agent_Handle[i] == aid)
			{
				MAESAgent* a;
				platform->suspend_agent(platform,aid);
				a = (MAESAgent*)platform->ptr_env->get_taskEnv(platform->ptr_env, aid);
				a->agent.AP = NULL;

				while (i < AGENT_LIST_SIZE - 1)
				{
					platform->Agent_Handle[i] = platform->Agent_Handle[i + 1];
					i++;
				}
				platform->Agent_Handle[AGENT_LIST_SIZE - 1] = NULL;
				platform->description.subscribers--;
				break;
			}
			i++;
		}
		if (i == AGENT_LIST_SIZE)
		{
			return NOT_FOUND;
		}
	}
	//else
	//{
		//return NO_ERRORS;
	//}
	return NO_ERRORS;
};

ERROR_CODE kill_agentFunction(Agent_Platform* platform, Agent_AID aid) {
	if (uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
	{
		ERROR_CODE error;
		error = platform->deregister_agent(platform,aid);

		if (error == NO_ERRORS)
		{
			MAESAgent* a;
			Mailbox_Handle m;

			a = (MAESAgent*)platform->ptr_env->get_taskEnv(platform->ptr_env, aid);;
			a->agent.aid = NULL;
			m = a->agent.mailbox_handle; //A esto hay que agregarle algo, pero no me tira error por el momento
			vQueueDelete(m);
			vTaskDelete(aid);
			
			platform->ptr_env->erase_TaskEnv(platform->ptr_env,aid);
			platform->description.subscribers--;
		}
		return error;
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE suspend_agentFunction(Agent_Platform* platform, Agent_AID aid) {
	if (uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
	{
		if (platform->agent_search(platform,aid))
		{
			vTaskSuspend(aid);
			return (NO_ERRORS); /* code */
		}
		else
		{
			return NOT_FOUND;
		}
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE resume_agentFunction(Agent_Platform* platform, Agent_AID aid) {
	if (uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
	{
		if (platform->agent_search(platform,aid))
		{
			MAESAgent* a;
			a = (MAESAgent*)platform->ptr_env->get_taskEnv(platform->ptr_env, aid);
			vTaskResume(aid);
			vTaskPrioritySet(aid, a->agent.priority);
			return NO_ERRORS;
		}
		else
		{
			return NOT_FOUND;
		}
	}
	else
	{
		return INVALID;
	}
};

void restartFunction(Agent_Platform* platform, Agent_AID aid) {
	if (uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
	{
		MAESAgent* a;
		a = (MAESAgent*)platform->ptr_env->get_taskEnv(platform->ptr_env, aid);
		Mailbox_Handle m;

		// delete Task and Mailbox

		m = a->agent.mailbox_handle; //Esto tiene algo raro, debería de pedir algo, no?
		vTaskDelete(aid);
		vQueueDelete(m);
		 //Tema del env;
		platform->ptr_env->erase_TaskEnv(platform->ptr_env,aid);

		// Mailbox = Queue

		a->agent.mailbox_handle = xQueueCreate(1, sizeof(MSG_TYPE));

		// Task


		xTaskCreate(a->resources.function, a->agent.agent_name, a->resources.stackSize, a->resources.taskParameters, 0, &a->agent.aid);
		platform->ptr_env->set_TaskEnv(platform->ptr_env,a->agent.aid, a);
	}
};



void ConstructorAgent_Platform(Agent_Platform* platform, sysVars* env) {
	platform->ptr_env = env;
	platform->Agent_Platform = &Agent_PlatformFunction;
	platform->Agent_PlatformWithCond = &Agent_PlatformWithCondFunction;
	platform->boot = &bootFunction;
	platform->agent_init = &agent_initFunction;
	platform->agent_initConParam = &agent_initConParamFunction;
	platform->agent_search = &agent_searchFunction;
	platform->agent_wait = &agent_waitFunction;
	platform->agent_yield = &agent_yieldFunction;
	platform->get_running_agent = &get_running_agentFunction;
	#ifdef tskKERNEL_VERSION_MAJOR
	platform->get_state = &get_stateFunction;
	#endif
	platform->get_Agent_description = &get_Agent_descriptionFunction;
	platform->get_AP_description = &get_AP_descriptionFunction;
	platform->register_agent = &register_agentFunction;
	platform->deregister_agent = &deregister_agentFunction;
	platform->kill_agent = &kill_agentFunction;
	platform->suspend_agent = &suspend_agentFunction;
	platform->resume_agent = &resume_agentFunction;
	platform->restart = &restartFunction;
};