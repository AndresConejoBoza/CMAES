#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


//void CreateCyclicBehaviourFunction(CyclicBehaviour* Behaviour) {
	//No tiene nada
//};

void actionFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	// Según la programación original, asignaba un =0
};

void setupFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	//No tiene nada
};

bool doneFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	return false;
};

bool failure_detectionFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	return false;
};

void failure_identificationFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	//No tiene nada
};

void failure_recoveryFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	//No tiene nada
};

void executeFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	//printf("Entre a la ejecucion del behavior\n");
	Behaviour->setup(Behaviour, pvParameters);
	do
	{
		//printf("Estoy metido dentro del ciclo del behaviour\n");
		Behaviour->action(Behaviour, pvParameters);
		//printf("Llegue a comparar si hay deteccion de fallas\n");
		if (Behaviour->failure_detection(Behaviour, pvParameters))
		{
			Behaviour->failure_identification(Behaviour, pvParameters);
			Behaviour->failure_recovery(Behaviour, pvParameters);
		}
	} while (!Behaviour->done(Behaviour, pvParameters));
};


void ConstructorCyclicBehaviour(CyclicBehaviour* Behaviour) {
	//Behaviour->CreateCyclicBehaviour = &CreateCyclicBehaviourFunction;
	Behaviour->action = &actionFunction;
	Behaviour->setup = &setupFunction;
	Behaviour->done = &doneFunction;
	Behaviour->failure_detection = &failure_detectionFunction;
	Behaviour->failure_identification = &failure_detectionFunction;
	Behaviour->failure_recovery = &failure_recoveryFunction;
	Behaviour->execute = &executeFunction;
};