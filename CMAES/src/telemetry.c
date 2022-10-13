#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeMAES.h"

/* Demo includes. */
#include "supporting_functions.h"


/*Enums y structs*/
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



MAESAgent logger_current,logger_voltage,logger_temperature,measurement;
Agent_Platform Platform;
sysVars env;
CyclicBehaviour CurrentBehaviour,VoltageBehaviour,TemperatureBehaviour, genBehaviour;
Agent_Msg msg_current,msg_voltage,msg_temperature, msg_gen;
logger_info log_current, log_voltage, log_temperature, *info;
portFLOAT min, max, value;
char response[50];



void loggerAction(CyclicBehaviour* Behaviour,void* pvParameters) {
	printf("Entre a logger action\n");
	//info = (logger_info*)pvParameters;
	printf("Vamos a ver si algo llego a logger info: %i\n", info->rate);
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->set_msg_content(Behaviour->msg, (char*)info->type);
	Behaviour->msg->send(Behaviour->msg, measurement.AID(&measurement), portMAX_DELAY);
	Behaviour->msg->receive(Behaviour->msg,portMAX_DELAY);

	/*Logging*/
	printf("%s\n", Behaviour->msg->get_msg_content(&Behaviour->msg));
	Platform.agent_wait(&Platform,pdMS_TO_TICKS(info->rate));
	printf("Cuanto tiene que esperarse: %i\n", info->rate);
	printf("Cantidad de ticks: %i\n", info->rate);
};

void Currentlogger(void* pvParameters) {
	info = (logger_info*)pvParameters;
	//printf("Vamos a ver si algo llego a logger INICIAL info: %i\n", info->rate);
	//printf("Entre a logger\n");
	CurrentBehaviour.msg = &msg_current;
	CurrentBehaviour.action = &loggerAction;
	CurrentBehaviour.execute(&CurrentBehaviour, &pvParameters);
};

void Voltagelogger(void* pvParameters) {
	info = (logger_info*)pvParameters;
	//printf("Vamos a ver si algo llego a logger INICIAL info: %i\n", info->rate);
	//printf("Entre a logger\n");
	VoltageBehaviour.msg = &msg_voltage;
	VoltageBehaviour.action = &loggerAction;
	VoltageBehaviour.execute(&VoltageBehaviour, &pvParameters);
};

void Temperaturelogger(void* pvParameters) {
	info = (logger_info*)pvParameters;
	//printf("Vamos a ver si algo llego a logger INICIAL info: %i\n", info->rate);
	//printf("Entre a logger\n");
	TemperatureBehaviour.msg = &msg_temperature;
	TemperatureBehaviour.action = &loggerAction;
	TemperatureBehaviour.execute(&TemperatureBehaviour, &pvParameters);
};

void genAction(CyclicBehaviour* Behaviour, void* pvParameters) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->receive(Behaviour->msg,portMAX_DELAY);
	printf("Mensaje recibido: %i\n", (int)Behaviour->msg->get_msg_content(Behaviour->msg));
	switch ((int)Behaviour->msg->get_msg_content(Behaviour->msg))
	{
	case CURRENT:
		min = 0.1; //mA
		max = 1000; //mA
		value = min + rand() / (RAND_MAX / (max - min + 1) + 1);
		snprintf(response, 50, "\r\nCurrent mesasurment: %f\r\n", value);
		break;

	case VOLTAGE:
		min = 0.5; //V
		max = 3.3; //V
		value = min + rand() / (RAND_MAX / (max - min + 1) + 1);
		snprintf(response, 50, "\r\nVoltage mesasurment: %f\r\n", value);
		break;

	case TEMPERATURE:
		min = 30; //C
		max = 100; //C
		value = min + rand() / (RAND_MAX / (max - min + 1) + 1);
		snprintf(response, 50, "\r\nTemperature mesasurment: %f\r\n", value);
		break;

	default:
		snprintf(response, 50, "\r\nNot understood");
		break;
	}

	Behaviour->msg->set_msg_content(Behaviour->msg,response);
	Behaviour->msg->send(&msg_gen, msg_gen.get_sender(Behaviour->msg), portMAX_DELAY);
};

void gen(void* pvParameters) {
	genBehaviour.msg = &msg_gen;
	genBehaviour.action = &genAction;
	genBehaviour.execute(&genBehaviour, &pvParameters);
};


int telemetry() {
	printf("MAES DEMO \n");
	log_current.rate = 500;
	log_voltage.rate = 1000;
	log_temperature.rate = 2000;
	log_current.type = CURRENT;
	log_voltage.type = VOLTAGE;
	log_temperature.type = TEMPERATURE;

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
	logger_current.Iniciador(&logger_current, "Current Logger", 3, 128);
	logger_voltage.Iniciador(&logger_voltage, "Voltage Logger", 2, 128);
	logger_temperature.Iniciador(&logger_temperature, "Temperature Logger", 1, 128);
	measurement.Iniciador(&measurement, "Measure", 3, 128);
	Platform.Agent_Platform(&Platform, "telemetry_platform");



	Platform.agent_initConParam(&Platform,&logger_current, &Currentlogger, (void*)&log_current);
	//printf("Current iniciado\n");
	Platform.agent_initConParam(&Platform, &logger_voltage, &Voltagelogger, (void*)&log_voltage);
	//printf("Voltage iniciado\n");
	Platform.agent_initConParam(&Platform, &logger_temperature, &Temperaturelogger, (void*)&log_temperature);
	//printf("Temp iniciado\n");
	Platform.agent_init(&Platform, &measurement, &gen);
	//printf("Measurement iniciado\n");
	Platform.boot(&Platform);
	printf("Boot exitoso \n");
	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	for (;;);

	return 0;
}

