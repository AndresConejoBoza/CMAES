#include <FreeMAES.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void CreateOneShotBehaviourFunction(OneShotBehaviour* Behaviour) {
	//No tiene nada
};

void actionFunctionOSB(OneShotBehaviour* Behaviour) {
	//Según la programación original, se le asignaba =0
};

void setupFunctionOSB(OneShotBehaviour* Behaviour) {
	//No tiene nada
};

bool doneFunctionOSB(OneShotBehaviour* Behaviour) {
	return true;
};

bool failure_detectionFunctionOSB(OneShotBehaviour* Behaviour) {
	return false;
};

void failure_identificationFunctionOSB(OneShotBehaviour* Behaviour) {
	//No tiene nada
};

void failure_recoveryFunctionOSB(OneShotBehaviour* Behaviour) {
	//No tiene nada
};

void executeFunctionOSB(OneShotBehaviour* Behaviour) {
	Behaviour->setup(Behaviour);
	do
	{
		Behaviour->action(Behaviour);
		if (Behaviour->failure_detection(Behaviour))
		{
			Behaviour->failure_identification(Behaviour);
			Behaviour->failure_recovery(Behaviour);
		}
	} while (!Behaviour->done(Behaviour));
};


void ConstructorOneShotBehaviour(OneShotBehaviour* Behaviour) {
	Behaviour->CreateOneShotBehaviour = &CreateOneShotBehaviourFunction;
	Behaviour->action = &actionFunctionOSB;
	Behaviour->setup = &setupFunctionOSB;
	Behaviour->done = &doneFunctionOSB;
	Behaviour->failure_detection = &failure_detectionFunctionOSB;
	Behaviour->failure_identification = &failure_detectionFunctionOSB;
	Behaviour->failure_recovery = &failure_recoveryFunctionOSB;
	Behaviour->execute = &executeFunctionOSB;
};