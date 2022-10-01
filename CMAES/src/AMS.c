#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



void AMS_taskFunction(AMSparameter* parameter, void* pvParameters, sysVars* env) {
	AMSparameter* amsParameters = (AMSparameter*)pvParameters;
	Agent_Platform* services = amsParameters->services;
	USER_DEF_COND* cond = amsParameters->cond;

	Agent_Msg msg;
	InicializadorAgent_Msg(&msg,env);

	MAESUBaseType_t error_msg = 0;
	for (;;)
	{
		msg.receive(&msg,portMAX_DELAY);
		if (msg.get_msg_type(&msg) == REQUEST)
		{
			if (strcmp(msg.get_msg_content(&msg), "KILL") == 0)
			{
				if (cond->kill_cond(cond))
				{
					error_msg = services->kill_agent(services,msg.get_target_agent(&msg));
					if (error_msg == NO_ERRORS)
					{
						msg.set_msg_type(&msg,CONFIRM);
					}
					else
					{
						msg.set_msg_type(&msg,REFUSE);
					}
				}
				else
				{
					msg.set_msg_type(&msg,REFUSE);
				}
				msg.send(&msg,msg.get_sender(&msg), 0);
			} //KILL Case

			else if (strcmp(msg.get_msg_content(&msg), "REGISTER") == 0)
			{
				if (cond->register_cond(cond))
				{
					error_msg = services->register_agent(services,msg.get_target_agent(&msg));
					if (error_msg == NO_ERRORS)
					{
						msg.set_msg_type(&msg,CONFIRM);
					}
					else
					{
						msg.set_msg_type(&msg,REFUSE);
					}
				}
				else
				{
					msg.set_msg_type(&msg,REFUSE);
				}
				msg.send(&msg,msg.get_sender(&msg), 0);
			} //REGISTER Case

			else if (strcmp(msg.get_msg_content(&msg), "DEREGISTER") == 0)
			{
				if (cond->deregister_cond(cond))
				{
					error_msg = services->deregister_agent(services,msg.get_target_agent(&msg));
					if (error_msg == NO_ERRORS)
					{
						msg.set_msg_type(&msg,CONFIRM);
					}
					else
					{
						msg.set_msg_type(&msg,REFUSE);
					}
				}
				else
				{
					msg.set_msg_type(&msg,REFUSE);
				}
				msg.send(&msg,msg.get_sender(&msg), 0);
			} //DEREGISTER Case

			else if (strcmp(msg.get_msg_content(&msg), "SUSPEND") == 0)
			{
				if (cond->suspend_cond(cond))
				{
					error_msg = services->suspend_agent(services,msg.get_target_agent(&msg));
					if (error_msg == NO_ERRORS)
					{
						msg.set_msg_type(&msg,CONFIRM);
					}
					else
					{
						msg.set_msg_type(&msg,REFUSE);
					}
				}
				else
				{
					msg.set_msg_type(&msg,REFUSE);
				}
				msg.send(&msg,msg.get_sender(&msg), 0);
			} //SUSPEND Case

			else if (strcmp(msg.get_msg_content(&msg), "RESUME") == 0)
			{
				if (cond->resume_cond(cond))
				{
					error_msg = services->resume_agent(services,msg.get_target_agent(&msg));
					if (error_msg == NO_ERRORS)
					{
						msg.set_msg_type(&msg,CONFIRM);
					}
					else
					{
						msg.set_msg_type(&msg,REFUSE);
					}
				}
				else
				{
					msg.set_msg_type(&msg,REFUSE);
				}
				msg.send(&msg,msg.get_sender(&msg), 0);
			} //RESUME Case

			else if (strcmp(msg.get_msg_content(&msg), "RESTART") == 0)
			{
				if (cond->restart_cond(cond))
				{
					services->restart(services,msg.get_target_agent(&msg));
				}
				else
				{
					msg.set_msg_type(&msg,REFUSE);
				}
				msg.send(&msg,msg.get_sender(&msg), 0);
			} //RESTART Case

			else
			{
				msg.set_msg_type(&msg,NOT_UNDERSTOOD);
				msg.send(&msg,msg.get_sender(&msg), 0);
			}
		} //end if
		else
		{
			msg.set_msg_type(&msg,NOT_UNDERSTOOD);
			msg.send(&msg,msg.get_sender(&msg), 0);
		}
	} // end while
};


void ConstructorAMS(AMSparameter* parameter) {
	parameter->AMS_task = &AMS_taskFunction;
};