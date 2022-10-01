#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void Agent_OrganizationFunction(Agent_Organization* Organization, ORG_TYPE organization_type);
ERROR_CODE createFunction(Agent_Organization* Organization);
ERROR_CODE destroyFunction(Agent_Organization* Organization);
ERROR_CODE isMemberFunction(Agent_Organization* Organization, Agent_AID aid);
ERROR_CODE isBannedFunction(Agent_Organization* Organization, Agent_AID aid);
ERROR_CODE change_ownerFunction(Agent_Organization* Organization, Agent_AID aid);
ERROR_CODE set_adminFunction(Agent_Organization* Organization, Agent_AID aid);
ERROR_CODE set_moderatorFunction(Agent_Organization* Organization, Agent_AID aid);
ERROR_CODE add_agentFunction(Agent_Organization* Organization, Agent_AID aid);
ERROR_CODE kick_agentFunction(Agent_Organization* Organization, Agent_AID aid);
ERROR_CODE ban_agentFunction(Agent_Organization* Organization, Agent_AID aid);
ERROR_CODE remove_banFunction(Agent_Organization* Organization, Agent_AID aid);
void clear_ban_listFunction(Agent_Organization* Organization);
ERROR_CODE set_participanFunction(Agent_Organization* Organization, Agent_AID aid);
ERROR_CODE set_visitorFunction(Agent_Organization* Organization, Agent_AID aid);
ORG_TYPE get_org_typeFunction(Agent_Organization* Organization);
org_info get_infoFunction(Agent_Organization* Organization);
MAESUBaseType_t get_sizeFunction(Agent_Organization* Organization);
MSG_TYPE inviteFunction(Agent_Organization* Organization, Agent_Msg msg, UBaseType_t password, Agent_AID target_agent, UBaseType_t timeout);


void ConstructorAgent_Organization(Agent_Organization* Organization, sysVars* env) {
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