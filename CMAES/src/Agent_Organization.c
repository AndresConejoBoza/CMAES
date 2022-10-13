#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void Agent_OrganizationFunction(Agent_Organization* Organization, ORG_TYPE organization_type) {
	Organization->description.org_type = organization_type;
	Organization->description.members_num = 0;
	Organization->description.banned_num = 0;

	for (MAESUBaseType_t i = 0; i < AGENT_LIST_SIZE; i++)
	{
		Organization->description.members[i] = NULL;
		Organization->description.banned[i] = NULL;
	}
};

ERROR_CODE createFunction(Agent_Organization* Organization) {
	if (xTaskGetCurrentTaskHandle() == NULL)
	{
		return INVALID;
	}
	else if (Organization->description.owner == NULL)
	{
		MAESAgent* agent = (MAESAgent*)Organization->ptr_env->get_taskEnv(Organization->ptr_env,xTaskGetCurrentTaskHandle());

		if (agent->agent.AP != NULL)
		{
			agent->agent.role = PARTICIPANT;
			agent->agent.affiliation = OWNER;
			agent->agent.org = &Organization->description;
			Organization->description.owner = xTaskGetCurrentTaskHandle();
			Organization->description.members[Organization->description.members_num] = Organization->description.owner;
			Organization->description.members_num++;
			return NO_ERRORS;
		}
		else
		{
			return NOT_REGISTERED;
		}
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE destroyFunction(Agent_Organization* Organization) {
	if (Organization->description.owner == xTaskGetCurrentTaskHandle())
	{
		MAESAgent* agent;
		for (UBaseType_t i = 0; i < Organization->description.members_num; i++)
		{
			agent = (MAESAgent*)Organization->ptr_env->get_taskEnv(Organization->ptr_env,Organization->description.members[i]);
			agent->agent.org = NULL;
			agent->agent.affiliation = NON_MEMBER;
			agent->agent.role = NONE;
			Organization->description.members[i] = NULL;
		}
		for (MAESUBaseType_t i = 0; i < Organization->description.banned_num; i++)
		{
			Organization->description.banned[i] = NULL;
		}
		return NO_ERRORS;
	}

	else
	{
		return INVALID;
	}
};

ERROR_CODE isMemberFunction(Agent_Organization* Organization, Agent_AID aid) {
	for (MAESUBaseType_t i = 0; i < Organization->description.members_num; i++)
	{
		if (Organization->description.members[i] == aid)
		{
			return FOUND;
		}
	}
	return NOT_FOUND;
};

ERROR_CODE isBannedFunction(Agent_Organization* Organization, Agent_AID aid) {
	for (MAESUBaseType_t i = 0; i < Organization->description.banned_num; i++)
	{
		if (Organization->description.banned[i] == aid)
		{
			return FOUND;
		}
	}
	return NOT_FOUND;
};

ERROR_CODE change_ownerFunction(Agent_Organization* Organization, Agent_AID aid) {
	if (Organization->description.owner == xTaskGetCurrentTaskHandle() && Organization->isMember(Organization,aid) == FOUND)
	{
		MAESAgent* agent = (MAESAgent*)Organization->ptr_env->get_taskEnv(Organization->ptr_env, Organization->description.owner);
		agent->agent.affiliation = MEMBER;
		agent->agent.role = VISITOR;
		agent = (MAESAgent*)Organization->ptr_env->get_taskEnv(Organization->ptr_env, aid);
		agent->agent.affiliation = OWNER;
		agent->agent.role = PARTICIPANT;
		Organization->description.owner = aid;
		return NO_ERRORS;
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE set_adminFunction(Agent_Organization* Organization, Agent_AID aid) {
	if (Organization->description.owner == xTaskGetCurrentTaskHandle() && Organization->isMember(Organization,aid) == FOUND)
	{
		MAESAgent* agent = (MAESAgent*)Organization->ptr_env->get_taskEnv(Organization->ptr_env, aid);
		if (agent->agent.affiliation != OWNER)
			agent->agent.affiliation = ADMIN;
		agent->agent.role = PARTICIPANT;
		Organization->description.admin = aid;
		return NO_ERRORS;
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE set_moderatorFunction(Agent_Organization* Organization, Agent_AID aid) {
	if (Organization->description.owner == xTaskGetCurrentTaskHandle() && Organization->isMember(Organization,aid) == FOUND)
	{
		MAESAgent* agent = (MAESAgent*)Organization->ptr_env->get_taskEnv(Organization->ptr_env, aid);
		agent->agent.role = MODERATOR;
		Organization->description.moderator = aid;
		return NO_ERRORS;
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE add_agentFunction(Agent_Organization* Organization, Agent_AID aid) {
	MAESAgent* agent = (MAESAgent*)Organization->ptr_env->get_taskEnv(Organization->ptr_env, aid);

	if (Organization->description.members_num == AGENT_LIST_SIZE)
		return LIST_FULL;
	else if (agent->agent.org != NULL || Organization->isBanned(Organization,aid) == FOUND || agent->agent.AP == NULL)
	{
		return INVALID;
	}
	else if (Organization->isMember(Organization,aid) == FOUND)
	{
		return DUPLICATED;
	}
	else if (Organization->description.owner == xTaskGetCurrentTaskHandle() || Organization->description.admin == xTaskGetCurrentTaskHandle())
	{
		Organization->description.members[Organization->description.members_num] = aid;
		Organization->description.members_num++;
		agent->agent.affiliation = MEMBER;
		agent->agent.role = PARTICIPANT;
		agent->agent.org = &Organization->description;
		return NO_ERRORS;
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE kick_agentFunction(Agent_Organization* Organization, Agent_AID aid) {
	if (aid == xTaskGetCurrentTaskHandle())
		return INVALID;
	else if (Organization->description.owner == xTaskGetCurrentTaskHandle() || Organization->description.admin == xTaskGetCurrentTaskHandle())
	{
		MAESUBaseType_t i = 0;
		MAESAgent* agent = (MAESAgent*)Organization->ptr_env->get_taskEnv(Organization->ptr_env, aid);
		while (i < AGENT_LIST_SIZE)
		{
			if (Organization->description.members[i] == aid)
			{
				while (i < AGENT_LIST_SIZE - 1)
				{
					Organization->description.members[i] = Organization->description.members[i + 1];
					i++;
				}
				Organization->description.members[AGENT_LIST_SIZE - 1] = NULL;
				Organization->description.members_num--;
				agent->agent.role = NONE;
				agent->agent.affiliation = NON_MEMBER;
				agent->agent.org = NULL;
				break;
			}
			i++;
		}
		if (i == AGENT_LIST_SIZE)
		{
			return NOT_FOUND;
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
};

ERROR_CODE ban_agentFunction(Agent_Organization* Organization, Agent_AID aid) {
	if (Organization->description.banned_num == AGENT_LIST_SIZE)
	{
		return LIST_FULL;
	}

	else if (Organization->isBanned(Organization,aid) == FOUND)
	{
		return DUPLICATED;
	}

	else if (Organization->description.owner == xTaskGetCurrentTaskHandle() || Organization->description.admin == xTaskGetCurrentTaskHandle())
	{
		Organization->description.banned[Organization->description.banned_num] = aid;
		Organization->description.banned_num++;
		if (Organization->isMember(Organization,aid) == FOUND)
			Organization->kick_agent(Organization,aid);
		return NO_ERRORS;
	}

	else
	{
		return INVALID;
	}
};

ERROR_CODE remove_banFunction(Agent_Organization* Organization, Agent_AID aid) {
	if (Organization->description.owner == xTaskGetCurrentTaskHandle() || Organization->description.admin == xTaskGetCurrentTaskHandle())
	{
		MAESUBaseType_t i = 0;
		while (i < AGENT_LIST_SIZE)
		{
			if (Organization->description.banned[i] == aid)
			{
				while (i < AGENT_LIST_SIZE - 1)
				{
					Organization->description.banned[i] = Organization->description.banned[i + 1];
					i++;
				}
				Organization->description.banned[AGENT_LIST_SIZE - 1] = NULL;
				Organization->description.banned_num--;
				break;
			}
			i++;
		}

		if (i == AGENT_LIST_SIZE)
		{
			return NOT_FOUND;
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
};

void clear_ban_listFunction(Agent_Organization* Organization) {
	if (Organization->description.owner == xTaskGetCurrentTaskHandle() || Organization->description.admin == xTaskGetCurrentTaskHandle())
	{
		for (MAESUBaseType_t i = 0; i < AGENT_LIST_SIZE; i++)
		{
			Organization->description.banned[i] = NULL;
		}
	}
};

ERROR_CODE set_participanFunction(Agent_Organization* Organization, Agent_AID aid) {
	if ((Organization->description.owner == xTaskGetCurrentTaskHandle() || Organization->description.moderator == xTaskGetCurrentTaskHandle()) && Organization->isMember(Organization,aid) == FOUND)
	{
		MAESAgent* agent = (MAESAgent*)Organization->ptr_env->get_taskEnv(Organization->ptr_env, aid);
		agent->agent.role = PARTICIPANT;
		return NO_ERRORS;
	}
	else
	{
		return INVALID;
	}
};

ERROR_CODE set_visitorFunction(Agent_Organization* Organization, Agent_AID aid) {
	if ((Organization->description.owner == xTaskGetCurrentTaskHandle() || Organization->description.moderator == xTaskGetCurrentTaskHandle()) && Organization->isMember(Organization,aid) == FOUND)
	{
		MAESAgent* agent = (MAESAgent*)Organization->ptr_env->get_taskEnv(Organization->ptr_env, aid);
		agent->agent.role = VISITOR;
		return NO_ERRORS;
	}
	else
	{
		return INVALID;
	}
};

ORG_TYPE get_org_typeFunction(Agent_Organization* Organization) {
	return Organization->description.org_type;
};

org_info get_infoFunction(Agent_Organization* Organization) {
	return Organization->description;
};

MAESUBaseType_t get_sizeFunction(Agent_Organization* Organization) {
	return Organization->description.members_num;
};

MSG_TYPE inviteFunction(Agent_Organization* Organization, Agent_Msg msg, UBaseType_t password, Agent_AID target_agent, UBaseType_t timeout) {
	MAESAgent* caller = (MAESAgent*)Organization->ptr_env->get_taskEnv(Organization->ptr_env, xTaskGetCurrentTaskHandle());

	if (caller->agent.affiliation == OWNER || caller->agent.affiliation == ADMIN)
	{
		msg.set_msg_type(&msg,PROPOSE);
		msg.set_msg_content(&msg,(char*)"Join Organization");
		msg.send(&msg,target_agent, timeout);

		msg.receive(&msg,timeout);

		if (msg.get_msg_type(&msg) == ACCEPT_PROPOSAL)
		{
			Organization->add_agent(Organization,target_agent);
		}
	}

	else
	{
		msg.set_msg_type(&msg,REFUSE);
	}
	return msg.get_msg_type(&msg);
};



void ConstructorAgent_Organization(Agent_Organization* Organization, sysVars* env) {
	Organization->ptr_env=env;
	Organization->Agent_Organization = &Agent_OrganizationFunction;
	Organization->create = &createFunction;
	Organization->destroy = &destroyFunction;
	Organization->isMember = &isMemberFunction;
	Organization->isBanned = &isBannedFunction;
	Organization->change_owner = &change_ownerFunction;
	Organization->set_admin = &set_adminFunction;
	Organization->set_moderator = &set_moderatorFunction;
	Organization->add_agent = &add_agentFunction;
	Organization->kick_agent = &kick_agentFunction;
	Organization->ban_agent = &ban_agentFunction;
	Organization->remove_ban = &remove_banFunction;
	Organization->clear_ban_list = &clear_ban_listFunction;
	Organization->set_participan = &set_participanFunction;
	Organization->set_visitor = &set_visitorFunction;
	Organization->get_org_type = &get_org_typeFunction;
	Organization->get_info = &get_infoFunction;
	Organization->get_size = &get_sizeFunction;
	Organization->invite = &inviteFunction;
};