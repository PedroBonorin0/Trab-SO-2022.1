/*
*  lottery.c - Implementacao do algoritmo Lottery Scheduling e sua API
*
*  Autores: SUPER_PROGRAMADORES_C
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*  Organizacao: Universidade Federal de Juiz de Fora
*  Departamento: Dep. Ciencia da Computacao
*
*/

#include "lottery.h"
#include <stdio.h>
#include <string.h>

//Nome unico do algoritmo. Deve ter 4 caracteres.
const char lottName[]="LOTT";
//=====Funcoes Auxiliares=====

int lottIndex = -1;
int distribute_tickets = 0;
int total_tickets;

void addReadyProc(Process* p) {
	LotterySchedParams *params;

	params = processGetSchedParams(p);
	params->num_minor_ticket = total_tickets;
	total_tickets += params->num_tickets;
	params->num_greater_ticket = total_tickets;
}

//=====Funcoes da API=====

//Funcao chamada pela inicializacao do S.O. para a incializacao do escalonador
//conforme o algoritmo Lottery Scheduling
//Deve envolver a inicializacao de possiveis parametros gerais
//Deve envolver o registro do algoritmo junto ao escalonador
void lottInitSchedInfo() {
	SchedInfo *sched = malloc(sizeof(SchedInfo)); // Cria o ponteiro sched e usa a função malloc para alocar espaço a partir da quantidade de bytes do objeto SchedInfo.

	total_tickets = 0;

	// Laço para definir o nome do escalonador
	for(int i = 0; i < 4; i++)
		sched->name[i] = lottName[i];

	// Define as funções necessárias para o escalonador funcionar corretamente
	sched->initParamsFn = &lottInitSchedParams;
	sched->notifyProcStatusChangeFn = &lottNotifyProcStatusChange;
	sched->scheduleFn = &lottSchedule;
	sched->releaseParamsFn = &lottReleaseParams;

	lottIndex = schedRegisterScheduler(sched); // Registra o algoritmo de escalonamento
}

//Inicializa os parametros de escalonamento de um processo p, chamada 
//normalmente quando o processo e' associado ao slot de Lottery
void lottInitSchedParams(Process *p, void *params) {
	schedSetScheduler(p, params, lottIndex);
	
}

//Recebe a notificação de que um processo sob gerência de Lottery mudou de estado
//Deve realizar qualquer atualização de dados da Loteria necessária quando um processo muda de estado
void lottNotifyProcStatusChange(Process* p) {
	LotterySchedParams* params;

	int status = processGetStatus(p); // Função que retorna o status do processo p

	if(status == PROC_READY) // Entra no if se o processo estiver pronto
		addReadyProc(p);
	else
		distribute_tickets = 1;
}

//Retorna o proximo processo a obter a CPU, conforme o algortimo Lottery 
Process* lottSchedule(Process *plist) {
	Process* p = plist; 
	Process* nextProc = NULL; // Cria  um novo processo que vai receber o processo sorteado

	LotterySchedParams *params;

	int drawn_ticket = -1;

	// Entra no if se for necessario calcular novamente os intervalos
	if(distribute_tickets == 1) {
		Process *proc;
		LotterySchedParams *procParams;
		total_tickets = 0;

		for(proc = plist; proc != NULL; proc = processGetNext(proc)) {
			if(processGetStatus(proc) == PROC_READY)
				addReadyProc(proc); // Chama a funcao addReadyProc() que insere o processo proc para o sorteio 
		}

		distribute_tickets = 0;
	}

	drawn_ticket = rand() % total_tickets; // Define um número aleatório entre 0 e o total de tickets dos processos que estão prontos

	// Laço para percorres todos os processos e avaliar se um processo pronto tem o ticket premiado
	for(p = plist; p != NULL; p = processGetNext(p)) {
		if (processGetStatus(p) == PROC_READY){
			params = processGetSchedParams(p);

			if (drawn_ticket >= params->num_minor_ticket &&
				  drawn_ticket < params->num_greater_ticket){
				nextProc = p;
				break;
			}
		}
	}

	return nextProc;
}

//Libera os parametros de escalonamento de um processo p, chamada 
//normalmente quando o processo e' desassociado do slot de Lottery
//Retorna o numero do slot ao qual o processo estava associado
int lottReleaseParams(Process* p) {
	int slot = processGetSchedSlot(p);

	LotterySchedParams* params = processGetSchedParams(p);
	free(params); // Utiliza a função free para desalocar e evitar vazamento de memória

	return slot;
}

//Transfere certo numero de tickets do processo src para o processo dst.
//Retorna o numero de tickets efetivamente transfeirdos (pode ser menos)
int lottTransferTickets(Process *src, Process *dst, int tickets) {
	
	int current;

	// Usa a função processGetSchedParams para pegar os paramentros dos processos src e dst
	LotterySchedParams* proc1 = processGetSchedParams(src);
	LotterySchedParams* proc2 = processGetSchedParams(dst);

	if(proc1->num_tickets < tickets)
		current = proc1->num_tickets;
	else
		current = tickets;

	proc1->num_tickets -= current;
	proc2->num_tickets += current;

	distribute_tickets = 1;
	return current;
}
