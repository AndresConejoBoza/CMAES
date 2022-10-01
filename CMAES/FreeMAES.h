/***********************************************************
 *   MAES is a framework for Real Time Embedded Systems    *
 *   designed by C. Chan-Zheng at TU Delft.                *
 *   This is a library which implements the framework      *
 *   components to be compatible with FreeRTOS developed   *
 *   by D. Rojas Marin at ITCR.                            *
************************************************************/


#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>





#ifndef MAEStskKERNEL_VERSION_MAJOR
#define MAESTaskHandle_t xTaskHandle
#define MAESQueueHandle_t xQueueHandle
#define MAESTickType_t portTickType
#define MAESTaskFunction_t pdTASK_CODE
#define MAESUBaseType_t unsigned portBASE_TYPE
#define MAESBaseType_t portBASE_TYPE
#endif

#define Agent_AID MAESTaskHandle_t		        // Agent ID
#define Mailbox_Handle MAESQueueHandle_t        // Agent's Mailbox Handle
#define AGENT_LIST_SIZE 64					// Maximum Agents per platform
#define MAX_RECEIVERS AGENT_LIST_SIZE - 1	// Maximum receivers available for any agent
#define BEHAVIOUR_LIST_SIZE 8				//
#define ORGANIZATIONS_SIZE 16				// Maximum Members for org





	/*******************************************************
	 *                        ENUMS                        *
	 *******************************************************/

	typedef enum 
	{
		ACCEPT_PROPOSAL,
		AGREE,
		CANCEL,
		CFP,
		CONFIRM,
		DISCONFIRM,
		FAILURE,
		INFORM,
		INFORM_IF,
		INFORM_REF,
		NOT_UNDERSTOOD,
		PROPAGATE,
		PROPOSE,
		QUERY_IF,
		QUERY_REF,
		REFUSE,
		REJECT_PROPOSAL,
		REQUEST,
		REQUEST_WHEN,
		REQUEST_WHENEVER,
		SUBSCRIBE,
		NO_RESPONSE,
	}MSG_TYPE;

	typedef enum 
	{
		NO_ERRORS,
		FOUND,
		HANDLE_NULL,
		LIST_FULL,
		DUPLICATED,
		NOT_FOUND,
		TIMEOUT,
		INVALID,
		NOT_REGISTERED,
	}ERROR_CODE;

	typedef enum
	{
		ACTIVE,
		SUSPENDED,
		WAITING,
		TERMINATED,
		NO_MODE,
	}AGENT_MODE;

	typedef enum 
	{
		OWNER,
		ADMIN,
		MEMBER,
		NON_MEMBER,
	}ORG_AFFILIATION;

	typedef enum 
	{
		MODERATOR,
		PARTICIPANT,
		VISITOR,
		NONE,
	}ORG_ROLE;

	typedef enum
	{
		HIERARCHY,
		TEAM,
	}ORG_TYPE;

	/*******************************************************
	 *                     DEFINITIONS                     *
	 *******************************************************/

	 // Organization Information
	typedef struct
	{
		ORG_TYPE org_type;
		MAESUBaseType_t members_num;
		MAESUBaseType_t banned_num;
		Agent_AID members[AGENT_LIST_SIZE];
		Agent_AID banned[AGENT_LIST_SIZE];
		Agent_AID owner;
		Agent_AID admin;
		Agent_AID moderator;
	} org_info;

	// Agent Information
	typedef struct Agent_info Agent_info;
	struct Agent_info
	{
		Agent_AID aid;
		Mailbox_Handle mailbox_handle;
		const char* agent_name;
		MAESUBaseType_t priority;
		Agent_AID AP;
		org_info* org;
		ORG_AFFILIATION affiliation;
		ORG_ROLE role;
	};
	
	// Agent Resources
	typedef struct Agent_resources Agent_resources;
	struct Agent_resources
	{
		uint16_t stackSize;
		MAESTaskFunction_t function;
		void* taskParameters;
	};

	// Message
	typedef struct
	{
		Agent_AID sender_agent;
		Agent_AID target_agent;
		MSG_TYPE type;
		char* content;
	} MsgObj;

	// AP Description
	typedef struct
	{
		Agent_AID AMS_AID;
		const char* AP_name;
		MAESUBaseType_t subscribers;
	} AP_Description;


	/****************************************************** *
	 *                       CLASSES                       *
	 *******************************************************/

	typedef struct MAESAgent MAESAgent;
	struct MAESAgent 
	{
		Agent_info agent;
		Agent_resources resources;
		//void* (*IniciadorVacio)(MAESAgent*);
		void* (*Iniciador)(MAESAgent*,const char* ,MAESUBaseType_t ,uint16_t );
		Agent_AID* (*AID)(MAESAgent *);
	};

	typedef struct {
		Agent_AID first;
		MAESAgent* second;
	}sysVar;

	typedef struct sysVars sysVars;
	struct sysVars {
		MAESAgent* (* get_taskEnv)(sysVars*,Agent_AID aid);
		void* (*set_TaskEnv)(sysVars*, Agent_AID aid, MAESAgent* agent_ptr);
		void* (* erase_TaskEnv)(sysVars*,Agent_AID aid);
		sysVar* (*getEnv)(sysVars*);
		sysVar environment[AGENT_LIST_SIZE];
	};


//Clase Agent MSG//	

	typedef struct Agent_Msg Agent_Msg;
	struct Agent_Msg {
		MsgObj msg;
		sysVars* ptr_env;
		Agent_AID receivers[MAX_RECEIVERS];
		UBaseType_t subscribers;
		Agent_AID caller;
		bool (* isRegistered)(Agent_Msg*,Agent_AID aid); //Este recibía un resultado booleano, revisar eso.// 
		Mailbox_Handle* (* get_mailbox)(Agent_Msg*,Agent_AID aid);
		void* (*Agent_Msg)(Agent_Msg*);
		ERROR_CODE (*add_receiver)(Agent_Msg*,Agent_AID aid_receiver);
		ERROR_CODE (*remove_receiver)(Agent_Msg*,Agent_AID aid_receiver);
		void* (*clear_all_receiver)(Agent_Msg*);
		void* (*refresh_list)(Agent_Msg*);
		MSG_TYPE (*receive)(Agent_Msg*,MAESTickType_t timeout);
		ERROR_CODE (*send)(Agent_Msg*,Agent_AID aid_receiver, TickType_t timeout);
		ERROR_CODE (*send0)(Agent_Msg*);
		void* (*set_msg_type)(Agent_Msg*,MSG_TYPE type);
		void* (*set_msg_content)(Agent_Msg*,char* body);
		MsgObj* (*get_msg)(Agent_Msg*);
		MSG_TYPE (*get_msg_type)(Agent_Msg*);
		char* (*get_msg_content)(Agent_Msg*);
		Agent_AID* (*get_sender)(Agent_Msg*);
		Agent_AID* (*get_target_agent)(Agent_Msg*);
		ERROR_CODE (*registration)(Agent_Msg*,Agent_AID target_agent);
		ERROR_CODE (*deregistration)(Agent_Msg*,Agent_AID target_agent);
		ERROR_CODE (*suspend)(Agent_Msg*,Agent_AID target_agent);
		ERROR_CODE (*resume)(Agent_Msg*,Agent_AID target_agent);
		ERROR_CODE (*kill)(Agent_Msg*,Agent_AID target_agent);
		ERROR_CODE (*restart)(Agent_Msg*);
	};

	//CLASE USER_DEF_COND


	typedef struct USER_DEF_COND USER_DEF_COND;
	struct USER_DEF_COND {
		bool (*register_cond)(USER_DEF_COND*);
		bool (*deregister_cond)(USER_DEF_COND*);
		bool (*suspend_cond)(USER_DEF_COND*);
		bool (*kill_cond)(USER_DEF_COND*);
		bool (*resume_cond)(USER_DEF_COND*);
		bool (*restart_cond)(USER_DEF_COND*);
	};

// Clase Agent Platform//
	typedef struct Agent_Platform Agent_Platform;
	struct Agent_Platform {
		sysVars* ptr_env; //Le agregué este puntero al env porque casi todas las funciones de Agent Platform lo necesitan, entonces en lugar de hacer un extern env, se lo envío en el constructor.
		MAESAgent agentAMS;
		Agent_AID Agent_Handle[AGENT_LIST_SIZE];
		AP_Description description;
		USER_DEF_COND cond;
		USER_DEF_COND* ptr_cond;
		void* (*Agent_Platform)(Agent_Platform* platform,const char* name);
		void* (*Agent_PlatformWithCond)(Agent_Platform* platform, const char* name, USER_DEF_COND* user_cond);
		bool (*boot)(Agent_Platform* platform);
		void* (*agent_init)(Agent_Platform* platform, MAESAgent* agent, void behaviour(void* pvParameters));
		void* (*agent_initConParam)(Agent_Platform* platform, MAESAgent* agent, void behaviour(void* pvParameters), void* pvParameters);
		bool (*agent_search)(Agent_Platform* platform, Agent_AID aid);
		void* (* agent_wait)(Agent_Platform* platform, MAESTickType_t ticks);
		void* (*agent_yield)(Agent_Platform* platform);
		Agent_AID* (*get_running_agent)(Agent_Platform* platform);
		#ifdef tskKERNEL_VERSION_MAJOR
		AGENT_MODE (*get_state)(Agent_Platform* platform, Agent_AID aid);
		#endif
		Agent_info* (*get_Agent_description)(Agent_Platform* platform, Agent_AID aid);
		AP_Description* (*get_AP_description)(Agent_Platform* platform);
		ERROR_CODE (*register_agent)(Agent_Platform* platform, Agent_AID aid);
		ERROR_CODE (*deregister_agent)(Agent_Platform* platform, Agent_AID aid);
		ERROR_CODE (*kill_agent)(Agent_Platform* platform, Agent_AID aid);
		ERROR_CODE (*suspend_agent)(Agent_Platform* platform, Agent_AID aid);
		ERROR_CODE (*resume_agent)(Agent_Platform* platform, Agent_AID aid);
		void* (*restart)(Agent_Platform* platform, Agent_AID aid);
	};



	//Clase AMS Task
	typedef struct AMSparameter AMSparameter;
	struct AMSparameter{
		Agent_Platform* services;
		USER_DEF_COND* cond;
		void* (* AMS_task)(AMSparameter* parameter,void* pvParameters,sysVars* env);
	};


	//Clase CyclicBehaviour
	typedef struct CyclicBehaviour CyclicBehaviour;
	struct CyclicBehaviour {
		void* taskParameters;
		Agent_Msg msg;
		void* (* CreateCyclicBehaviour)(CyclicBehaviour* Behaviour);
		void* (* action)(CyclicBehaviour* Behaviour);
		void* (*setup)(CyclicBehaviour* Behaviour);
		bool (*done)(CyclicBehaviour* Behaviour);
		bool (*failure_detection)(CyclicBehaviour* Behaviour);
		void* (*failure_identification)(CyclicBehaviour* Behaviour);
		void* (*failure_recovery)(CyclicBehaviour* Behaviour);
		void* (*execute)(CyclicBehaviour* Behaviour);
	};
	//Clase OneShotBehaviour
	typedef struct OneShotBehaviour OneShotBehaviour;
	struct OneShotBehaviour {
		void* taskParameters;
		Agent_Msg msg;
		void* (*CreateOneShotBehaviour)(OneShotBehaviour* Behaviour);
		void* (*action)(OneShotBehaviour* Behaviour);
		void* (*setup)(OneShotBehaviour* Behaviour);
		bool (*done)(OneShotBehaviour* Behaviour);
		bool (*failure_detection)(OneShotBehaviour* Behaviour);
		void* (*failure_identification)(OneShotBehaviour* Behaviour);
		void* (*failure_recovery)(OneShotBehaviour* Behaviour);
		void* (*execute)(OneShotBehaviour* Behaviour);
	};


	typedef struct Agent_Organization Agent_Organization;
	struct Agent_Organization {
		sysVars* ptr_env;
		org_info description;
		void* (*Agent_Organization)(Agent_Organization* Organization,ORG_TYPE organization_type);
		ERROR_CODE (*create)(Agent_Organization* Organization);
		ERROR_CODE (*destroy)(Agent_Organization* Organization);
		ERROR_CODE (*isMember)(Agent_Organization* Organization,Agent_AID aid);
		ERROR_CODE (*isBanned)(Agent_Organization* Organization, Agent_AID aid);
		ERROR_CODE (*change_owner)(Agent_Organization* Organization, Agent_AID aid);
		ERROR_CODE (*set_admin)(Agent_Organization* Organization, Agent_AID aid);
		ERROR_CODE (*set_moderator)(Agent_Organization* Organization, Agent_AID aid);
		ERROR_CODE (*add_agent)(Agent_Organization* Organization, Agent_AID aid);
		ERROR_CODE (*kick_agent)(Agent_Organization* Organization, Agent_AID aid);
		ERROR_CODE (*ban_agent)(Agent_Organization* Organization, Agent_AID aid);
		ERROR_CODE (*remove_ban)(Agent_Organization* Organization, Agent_AID aid);
		void* (*clear_ban_list)(Agent_Organization* Organization);
		ERROR_CODE (*set_participan)(Agent_Organization* Organization, Agent_AID aid);
		ERROR_CODE (*set_visitor)(Agent_Organization* Organization, Agent_AID aid);
		ORG_TYPE (*get_org_type)(Agent_Organization* Organization);
		org_info (*get_info)(Agent_Organization* Organization);
		MAESUBaseType_t (*get_size)(Agent_Organization* Organization);
		MSG_TYPE (*invite)(Agent_Organization* Organization, Agent_Msg msg, UBaseType_t password, Agent_AID target_agent, UBaseType_t timeout);
	};


	void ConstructorAgente(MAESAgent* agente);
	void InicializadorSysVars(sysVars* Vars);
	void InicializadorAgent_Msg(Agent_Msg* Message, sysVars* env);
	void ConstructorAgent_Platform(Agent_Platform* platform, sysVars* env);
	void ConstructorAMS(AMSparameter* parameter);
	void ConstructorCyclicBehaviour(CyclicBehaviour* Behaviour);
	void ConstructorOneShotBehaviour(OneShotBehaviour* Behaviour);