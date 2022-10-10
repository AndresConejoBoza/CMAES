#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


//void CreateCyclicBehaviourFunction(CyclicBehaviour* Behaviour) {
	//No tiene nada
//};

void actionFunction(CyclicBehaviour* Behaviour) {
	// Según la programación original, asignaba un =0
};

void setupFunction(CyclicBehaviour* Behaviour) {
	//No tiene nada
};

bool doneFunction(CyclicBehaviour* Behaviour) {
	return false;
};

bool failure_detectionFunction(CyclicBehaviour* Behaviour) {
	return false;
};

void failure_identificationFunction(CyclicBehaviour* Behaviour) {
	//No tiene nada
};

void failure_recoveryFunction(CyclicBehaviour* Behaviour) {
	//No tiene nada
};

void executeFunction(CyclicBehaviour* Behaviour) {
	//printf("Entre a la ejecucion del behavior\n");
	Behaviour->setup(Behaviour);
	do
	{
		//printf("Estoy metido dentro del ciclo del behaviour\n");
		Behaviour->action(Behaviour);
		//printf("Llegue a comparar si hay deteccion de fallas\n");
		if (Behaviour->failure_detection(Behaviour))
		{
			Behaviour->failure_identification(Behaviour);
			Behaviour->failure_recovery(Behaviour);
		}
	} while (!Behaviour->done(Behaviour));
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