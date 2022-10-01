#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



bool isRegistered0(Agent_Msg* Message, Agent_AID aid) { //Este recibía un resultado booleano, revisar eso.// No deja return true, hay que cambiar la lógica a 1 y 0.



	//MAESAgent* description_receiver = calloc(1, sizeof(MAESAgent));
	//MAESAgent* description_sender = calloc(1, sizeof(MAESAgent));
	//Inicializador(description_receiver);
	//Inicializador(description_sender);
	MAESAgent* description_receiver = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,aid);
	MAESAgent* description_sender = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,Message->caller);


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
Mailbox_Handle* get_mailbox0(Agent_Msg* Message, Agent_AID aid) { //En esta igual, no sé si tengo que inicializar el agente y env o sí sólo llamarlo así basta. 
	MAESAgent* description;
	description = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env, aid);
	return description->agent.mailbox_handle;
};

void Agent_Msg0(Agent_Msg* Message) {
	Message->caller = xTaskGetCurrentTaskHandle();
	Message->clear_all_receiver(Message);
	Message->subscribers = 0;
};

ERROR_CODE add_receiver0(Agent_Msg* Message, Agent_AID aid_receiver) {
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


ERROR_CODE remove_receiver0(Agent_Msg* Message, Agent_AID aid) {
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

void clear_all_receiver0(Agent_Msg* Message) {
	MAESUBaseType_t i = 0;
	while (i < MAX_RECEIVERS)
	{
		Message->receivers[i] = NULL;
		i++;
	}
};

void refresh_list0(Agent_Msg* Message) {// Lo mismo, no sé si inicializar env y el agente.
	MAESAgent* agent_caller, *agent_receiver;
	agent_caller = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,Message->caller);

	for (UBaseType_t i = 0; i < Message->subscribers; i++)
	{
		agent_receiver = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,Message->receivers[i]);
		if (Message->isRegistered(Message,Message->receivers[i]) || agent_caller->agent.org != agent_receiver->agent.org)
		{
			Message->remove_receiver(Message,Message->receivers[i]);
		}
	}
};

MSG_TYPE receive0(Agent_Msg* Message, MAESTickType_t timeout) {
	//ptr_env = &env;
	//Agent* a = (Agent*)ptr_env->get_TaskEnv(caller);
	if (xQueueReceive(Message->get_mailbox(Message, Message->caller), &Message->msg, timeout) != pdPASS)
	{
		return NO_RESPONSE;
	}
	else
	{
		return Message->msg.type;
	}
};

ERROR_CODE send1(Agent_Msg* Message, Agent_AID aid_receiver, TickType_t timeout) {// Igual, no sé si inicializar env y agentes. No sé qué es msg.etc
	Message->msg.target_agent = aid_receiver; 
	Message->msg.sender_agent = Message->caller;

	MAESAgent* agent_caller, * agent_receiver;
	agent_caller = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,Message->caller);
	agent_receiver = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,aid_receiver);

	
	//agent_caller = (Agent*)ptr_env->get_TaskEnv(caller);
	//agent_receiver = (Agent*)ptr_env->get_TaskEnv(aid_receiver);

	if (Message->isRegistered(Message,aid_receiver)==0)
	{
		return NOT_REGISTERED;
	}
	else
	{
		if (agent_caller->agent.org == NULL && agent_receiver->agent.org == NULL)
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
		else if (agent_caller->agent.org == agent_receiver->agent.org)
		{
			if (((agent_caller->agent.org->org_type == TEAM) && (agent_caller->agent.role == PARTICIPANT)) || ((agent_caller->agent.org->org_type == HIERARCHY) && (agent_receiver->agent.role == MODERATOR)))
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
ERROR_CODE send00(Agent_Msg* Message) {
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

void set_msg_type0(Agent_Msg* Message, MSG_TYPE type) {
	Message->msg.type = type;
};

void set_msg_content0(Agent_Msg* Message, char* msg_content) {
	Message->msg.content = msg_content;
};

MsgObj* get_msg0(Agent_Msg* Message) {
	MsgObj* ptr = &Message->msg;
	return ptr;
};

MSG_TYPE get_msg_type0(Agent_Msg* Message) {
	return Message->msg.type;
}

char* get_msg_content0(Agent_Msg* Message) {
	return Message->msg.content;
};

Agent_AID* get_sender0(Agent_Msg* Message) {
	return Message->msg.sender_agent;
};

Agent_AID* get_target_agent0(Agent_Msg* Message) {
	return Message->msg.target_agent;
};

ERROR_CODE registration0(Agent_Msg* Message, Agent_AID target_agent) { //mismo problema con agentes y env

	Agent_AID AMS;
	MAESAgent* agent_caller;
	MAESAgent* agent_target;
	agent_caller = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,Message->caller);
	agent_target = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,target_agent);

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

ERROR_CODE deregistration0(Agent_Msg* Message, Agent_AID target_agent){

	Agent_AID AMS;
	MAESAgent* agent_caller;
	MAESAgent* agent_target;
	agent_caller = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,Message->caller);
	agent_target = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,target_agent);

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

ERROR_CODE suspend0(Agent_Msg* Message, Agent_AID target_agent) {

	Agent_AID AMS;
	MAESAgent* agent_caller;
	MAESAgent* agent_target;
	agent_caller = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env, Message->caller);
	agent_target = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,target_agent);

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
ERROR_CODE resume0(Agent_Msg* Message, Agent_AID target_agent) {

	Agent_AID AMS;
	MAESAgent* agent_caller;
	MAESAgent* agent_target;
	agent_caller = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,Message->caller);
	agent_target = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,target_agent);

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
ERROR_CODE kill0(Agent_Msg* Message, Agent_AID target_agent) {

	Agent_AID AMS;
	MAESAgent* agent_caller;
	MAESAgent* agent_target;
	agent_caller = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,Message->caller);
	agent_target = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,target_agent);

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
ERROR_CODE restart0(Agent_Msg* Message) {

	Agent_AID AMS;
	MAESAgent* agent_caller;
	agent_caller = (MAESAgent*)Message->ptr_env->get_taskEnv(Message->ptr_env,Message->caller);

	Message->msg.type = REQUEST;
	Message->msg.content = (char*)"RESTART";
	Message->msg.target_agent = xTaskGetCurrentTaskHandle();
	Message->msg.sender_agent = xTaskGetCurrentTaskHandle();

	AMS = agent_caller->agent.AP;

	if (xQueueSend(Message->get_mailbox(Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
	{
		return INVALID;
	}
	else
	{
		return NO_ERRORS;
	}
};


void InicializadorAgent_Msg(Agent_Msg* Message,sysVars* env) {
	Message->ptr_env = env;
	Message->isRegistered = &isRegistered0;
	Message->get_mailbox = &get_mailbox0;
	Message->Agent_Msg = &Agent_Msg0;
	Message->add_receiver = &add_receiver0;
	Message->remove_receiver = &remove_receiver0;
	Message->clear_all_receiver = &clear_all_receiver0;
	Message->refresh_list = &refresh_list0;
	Message->receive = &receive0;
	Message->send = &send1;
	Message->send0= &send00;
	Message->set_msg_type = &set_msg_type0;
	Message->set_msg_content = &set_msg_content0;
	Message->get_msg = &get_msg0;
	Message->get_msg_type = &get_msg_type0;
	Message->get_msg_content = &get_msg_content0;
	Message->get_sender = &get_sender0;
	Message->get_target_agent = &get_target_agent0;
	Message->registration = &registration0;
	Message->deregistration = &deregistration0;
	Message->suspend = &suspend0;
	Message->resume = &resume0;
	Message->kill = &kill0;
	Message->restart = &restart0;
}
