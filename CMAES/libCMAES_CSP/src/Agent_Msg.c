#include <CMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


//Is Registered Function: This function checks if an Agent is registered in the environment.
//Inputs: The Message instance itself and the AID.
//Outputs: True or False, depending on wether the agent was found or not.
bool isRegisteredFunction(Agent_Msg* Message, Agent_AID aid) { //Este recibía un resultado booleano, revisar eso.// No deja return true, hay que cambiar la lógica a 1 y Function.
	MAESAgent* description_receiver = (MAESAgent*)env.get_taskEnv(&env,aid);
	MAESAgent* description_sender = (MAESAgent*)env.get_taskEnv(&env,Message->caller);
	if (description_receiver->agent.AP == description_sender->agent.AP)
	{
		return 1;
	}
	else
	{
		return 0;
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


//Add Receiver Function: This function adds the AID of the agent that will receive a message into the list of the message receivers.
//Inputs:The message instance itself and the AID of the Agent that will receive the messages.
//Outputs: An struct Error Code that indicates if there was an error during the process of adding a new message receiver.
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

//Remove Receiver Function: This function removes the AID of the agent that will receive a message from the list of the message receivers.
//Inputs:The message instance itself and the AID of the Agent that will receive the messages.
//Outputs: An struct Error Code that indicates if there was an error during the process of adding a new message receiver.
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

//Clear All Receivers Function: This function removes the AID of all the agents that are listed in the agent receiver list.
//Inputs:The message instance itself.
//Outputs: None.
void clear_all_receiverFunction(Agent_Msg* Message) {
	MAESUBaseType_t i = 0;
	while (i < MAX_RECEIVERS)
	{
		Message->receivers[i] = NULL;
		i++;
	}
};

//Refresh List Function: This function removes the AID of all the agents that are listed in the agent receiver list.
//Inputs:The message instance itself.
//Outputs: None.
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

//Receive Function: This function uses the library CSP to receive a message from another agent.
//Inputs:The message instance itself and a timeout.
//Outputs: The message received or a no response error.
MSG_TYPE receiveFunction(Agent_Msg* Message, MAESTickType_t timeout) {
	if (csp_queue_dequeue(Message->get_mailbox(Message, Message->caller), &Message->msg, timeout) != pdPASS)  
	{
		return NO_RESPONSE;
	}
	else
	{
		return Message->msg.type;
	}
};

//Send Function: This function uses the library CSP to send a message to another agent.
//Inputs:The message instance itself, the receiving agent and a timeout.
//Outputs: An Error code indicating if the message was sent successfuly.
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
			if (csp_queue_enqueue(Message->get_mailbox(Message,aid_receiver), &Message->msg, timeout) != pdPASS) 
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
				if (csp_queue_enqueue(Message->get_mailbox(Message,aid_receiver), &Message->msg, timeout) != pdPASS)
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
			if (csp_queue_enqueue(Message->get_mailbox(Message,aid_receiver), &Message->msg, timeout) != pdPASS)
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

//Send to All Receivers Function: This function uses the Send function to deliver messages to all of the receivers listed in the message receivers list. 
//Inputs:The message instance itself.
//Outputs: An Error code indicating if the message was sent successfuly to each receiver.
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

//Set Msg Type Function: This function sets the message type. 
//Inputs:The message instance itself.
//Outputs: None.
void set_msg_typeFunction(Agent_Msg* Message, MSG_TYPE type) {
	Message->msg.type = type;
};

//Set Msg Content Function: This function sets the message content. 
//Inputs:The message instance itself.
//Outputs: None.
void set_msg_contentFunction(Agent_Msg* Message, char* msg_content) {
	Message->msg.content = msg_content;
};

//Get Msg Function: This function gets the message. 
//Inputs:The message instance itself.
//Outputs: A pointer to the message.
MsgObj* get_msgFunction(Agent_Msg* Message) {
	MsgObj* ptr = &Message->msg;
	return ptr;
};

//Get Msg Type Function: This function gets the message type. 
//Inputs:The message instance itself.
//Outputs: The message type.
MSG_TYPE get_msg_typeFunction(Agent_Msg* Message) {
	return Message->msg.type;
}

//Get Msg Content Function: This function gets the message content. 
//Inputs:The message instance itself.
//Outputs: The message content.
char* get_msg_contentFunction(Agent_Msg* Message) {
	return Message->msg.content;
};

//Get Sender Function: This function gets the message sender. 
//Inputs:The message instance itself.
//Outputs: The agent that sent the message.
Agent_AID get_senderFunction(Agent_Msg* Message) {
	return Message->msg.sender_agent;
};

//Get Target Agent Function: This function gets the agent that is going to receive the message. 
//Inputs:The message instance itself.
//Outputs: The message target.
Agent_AID get_target_agentFunction(Agent_Msg* Message) {
	return Message->msg.target_agent;
};

//Registration Function: This function sends a request to the AMS in order to register an agent in the platform. 
//Inputs:The message instance itself and the AID of the agent that will be registered.
//Outputs: An error code indicating if the registration was successful.
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
			if (csp_queue_enqueue(Message->get_mailbox(Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
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

//Deregistration Function: This function sends a request to the AMS in order to deregister an agent in the platform. 
//Inputs:The message instance itself and the AID of the agent that will be deregistered.
//Outputs: An error code indicating if the registration was successful.
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
			if (csp_queue_enqueue(Message->get_mailbox(Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
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

//Suspend Function: This function sends a request to the AMS in order to suspend an agent in the platform. 
//Inputs:The message instance itself and the AID of the agent that will be suspended.
//Outputs: An error code indicating if the suspension was successful.
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
			if (csp_queue_enqueue(Message->get_mailbox(Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
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

//Resume Function: This function sends a request to the AMS in order to resume an agent in the platform. 
//Inputs:The message instance itself and the AID of the agent that will be resumed.
//Outputs: An error code indicating if the agent was resumed.
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
			if (csp_queue_enqueue(Message->get_mailbox(Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
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

//Kill Function: This function sends a request to the AMS in order to kill an agent in the platform. 
//Inputs:The message instance itself and the AID of the agent that will be resumed.
//Outputs: An error code indicating if the agent was resumed.
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
			if (csp_queue_enqueue(Message->get_mailbox(Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
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

//Restart Function: This function sends a request to the AMS in order to restart an agent in the platform. 
//Inputs:The message instance itself and the AID of the agent that will be restarted.
//Outputs: An error code indicating if the agent was restarted.
ERROR_CODE restartMsgFunction(Agent_Msg* Message) {
	Agent_AID AMS;
	MAESAgent* agent_caller;
	agent_caller = (MAESAgent*)env.get_taskEnv(&env,Message->caller);
	Message->msg.type = REQUEST;
	Message->msg.content = (char*)"RESTART";
	Message->msg.target_agent = xTaskGetCurrentTaskHandle();
	Message->msg.sender_agent = xTaskGetCurrentTaskHandle();
	AMS = agent_caller->agent.AP;
	if (csp_queue_enqueue(Message->get_mailbox(Message,AMS), &Message->msg, portMAX_DELAY) != pdPASS)
	{
		return INVALID;
	}
	else
	{
		return NO_ERRORS;
	}
};


//Agent Message Constructor: This function assigns the class pointers to its corresponding function.
//Inputs: Ponter to the Agent Message class.
//Outputs: None.
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
