#include <CMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


//Is Registered Function: This function checks if an Agent is registered in the environment.
//Inputs: The Message instance itself and the AID.
//Outputs: True or False, depending on wether the agent was found or not.
bool isRegisteredFunction(Agent_Msg* Message, Agent_AID aid) { //Este recibía un resultado booleano, revisar eso.// No deja return true, hay que cambiar la lógica a 1 y Function.



	//MAESAgent* description_receiver = calloc(1, sizeof(MAESAgent));
	//MAESAgent* description_sender = calloc(1, sizeof(MAESAgent));
	//Inicializador(description_receiver);
	//Inicializador(description_sender);
	MAESAgent* description_receiver = (MAESAgent*)env.get_taskEnv(&env,aid);
	MAESAgent* description_sender = (MAESAgent*)env.get_taskEnv(&env,Message->caller);


	//Agent* description_receiver = (Agent*)ptr_env->get_TaskEnv(aid); No estoy seguro de si transcribí bien estas. Se transcribió a lineas 12-17
//	Agent* description_sender = (Agent*)ptr_env->get_TaskEnv(caller);

	if (description_receiver->agent.AP == description_sender->agent.AP)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
};

//Get Mailbox Function: This function returns a pointer to the mailbox handle of an Agent.
//Inputs: The message instance itself and the AID.
//Outputs: Pointer to the mailbox handle of the Agent.
Mailbox_Handle* get_mailboxFunction(Agent_Msg* Message, Agent_AID aid) { //En esta igual, no sé si tengo que inicializar el agente y env o sí sólo llamarlo así basta. 

	MAESAgent* description = (MAESAgent*)env.get_taskEnv(&env, aid);
	if (description == NULL) {
		printf("Retorne nulo en getmailbox\n");
		return NULL;
	}
	return description->agent.mailbox_handle;
};

//Agent MSG Function: This function sets the default values of some of the pointers used. This function is normally used when you need to set the caller.
//Inputs: The message instance itself.
//Outputs: None.
void Agent_MsgFunction(Agent_Msg* Message) {
	Message->caller = xTaskGetCurrentTaskHandle();
	Message->clear_all_receiver(Message);
};

ERROR_CODE add_receiverFunction(Agent_Msg* Message, Agent_AID aid_receiver) {
	if (Message->isRegistered(Message,aid_receiver))
	{
		if (aid_receiver == NULL)
		{
			return HANDLE_NULL;
		}
		if (Message->subscribers < MAX_RECEIVERS)
		{
			Message->receivers[Message->subscribers] = aid_receiver;
			Message->subscribers= Message->subscribers+1;
			return NO_ERRORS;
		}
		else
		{
			return LIST_FULL;
		}
	}
	else
	{
		return NOT_FOUND;
	}
};


ERROR_CODE remove_receiverFunction(Agent_Msg* Message, Agent_AID aid) {
	MAESUBaseType_t i = 0;
	while (i < MAX_RECEIVERS)
	{
		if (Message->receivers[i] == aid)
		{
			while (i < MAX_RECEIVERS - 1)
			{
				Message->receivers[i] = Message->receivers[i + 1];
				i++;
			}
			Message->receivers[MAX_RECEIVERS - 1] = NULL;
			Message->subscribers--;
			break;
		}
		i++;
	}
	if (i == MAX_RECEIVERS)
	{
		return NOT_FOUND;
	}
	else
	{
		return NO_ERRORS;
	}
};

void clear_all_receiverFunction(Agent_Msg* Message) {
	MAESUBaseType_t i = 0;
	while (i < MAX_RECEIVERS)
	{
		Message->receivers[i] = NULL;
		i++;
	}
};

void refresh_listFunction(Agent_Msg* Message) {
	MAESAgent* agent_caller, *agent_receiver;
	agent_caller = (MAESAgent*)env.get_taskEnv(&env,Message->caller);
	for (UBaseType_t i = 0; i < Message->subscribers; i++)
	{
		agent_receiver = (MAESAgent*)env.get_taskEnv(&env,Message->receivers[i]);
		if (Message->isRegistered(Message,Message->receivers[i]) || agent_caller->agent.org != agent_receiver->agent.org)
		{
			Message->remove_receiver(Message,Message->receivers[i]);
		}
	}
};

MSG_TYPE receiveFunction(Agent_Msg* Message, MAESTickType_t timeout) {
	if (xQueueReceive(Message->get_mailbox(&Message, Message->caller), &Message->msg, timeout) != pdPASS)
	{
		return NO_RESPONSE;
	}
	else
	{
		return Message->msg.type;
	}
};

ERROR_CODE send1Function(Agent_Msg* Message, Agent_AID aid_receiver, TickType_t timeout) {
	Message->msg.target_agent = aid_receiver; 
	Message->msg.sender_agent = Message->caller;
	MAESAgent* agent_caller, * agent_receiver;
	agent_caller = (MAESAgent*)env.get_taskEnv(&env,Message->caller);
	agent_receiver = (MAESAgent*)env.get_taskEnv(&env,aid_receiver);
	if (Message->isRegistered(Message,aid_receiver)==0)
	{
		return NOT_REGISTERED;
	}
	else
	{
		if (agent_caller->agent.org == NULL && agent_receiver->agent.org == NULL)
		{
			if (xQueueSend(Message->get_mailbox(&Message,aid_receiver), &Message->msg, timeout) != pdPASS) 
			{
				return TIMEOUT;
			}
			else
			{
				return NO_ERRORS;
			}
		}
		else if (agent_caller->agent.org == agent_receiver->agent.org)
		{
			if (((agent_caller->agent.org->org_type == TEAM) && (agent_caller->agent.role == PARTICIPANT)) || ((agent_caller->agent.org->org_type == HIERARCHY) && (agent_receiver->agent.role == MODERATOR)))
			{
				if (xQueueSend(Message->get_mailbox(&Message,aid_receiver), &Message->msg, timeout) != pdPASS)
				{
					return TIMEOUT;
				}
				else
				{
					return NO_ERRORS;
				}
			}
			else
			{
				return INVALID;
			}
		}
		else if ((agent_caller->agent.affiliation == ADMIN) || (agent_caller->agent.affiliation == OWNER))
		{
			if (xQueueSend(Message->get_mailbox(Message,aid_receiver), &Message->msg, timeout) != pdPASS)
			{
				return TIMEOUT;
			}
			else
			{
				return NO_ERRORS;
			}
		}
		else
		{
			return NOT_REGISTERED;
		}
	}
};
ERROR_CODE send0Function(Agent_Msg* Message) {
	MAESUBaseType_t i = 0;
	ERROR_CODE error_code;
	ERROR_CODE error = NO_ERRORS;

	while (Message->receivers[i] != NULL)
	{
		error_code = Message->send(Message, Message->receivers[i], portMAX_DELAY);
		if (error_code != NO_ERRORS)
		{
			error = error_code;
		}
		i++;
	}
	return error;
};

void set_msg_typeFunction(Agent_Msg* Message, MSG_TYPE type) {
	Message->msg.type = type;
};

void set_msg_contentFunction(Agent_Msg* Message, char* msg_content) {
	Message->msg.content = msg_content;
};

MsgObj* get_msgFunction(Agent_Msg* Message) {
	MsgObj* ptr = &Message->msg;
	return ptr;
};

MSG_TYPE get_msg_typeFunction(Agent_Msg* Message) {
	return Message->msg.type;
}

char* get_msg_contentFunction(Agent_Msg* Message) {
	return Message->msg.content;
};

Agent_AID get_senderFunction(Agent_Msg* Message) {
	return Message->msg.sender_agent;
};

Agent_AID get_target_agentFunction(Agent_Msg* Message) {
	return Message->msg.target_agent;
};

ERROR_CODE registrationFunction(Agent_Msg* Message, Agent_AID target_agent) {
	Agent_AID AMS;
	MAESAgent* agent_caller;
	MAESAgent* agent_target;
	agent_caller = (MAESAgent*)env.get_taskEnv(&env,Message->caller);
	agent_target = (MAESAgent*)env.get_taskEnv(&env,target_agent);
	if (target_agent == NULL)
	{
		return HANDLE_NULL;
	}
	else if ((agent_caller->agent.org == NULL) || (agent_caller->agent.org != NULL && ((agent_caller->agent.affiliation == OWNER) || (agent_caller->agent.affiliation == ADMIN))))
	{
		if (agent_caller->agent.org == agent_target->agent.org)
		{
			Message->msg.type = REQUEST;
			Message->msg.content = (char*)"REGISTER";
			Message->msg.target_agent = target_agent;
			Message->msg.sender_agent = xTaskGetCurrentTaskHandle();
			//Get the AMS info
			AMS = agent_caller->agent.AP;
			//Sending request
			if (xQueueSend(Message->get_mailbox(Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
			{
				return INVALID;
			}
			else
			{
				return NO_ERRORS;
			}
		}
		else
		{
			return INVALID;
		}
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE deregistrationFunction(Agent_Msg* Message, Agent_AID target_agent){

	Agent_AID AMS;
	MAESAgent* agent_caller;
	MAESAgent* agent_target;
	agent_caller = (MAESAgent*)env.get_taskEnv(&env,Message->caller);
	agent_target = (MAESAgent*)env.get_taskEnv(&env,target_agent);
	if (target_agent == NULL)
	{
		return HANDLE_NULL;
	}
	else if (agent_caller->agent.org == NULL || (agent_caller->agent.org != NULL && (agent_caller->agent.affiliation == OWNER || agent_caller->agent.affiliation == ADMIN)))
	{
		if (agent_caller->agent.org == agent_target->agent.org)
		{
			Message->msg.type = REQUEST;
			Message->msg.content = (char*)"DEREGISTER";
			Message->msg.target_agent = target_agent;
			Message->msg.sender_agent = xTaskGetCurrentTaskHandle();
			//Get the AMS info
			AMS = agent_caller->agent.AP;
			//Sending request
			if (xQueueSend(Message->get_mailbox(&Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
			{
				return INVALID;
			}
			else
			{
				return NO_ERRORS;
			}
		}
		else
		{
			return INVALID;
		}
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE suspendFunction(Agent_Msg* Message, Agent_AID target_agent) {
	Agent_AID AMS;
	MAESAgent* agent_caller;
	MAESAgent* agent_target;
	agent_caller = (MAESAgent*)env.get_taskEnv(&env, Message->caller);
	agent_target = (MAESAgent*)env.get_taskEnv(&env,target_agent);

	if (target_agent == NULL)
	{
		return HANDLE_NULL;
	}
	else if (agent_caller->agent.org == NULL || (agent_caller->agent.org != NULL && (agent_caller->agent.affiliation == OWNER || agent_caller->agent.affiliation == ADMIN)))
	{
		if (agent_caller->agent.org == agent_target->agent.org)
		{
			Message->msg.type = REQUEST;
			Message->msg.content = (char*)"SUSPEND";
			Message->msg.target_agent = target_agent;
			Message->msg.sender_agent = xTaskGetCurrentTaskHandle();
			//Get the AMS info
			AMS = agent_caller->agent.AP;
			//Sending request
			if (xQueueSend(Message->get_mailbox(&Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
			{
				return INVALID;
			}
			else
			{
				return NO_ERRORS;
			}
		}
		else
		{
			return INVALID;
		}
	}
	else
	{
		return INVALID;
	}
};
ERROR_CODE resumeFunction(Agent_Msg* Message, Agent_AID target_agent) {
	Agent_AID AMS;
	MAESAgent* agent_caller;
	MAESAgent* agent_target;
	agent_caller = (MAESAgent*)env.get_taskEnv(&env,Message->caller);
	agent_target = (MAESAgent*)env.get_taskEnv(&env,target_agent);
	if (target_agent == NULL)
	{
		return HANDLE_NULL;
	}
	else if (agent_caller->agent.org == NULL || (agent_caller->agent.org != NULL && (agent_caller->agent.affiliation == OWNER || agent_caller->agent.affiliation == ADMIN)))
	{
		if (agent_caller->agent.org == agent_target->agent.org)
		{
			Message->msg.type = REQUEST;
			Message->msg.content = (char*)"RESUME";
			Message->msg.target_agent = target_agent;
			Message->msg.sender_agent = xTaskGetCurrentTaskHandle();
			//Get the AMS info
			AMS = agent_caller->agent.AP;
			//Sending request
			if (xQueueSend(Message->get_mailbox(&Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
			{
				return INVALID;
			}
			else
			{
				return NO_ERRORS;
			}
		}
		else
		{
			return INVALID;
		}
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE killFunction(Agent_Msg* Message, Agent_AID target_agent) {
	Agent_AID AMS;
	MAESAgent* agent_caller;
	MAESAgent* agent_target;
	agent_caller = (MAESAgent*)env.get_taskEnv(&env,Message->caller);
	agent_target = (MAESAgent*)env.get_taskEnv(&env,target_agent);
	if (target_agent == NULL)
	{
		return HANDLE_NULL;
	}
	else if (agent_caller->agent.org == NULL || (agent_caller->agent.org != NULL && (agent_caller->agent.affiliation == OWNER || agent_caller->agent.affiliation == ADMIN)))
	{
		if (agent_caller->agent.org == agent_target->agent.org)
		{
			Message->msg.type = REQUEST;
			Message->msg.content = (char*)"KILL";
			Message->msg.target_agent = target_agent;
			Message->msg.sender_agent = xTaskGetCurrentTaskHandle();
			//Get the AMS info
			AMS = agent_caller->agent.AP;
			//Sending request
			if (xQueueSend(Message->get_mailbox(&Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
			{
				return INVALID;
			}
			else
			{
				return NO_ERRORS;
			}
		}
		else
		{
			return INVALID;
		}
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE restartMsgFunction(Agent_Msg* Message) {
	Agent_AID AMS;
	MAESAgent* agent_caller;
	agent_caller = (MAESAgent*)env.get_taskEnv(&env,Message->caller);
	Message->msg.type = REQUEST;
	Message->msg.content = (char*)"RESTART";
	Message->msg.target_agent = xTaskGetCurrentTaskHandle();
	Message->msg.sender_agent = xTaskGetCurrentTaskHandle();
	AMS = agent_caller->agent.AP;
	if (xQueueSend(Message->get_mailbox(&Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
	{
		return INVALID;
	}
	else
	{
		return NO_ERRORS;
	}
};

void ConstructorAgent_Msg(Agent_Msg* Message,sysVars* env) {
	Message->subscribers = 0;
	Message->ptr_env = env;
	Message->isRegistered = &isRegisteredFunction;
	Message->get_mailbox = &get_mailboxFunction;
	Message->Agent_Msg = &Agent_MsgFunction;
	Message->add_receiver = &add_receiverFunction;
	Message->remove_receiver = &remove_receiverFunction;
	Message->clear_all_receiver = &clear_all_receiverFunction;
	Message->refresh_list = &refresh_listFunction;
	Message->receive = &receiveFunction;
	Message->send = &send1Function;
	Message->send0= &send0Function;
	Message->set_msg_type = &set_msg_typeFunction;
	Message->set_msg_content = &set_msg_contentFunction;
	Message->get_msg = &get_msgFunction;
	Message->get_msg_type = &get_msg_typeFunction;
	Message->get_msg_content = &get_msg_contentFunction;
	Message->get_sender = &get_senderFunction;
	Message->get_target_agent = &get_target_agentFunction;
	Message->registration = &registrationFunction;
	Message->deregistration = &deregistrationFunction;
	Message->suspend = &suspendFunction;
	Message->resume = &resumeFunction;
	Message->kill = &killFunction;
	Message->restart = &restartMsgFunction;
}
