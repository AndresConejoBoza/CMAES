#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeMAES.h"


/* Demo includes. */
#include "supporting_functions.h"

MAESAgent AgentA, AgentB, Referee;
Agent_Platform Platform; 
sysVars env;
CyclicBehaviour playBehaviour;
OneShotBehaviour watchoverBehaviour;
Agent_Msg msg_play, msg_watchover;
int bet;
Agent_AID aidfunc;

//Funcion aleatoria
int getRandom() {
	printf("numero generado: %i", rand() % 2);
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

void playSetup(void* behaviour) {
	msg_play.Agent_Msg(&msg_play);
	msg_play.add_receiver(&msg_play, Referee.AID(&Referee));
};

void playAction(void* behaviour) {
	printf("\nEntre a play action\n");
	//aidfunc = Platform.get_running_agent(&Platform);
	//printf("adfunc: %p\n", &aidfunc);
	//printf("Nombre del agente: %s", Platform.get_running_agent(&Platform));
	//printf("Direccion de la plataforma: %p\n", Platform);
	printf("Nombre del agente jugando: %s\n",Platform.get_Agent_description(&Platform, Platform.get_running_agent(&Platform))->agent_name);
	printf(": Rock, Paper, Scissors... \n");
	Platform.agent_wait(&Platform, pdMS_TO_TICKS(10));
	auto num = getRandom();
	char* bet= "";
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
	msg_play.set_msg_content(&msg_play, bet);
	msg_play.set_msg_type(&msg_play, INFORM);
	msg_play.send0(&msg_play);
};

void play(void* behaviour) {
	printf("Entre a play");
	playBehaviour.setup = &playSetup;
	playBehaviour.action = &playAction;
	playBehaviour.execute(&playBehaviour);
};

void watchoverSetup(void* behaviour) {
	printf("Entre a watchover setup\n");
	msg_watchover.Agent_Msg(&msg_watchover);
	msg_watchover.add_receiver(&msg_watchover,AgentA.AID(&AgentA));
	msg_watchover.add_receiver(&msg_watchover,AgentB.AID(&AgentB));
	printf("Termine watchover setup\n");
};

void watchoverAction(void* behaviour) {
	printf("Entre a watchover action\n");
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
		msg_watchover.receive(&msg_watchover,portMAX_DELAY);
		if (msg_watchover.get_msg_type(&msg_watchover) == INFORM)
		{
			printf("\nPlaying now: \n");
			printf(Platform.get_Agent_description(&Platform,msg_watchover.get_sender(&msg_watchover))->agent_name);
			printf(": ");
			printf(msg_watchover.get_msg_content(&msg_watchover));
			printf("\n");
			if (msg_watchover.get_sender(&msg_watchover) == AgentA.AID(&AgentA))
			{
				printf("El sender era el Agente A\n");
				msgA = msg_watchover.get_msg_content(&msg_watchover);
				choiceA = choices(msgA);
			}
			else if (msg_watchover.get_sender(&msg_watchover) == AgentB.AID(&AgentB))
			{
				printf("El sender era el Agente B\n");
				msgB = msg_watchover.get_msg_content(&msg_watchover);
				choiceB = choices(msgB);
			}
			printf("Llegue al suspend\n");
			printf("");
			msg_watchover.suspend(&msg_watchover,msg_watchover.get_sender(&msg_watchover));
			printf("sali del suspend\n");
		}
		if (Platform.get_state(&Platform,AgentA.AID(&AgentA)) == SUSPENDED && Platform.get_state(&Platform,AgentB.AID(&AgentB)) == SUSPENDED) {
			break;
		}
	}
	printf("Llegue al switch\n");
	switch (winner[choiceA][choiceB])
	{
	case 0:
		printf(Platform.get_Agent_description(&Platform,Platform.get_running_agent(&Platform))->agent_name);
		printf(": DRAW!\n");
		break;

	case 1:
		printf(Platform.get_Agent_description(&Platform, Platform.get_running_agent(&Platform))->agent_name);
		printf(": PLAYER A WINS!\n");
		break;

	case 2:
		printf(Platform.get_Agent_description(&Platform, Platform.get_running_agent(&Platform))->agent_name);
		printf(": PLAYER B WINS!\n");
		break;

	default:
		break;
	}
	Platform.agent_wait(&Platform,pdMS_TO_TICKS(2000));
	msg_watchover.resume(&msg_watchover,AgentA.AID(&AgentA));
	msg_watchover.resume(&msg_watchover,AgentB.AID(&AgentB));
	printf("-------------------PLAYING AGAIN---------------------\n");
};

void watchover(void* behaviour) {
	printf("Entre a watchover\n");
	watchoverBehaviour.setup = &watchoverSetup;
	watchoverBehaviour.action = &watchoverAction;
	watchoverBehaviour.execute(&watchoverBehaviour);
};


int rock_paper_scissors() {

	ConstructorAgente(&AgentA);
	ConstructorAgente(&AgentB);
	ConstructorAgente(&Referee);
	ConstructorSysVars(&env);
	ConstructorAgent_Platform(&Platform, &env);
	ConstructorAgent_Msg(&msg_play, &env);
	ConstructorAgent_Msg(&msg_watchover, &env);
	ConstructorCyclicBehaviour(&playBehaviour);
	ConstructorOneShotBehaviour(&watchoverBehaviour);
	AgentA.Iniciador(&AgentA, "Player A", 1, 128);
	AgentB.Iniciador(&AgentB, "Player B", 1, 128);
	Referee.Iniciador(&Referee, "Referee", 2, 128);
	Platform.Agent_Platform(&Platform, "RPS_platform");
	printf("MAES DEMO \n");
	Platform.agent_init(&Platform,&AgentA, &play);
	Platform.agent_init(&Platform, &AgentB, &play);
	Platform.agent_init(&Platform, &Referee, &watchover);
	//printf("Pase los agent inits");
	Platform.boot(&Platform);
	printf("Boot exitoso \n");
	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	for (;;);

	return 0;
}