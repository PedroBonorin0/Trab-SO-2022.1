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
int total_tickets;
int distribute_tickets = 0;

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
	SchedInfo *sched = malloc(sizeof(SchedInfo));

	total_tickets = 0;

	for(int i = 0; i < 4; i++)
		sched->name[i] = lottName[i];

	sched->initParamsFn = &lottInitSchedParams;
	sched->notifyProcStatusChangeFn = &lottNotifyProcStatusChange;
	sched->scheduleFn = &lottSchedule;
	sched->releaseParamsFn = &lottReleaseParams;

	lottIndex = schedRegisterScheduler(sched);
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

	int status = processGetStatus(p);

	if(status == PROC_READY)
		addReadyProc(p);
	else
		distribute_tickets = 1;
}

//Retorna o proximo processo a obter a CPU, conforme o algortimo Lottery 
Process* lottSchedule(Process *plist) {
	Process* p = plist;
	Process* nextProc = NULL;

	LotterySchedParams *params;

	int drawn_ticket = -1;

	if(distribute_tickets == 1) {
		Process *proc;
		LotterySchedParams *procParams;
		total_tickets = 0;

		for(proc = plist; proc != NULL; proc = processGetNext(proc)) {
			if(processGetStatus(proc) == PROC_READY)
				addReadyProc(proc);
		}

		distribute_tickets = 0;
	}

	drawn_ticket = rand() % total_tickets;

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
	free(params);

	return slot;
}

//Transfere certo numero de tickets do processo src para o processo dst.
//Retorna o numero de tickets efetivamente transfeirdos (pode ser menos)
int lottTransferTickets(Process *src, Process *dst, int tickets) {
	LotterySchedParams* proc1 = processGetSchedParams(src);
	LotterySchedParams* proc2 = processGetSchedParams(dst);

	int current;

	if(proc1->num_tickets < tickets)
		current = proc1->num_tickets;
	else
		current = tickets;

	proc1->num_tickets -= current;
	proc2->num_tickets += current;

	distribute_tickets = 1;
	return current;
}
