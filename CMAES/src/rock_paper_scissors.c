#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeMAES.h"


/* Demo includes. */
#include "supporting_functions.h"

MAESAgent AgentA, AgentB, Referee;
Agent_Platform Platform; 
sysVars env;
CyclicBehaviour playBehaviourA, playBehaviourB;
OneShotBehaviour watchoverBehaviour;
Agent_Msg msg_playA,msg_playB, msg_watchover;
int bet;


//Funcion aleatoria
int getRandom() {
	//printf("numero generado: %i", rand() % 2);
	return rand()%2;
};

//Funcion msg a int
int choices(char* msg) {
	if (msg == "ROCK")
	{
		return 0;
	}
	else if (msg == "PAPER")
	{
		return 1;
	}
	else
	{
		return 2; //scissors
	}
};

void playSetup(CyclicBehaviour* Behaviour, void* pvParameters) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->add_receiver(Behaviour->msg, Referee.AID(&Referee));
};

void playAction(CyclicBehaviour* Behaviour, void* pvParameters) {
	//printf("Caller: %p\n", Behaviour->msg->caller);
	//printf("%p\n", msg_playA.caller);
	//printf("%p\n", msg_playB.caller);
	Agent_info informacion = Platform.get_Agent_description(Platform.get_running_agent(&Platform));
	printf("Player: %s\n",informacion.agent_name);
	printf(": Rock, Paper, Scissors... \n");
	Platform.agent_wait(&Platform, pdMS_TO_TICKS(10));
	int num = getRandom();
	char* bet= "";
	printf("\nPlaying: %s\n", informacion.agent_name);
	switch (num){
	case 0:
		bet = "ROCK";
		break;

	case 1:
		bet = "PAPER";
		break;

	case 2:
		bet = "SCISSORS";
		break;

	default:
		break;
	}
	Behaviour->msg->set_msg_content(Behaviour->msg, bet);
	//printf("Paso el set content\n");
	Behaviour->msg->set_msg_type(Behaviour->msg, INFORM);
	Behaviour->msg->send0(Behaviour->msg);
};

void playA(void* pvParameters) {;
	playBehaviourA.msg = &msg_playA;
	playBehaviourA.setup = &playSetup;
	playBehaviourA.action = &playAction;
	for (;;) {
		playBehaviourA.execute(&playBehaviourA, &pvParameters);
	}
};

void playB(void* pvParameters) {
	playBehaviourB.msg = &msg_playB;
	playBehaviourB.setup = &playSetup;
	playBehaviourB.action = &playAction;
	for (;;) {
		playBehaviourB.execute(&playBehaviourB, &pvParameters);
	}
};


void watchoverSetup(OneShotBehaviour* Behaviour, void* pvParameters) {
	Behaviour->msg->Agent_Msg(Behaviour->msg);
	Behaviour->msg->add_receiver(Behaviour->msg,AgentA.AID(&AgentA));
	Behaviour->msg->add_receiver(Behaviour->msg,AgentB.AID(&AgentB));
};

void watchoverAction(OneShotBehaviour* Behaviour, void* pvParameters) {
	char* msgA;
	char* msgB;
	int choiceA;
	int choiceB;
	int winner[3][3] = {
		{0, 2, 1},
		{1, 0, 2},
		{2, 1, 0}
	};
	printf("\nREFEREE READY \n");
	while (true)
	{
		Behaviour->msg->receive(Behaviour->msg,portMAX_DELAY);
		if (Behaviour->msg->get_msg_type(Behaviour->msg) == INFORM)
		{
			//printf("\nPlaying now: \n");
			Agent_info PlayerInfo = Platform.get_Agent_description(Behaviour->msg->get_sender(Behaviour->msg));
			//printf("\n");
			printf(PlayerInfo.agent_name);
			printf(": ");
			printf(Behaviour->msg->get_msg_content(Behaviour->msg));
			printf("\n");
			if (Behaviour->msg->get_sender(Behaviour->msg) == AgentA.AID(&AgentA))
			{
				//printf("El sender era el Agente A\n");
				msgA = Behaviour->msg->get_msg_content(Behaviour->msg);
				choiceA = choices(msgA);
			}
			else if (Behaviour->msg->get_sender(Behaviour->msg) == AgentB.AID(&AgentB))
			{
				//printf("El sender era el Agente B\n");
				//printf("AID del Agente que envio el mensaje: %p\n", msg_watchover.get_sender(&msg_watchover));
				//printf("AID del Agente B: %p\n", AgentA.AID(&AgentA));
				//printf("AID del Agente B: %p\n", AgentB.AID(&AgentB));
				//printf("Nombre del Agente que envio el mensaje: %s\n", Platform.get_Agent_description(&Platform, AgentA.AID(&AgentA)).agent_name);
				//printf("Nombre del Agente que envio el mensaje: %s\n",Platform.get_Agent_description(&Platform, AgentB.AID(&AgentB)).agent_name);//Por alguna razon el AID del agente B es el del referee
				//printf("Nombre del Agente que envio el mensaje: %s\n", Platform.get_Agent_description(&Platform, Referee.AID(&Referee)).agent_name);
				//printf("AID del Agente Referee: %p\n", Referee.AID(&Referee));
				msgB = Behaviour->msg->get_msg_content(Behaviour->msg);
				choiceB = choices(msgB);
			}
			//printf("Llegue al suspend\n");
			//printf("");
			//printf("Se suspende a: %p\n", Behaviour->msg->get_sender(Behaviour->msg));
			Behaviour->msg->suspend(Behaviour->msg, Behaviour->msg->get_sender(Behaviour->msg));
			//printf("sali del suspend\n");
		}
		if (Platform.get_state(&Platform,AgentA.AID(&AgentA)) == SUSPENDED && Platform.get_state(&Platform,AgentB.AID(&AgentB)) == SUSPENDED) {
			break;
		}
	}
	//printf("Llegue al switch\n");
	Agent_info RefereeInfo = Platform.get_Agent_description(Platform.get_running_agent(&Platform));
	switch (winner[choiceA][choiceB])
	{
	case 0:
		printf("\n");
		printf(RefereeInfo.agent_name);
		printf(": DRAW!\n");
		break;

	case 1:
		printf("\n");
		printf(RefereeInfo.agent_name);
		printf(": PLAYER A WINS!\n");
		break;

	case 2:
		printf("\n");
		printf(RefereeInfo.agent_name);
		printf(": PLAYER B WINS!\n");
		break;

	default:
		break;
	}
	Platform.agent_wait(&Platform,pdMS_TO_TICKS(2000));
	Behaviour->msg->resume(Behaviour->msg,AgentA.AID(&AgentA));
	Behaviour->msg->resume(Behaviour->msg,AgentB.AID(&AgentB));
	printf("-------------------PLAYING AGAIN---------------------\n");
};

void watchover(void* pvParameters) {
	watchoverBehaviour.msg = &msg_watchover;
	//printf("Entre a watchover\n");
	watchoverBehaviour.setup = &watchoverSetup;
	watchoverBehaviour.action = &watchoverAction;
	for (;;) {
		watchoverBehaviour.execute(&watchoverBehaviour, &pvParameters);
	}
};


int rock_paper_scissors() {

	ConstructorAgente(&AgentA);
	ConstructorAgente(&AgentB);
	ConstructorAgente(&Referee);
	ConstructorSysVars(&env);
	ConstructorAgent_Platform(&Platform, &env);
	ConstructorAgent_Msg(&msg_playA, &env);
	ConstructorAgent_Msg(&msg_playB, &env);
	ConstructorAgent_Msg(&msg_watchover, &env);
	ConstructorCyclicBehaviour(&playBehaviourA);
	ConstructorCyclicBehaviour(&playBehaviourB);
	ConstructorOneShotBehaviour(&watchoverBehaviour);
	AgentA.Iniciador(&AgentA, "Player A", 1, 20);
	AgentB.Iniciador(&AgentB, "Player B", 1, 20);
	Referee.Iniciador(&Referee, "Referee", 2, 20);
	Platform.Agent_Platform(&Platform, "RPS_platform");
	printf("MAES DEMO \n");
	Platform.agent_init(&Platform,&AgentA, &playA);
	Platform.agent_init(&Platform, &AgentB, &playB);
	Platform.agent_init(&Platform, &Referee, &watchover);
	//printf("AID de Player A: %p\n", AgentA.AID(&AgentA));
	//printf("AID de Player B: %p\n", AgentB.AID(&AgentB));
	//printf("Pase los agent inits");
	Platform.boot(&Platform);
	printf("Boot exitoso \n");
	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	for (;;);

	return 0;
}