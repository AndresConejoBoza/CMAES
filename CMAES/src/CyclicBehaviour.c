#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


void actionFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	// Empty
};

void setupFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	// Empty
};

bool doneFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	return false;
};

bool failure_detectionFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	return false;
};

void failure_identificationFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	// Empty
};

void failure_recoveryFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	// Empty
};

void executeFunction(CyclicBehaviour* Behaviour, void* pvParameters) {
	Behaviour->setup(Behaviour, pvParameters);
	do
	{
		Behaviour->action(Behaviour, pvParameters);
		if (Behaviour->failure_detection(Behaviour, pvParameters))
		{
			Behaviour->failure_identification(Behaviour, pvParameters);
			Behaviour->failure_recovery(Behaviour, pvParameters);
		}
	} while (!Behaviour->done(Behaviour, pvParameters));
};


void ConstructorCyclicBehaviour(CyclicBehaviour* Behaviour) {
	Behaviour->action = &actionFunction;
	Behaviour->setup = &setupFunction;
	Behaviour->done = &doneFunction;
	Behaviour->failure_detection = &failure_detectionFunction;
	Behaviour->failure_identification = &failure_detectionFunction;
	Behaviour->failure_recovery = &failure_recoveryFunction;
	Behaviour->execute = &executeFunction;
};