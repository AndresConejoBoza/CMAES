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
CyclicBehaviour loggerBehaviour, genBehaviour;
Agent_Msg msg_logger, msg_gen;
logger_info log_current, log_voltage, log_temperature, *info;
portFLOAT min, max, value;
char response[50];



void loggerSetup(void* behaviour) {
	info = (logger_info*)behaviour;
};

void loggerAction(void* behaviour) {
	msg_logger.set_msg_content(&msg_logger, (char*)info->type);
	msg_logger.send(&msg_logger, measurement.AID(&measurement), portMAX_DELAY);
	msg_logger.receive(&msg_logger,portMAX_DELAY);

	/*Logging*/
	printf("%s\n", msg_logger.get_msg_content(&msg_logger));
	Platform.agent_wait(&Platform,pdMS_TO_TICKS(info->rate));
};

void logger(void* behaviour) {
	loggerBehaviour.setup = &loggerSetup;
	loggerBehaviour.action = &loggerAction;
	loggerBehaviour.execute(&loggerBehaviour);
};


void genAction(void* behaviour) {
	msg_gen.receive(&msg_gen,portMAX_DELAY);
	switch ((int)msg_gen.get_msg_content(&msg_gen))
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

	msg_gen.set_msg_content(&msg_gen,response);
	msg_gen.send(&msg_gen, msg_gen.get_sender(&msg_gen), portMAX_DELAY);
};

void gen(void* behaviour) {
	genBehaviour.action = &genAction;
	genBehaviour.execute(&genBehaviour);
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
	ConstructorAgent_Msg(&msg_logger, &env);
	ConstructorAgent_Msg(&msg_gen, &env);
	ConstructorCyclicBehaviour(&loggerBehaviour);
	ConstructorCyclicBehaviour(&genBehaviour);
	logger_current.Iniciador(&logger_current, "Current Logger", 1, 128);
	logger_voltage.Iniciador(&logger_voltage, "Voltage Logger", 3, 128);
	logger_temperature.Iniciador(&logger_temperature, "Temperature Logger", 1, 128);
	measurement.Iniciador(&measurement, "Measure", 3, 128);
	Platform.Agent_Platform(&Platform, "sender_receiver_platform");




	Platform.agent_init(&Platform,&logger_current, &logger, (void*)&log_current);
	Platform.agent_init(&Platform, &logger_voltage, &logger, (void*)&log_voltage);
	Platform.agent_init(&Platform, &logger_temperature, &logger, (void*)&log_temperature);
	Platform.agent_init(&Platform, &measurement, &gen);
	Platform.boot(&Platform);
	printf("Boot exitoso \n");
	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	for (;;);

	return 0;
}

