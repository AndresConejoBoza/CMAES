#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "CMAES.h"


/*Enums and structs*/
typedef enum meas_type
{
	CURRENT,
	VOLTAGE,
	TEMPERATURE
}meas_type;

typedef struct logger_info
{
	meas_type type;
	UBaseType_t rate;
}logger_info;


//Defining the app's variables.

MAESAgent logger_current,logger_voltage,logger_temperature,measurement;
Agent_Platform Platform;
sysVars env;
CyclicBehaviour CurrentBehaviour,VoltageBehaviour,TemperatureBehaviour, genBehaviour;
Agent_Msg msg_current,msg_voltage,msg_temperature, msg_gen;
logger_info log_current, log_voltage, log_temperature, *info, *infox;
portFLOAT min, max, value;
char response[50];


//Functions related to the Logger Action Behaviour//

void loggerAction(CyclicBehaviour* Behaviour,void* pvParameters) {
	info= (logger_info*)pvParameters;
	Behaviour->msg->set_msg_content(Behaviour->msg, (char*)info->type);
	Behaviour->msg->send(Behaviour->msg, measurement.AID(&measurement), portMAX_DELAY);
	Behaviour->msg->receive(Behaviour->msg,portMAX_DELAY);

	/*Logging*/
	printf("%s\n", Behaviour->msg->get_msg_content(Behaviour->msg));
	Platform.agent_wait(&Platform,info->rate);
};

//Current Logger Wrapper:
void Currentlogger(void* pvParameters) {
	CurrentBehaviour.msg = &msg_current;
	CurrentBehaviour.msg->Agent_Msg(CurrentBehaviour.msg);
	CurrentBehaviour.action = &loggerAction;
	CurrentBehaviour.execute(&CurrentBehaviour, pvParameters);
};

//Voltage Logger Wrapper:
void Voltagelogger(void* pvParameters) {
	VoltageBehaviour.msg = &msg_voltage;
	VoltageBehaviour.msg->Agent_Msg(VoltageBehaviour.msg);
	VoltageBehaviour.action = &loggerAction;
	VoltageBehaviour.execute(&VoltageBehaviour, pvParameters);
};

//Temperature Logger Wrapper:
void Temperaturelogger(void* pvParameters) {
	TemperatureBehaviour.msg = &msg_temperature;
	TemperatureBehaviour.msg->Agent_Msg(TemperatureBehaviour.msg);
	TemperatureBehaviour.action = &loggerAction;
	TemperatureBehaviour.execute(&TemperatureBehaviour, pvParameters);
};



//Functions related to the Generator Behaviour//

void genAction(CyclicBehaviour* Behaviour, void* pvParameters) {
	Behaviour->msg->receive(Behaviour->msg,portMAX_DELAY);
	int i = (int)Behaviour->msg->get_msg_content(Behaviour->msg);
	switch ((int)Behaviour->msg->get_msg_content(Behaviour->msg))
	{
	case CURRENT:
		min = 0.1; //mA
		max = 1000; //mA
		value = min + rand() / (RAND_MAX / (max - min + 1) + 1);
		snprintf(response, 50, "\r\nCurrent mesasurment: %f\r", value);
		break;

	case VOLTAGE:
		min = 0.5; //V
		max = 3.3; //V
		value = min + rand() / (RAND_MAX / (max - min + 1) + 1);
		snprintf(response, 50, "\r\nVoltage mesasurment: %f\r", value);
		break;

	case TEMPERATURE:
		min = 30; //C
		max = 100; //C
		value = min + rand() / (RAND_MAX / (max - min + 1) + 1);
		snprintf(response, 50, "\r\nTemperature mesasurment: %f\r", value);
		break;

	default:
		snprintf(response, 50, "\r\nNot understood");
		printf(response);
		break;
	}

	Behaviour->msg->set_msg_content(Behaviour->msg,response);
	Behaviour->msg->send(Behaviour->msg, Behaviour->msg->get_sender(Behaviour->msg), portMAX_DELAY);
};

//Generator Wrapper
void gen(void* pvParameters) {
	genBehaviour.msg = &msg_gen;
	genBehaviour.msg->Agent_Msg(genBehaviour.msg);
	genBehaviour.action = &genAction;
	genBehaviour.execute(&genBehaviour, &pvParameters);
};

//Main
int telemetry() {
	printf("------Telemetry------ \n");

	//Rates and types for each logger.
	log_current.rate = 500;
	log_voltage.rate = 1000;
	log_temperature.rate = 2000;
	log_current.type = CURRENT;
	log_voltage.type = VOLTAGE;
	log_temperature.type = TEMPERATURE;

	//Constructors for each initialized class
	ConstructorAgente(&logger_current);
	ConstructorAgente(&logger_voltage);
	ConstructorAgente(&logger_temperature);
	ConstructorAgente(&measurement);
	ConstructorSysVars(&env);
	ConstructorAgent_Platform(&Platform, &env);
	ConstructorAgent_Msg(&msg_current, &env);
	ConstructorAgent_Msg(&msg_voltage, &env);
	ConstructorAgent_Msg(&msg_temperature, &env);
	ConstructorAgent_Msg(&msg_gen, &env);
	ConstructorCyclicBehaviour(&CurrentBehaviour);
	ConstructorCyclicBehaviour(&VoltageBehaviour);
	ConstructorCyclicBehaviour(&TemperatureBehaviour);
	ConstructorCyclicBehaviour(&genBehaviour);

	//Initializing the Agents and the Platform.
	logger_current.Iniciador(&logger_current, "Current Logger", 3, 10);
	logger_voltage.Iniciador(&logger_voltage, "Voltage Logger", 2, 10);
	logger_temperature.Iniciador(&logger_temperature, "Temperature Logger", 1, 10);
	measurement.Iniciador(&measurement, "Measure", 3, 10);
	Platform.Agent_Platform(&Platform, "telemetry_platform");

	//Registering the Agents and their respective behaviour into the Platform
	Platform.agent_initConParam(&Platform,&logger_current, &Currentlogger, (void*)&log_current);
	Platform.agent_initConParam(&Platform, &logger_voltage, &Voltagelogger, (void*)&log_voltage);
	Platform.agent_initConParam(&Platform, &logger_temperature, &Temperaturelogger, (void*)&log_temperature);
	Platform.agent_init(&Platform, &measurement, &gen);
	Platform.boot(&Platform);
	printf("CMAES booted successfully \n");
	printf("Initiating APP\n\n");

	// Start the scheduler so the created tasks start executing.
	vTaskStartScheduler(); //Might have to be changed because CSP Library takes control of the FreeRTOS scheduler

	for (;;);
	return 0;
}

