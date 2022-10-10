#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeMAES.h"


/* Demo includes. */
#include "supporting_functions.h"

MAESAgent AgentA, AgentB, Referee;
Agent_Platform AP_RPS; //Preguntarle a daniel por qué esta parte dice AP_RPS("windows")
sysVars env;
CyclicBehaviour playBehaviour;
OneShotBehaviour watchoverBehaviour;
Agent_Msg msg_play, msg_watchover;
int bet;

//Funcion aleatoria
int getRandom() {
	printf("numero generado: %i", rand(0, 2));
	return rand(0, 2);
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
	//printf("Nombre del agente: %s", AP_RPS.get_running_agent(&AP_RPS));
	printf(AP_RPS.get_Agent_description(&AP_RPS, AP_RPS.get_running_agent(&AP_RPS))->agent_name);
	printf(": Rock, Paper, Scissors... \n");
	AP_RPS.agent_wait(&AP_RPS, pdMS_TO_TICKS(10));
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
			//printf("Playing now: ");
			printf(AP_RPS.get_Agent_description(&AP_RPS,msg_watchover.get_sender(&msg_watchover))->agent_name);
			printf(": ");
			printf(msg_watchover.get_msg_content(&msg_watchover));
			printf("\n");
			if (msg_watchover.get_sender(&msg_watchover) == AgentA.AID(&AgentA))
			{
				msgA = msg_watchover.get_msg_content(&msg_watchover);
				choiceA = choices(msgA);
			}
			else if (msg_watchover.get_sender(&msg_watchover) == AgentB.AID(&AgentB))
			{
				msgB = msg_watchover.get_msg_content(&msg_watchover);
				choiceB = choices(msgB);
			}
			msg_watchover.suspend(&msg_watchover,msg_watchover.get_sender(&msg_watchover));
		}
		if (AP_RPS.get_state(&AP_RPS,AgentA.AID(&AgentA)) == SUSPENDED && AP_RPS.get_state(&AP_RPS,AgentB.AID(&AgentB)) == SUSPENDED) {
			break;
		}
	}

	switch (winner[choiceA][choiceB])
	{
	case 0:
		printf(AP_RPS.get_Agent_description(&AP_RPS,AP_RPS.get_running_agent(&AP_RPS))->agent_name);
		printf(": DRAW!\n");
		break;

	case 1:
		printf(AP_RPS.get_Agent_description(&AP_RPS, AP_RPS.get_running_agent(&AP_RPS))->agent_name);
		printf(": PLAYER A WINS!\n");
		break;

	case 2:
		printf(AP_RPS.get_Agent_description(&AP_RPS, AP_RPS.get_running_agent(&AP_RPS))->agent_name);
		printf(": PLAYER B WINS!\n");
		break;

	default:
		break;
	}
	AP_RPS.agent_wait(&AP_RPS,pdMS_TO_TICKS(2000));
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
	ConstructorAgent_Platform(&AP_RPS, &env);
	ConstructorAgent_Msg(&msg_play, &env);
	ConstructorAgent_Msg(&msg_watchover, &env);
	ConstructorCyclicBehaviour(&playBehaviour);
	ConstructorOneShotBehaviour(&watchoverBehaviour);
	AgentA.Iniciador(&AgentA, "Player A", 1, 512);
	AgentB.Iniciador(&AgentB, "Player B", 1, 512);
	Referee.Iniciador(&Referee, "Referee", 2, 512);

	printf("MAES DEMO \n");
	AP_RPS.agent_init(&AP_RPS,&AgentA, &play);
	AP_RPS.agent_init(&AP_RPS, &AgentB, &play);
	AP_RPS.agent_init(&AP_RPS, &Referee, &watchover);
	//printf("Pase los agent inits");
	AP_RPS.boot(&AP_RPS);
	printf("Boot exitoso \n");
	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	for (;;);

	return 0;
}