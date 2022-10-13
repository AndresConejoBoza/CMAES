#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//void CreateOneShotBehaviourFunction(OneShotBehaviour* Behaviour) {
	//No tiene nada
//};

void actionFunctionOSB(OneShotBehaviour* Behaviour, void* pvParameters) {
	//Según la programación original, se le asignaba =0
};

void setupFunctionOSB(OneShotBehaviour* Behaviour, void* pvParameters) {
	//No tiene nada
};

bool doneFunctionOSB(OneShotBehaviour* Behaviour, void* pvParameters) {
	return true;
};

bool failure_detectionFunctionOSB(OneShotBehaviour* Behaviour, void* pvParameters) {
	return false;
};

void failure_identificationFunctionOSB(OneShotBehaviour* Behaviour, void* pvParameters) {
	//No tiene nada
};

void failure_recoveryFunctionOSB(OneShotBehaviour* Behaviour, void* pvParameters) {
	//No tiene nada
};

void executeFunctionOSB(OneShotBehaviour* Behaviour, void* pvParameters) {
	Behaviour->setup(Behaviour,pvParameters);
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


void ConstructorOneShotBehaviour(OneShotBehaviour* Behaviour) {
	//Behaviour->CreateOneShotBehaviour = &CreateOneShotBehaviourFunction;
	Behaviour->action = &actionFunctionOSB;
	Behaviour->setup = &setupFunctionOSB;
	Behaviour->done = &doneFunctionOSB;
	Behaviour->failure_detection = &failure_detectionFunctionOSB;
	Behaviour->failure_identification = &failure_detectionFunctionOSB;
	Behaviour->failure_recovery = &failure_recoveryFunctionOSB;
	Behaviour->execute = &executeFunctionOSB;
};