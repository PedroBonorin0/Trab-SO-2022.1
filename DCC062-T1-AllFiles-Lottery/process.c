/*
*  process.c - Definicao das operacoes sobre processos
*
*  Autor: Marcelo Moreno
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*  Organizacao: Universidade Federal de Juiz de Fora
*  Departamento: Dep. Ciencia da Computacao
*
*  => NAO MODIFIQUE ESTE ARQUIVO <=
*
*/

#include <stdlib.h>
#include <stdio.h>
#include "process.h"
#include "sched.h"

struct proc {
	int pid;		//Identificador do Processo
	int ppid;		//Identificador do Processo Pai
	int status;		//Status do Processo
	int cpu_usage;		//Tempo total de uso da CPU
	int sched_slot;		//Slot do algoritmo de escalonamento associado
	void* sched_params;	//Pont generico para parametros de escalonamento
	struct proc *prev;	//Encadeamento processo anterior
	struct proc *next;	//Encadeamento processo posterior
};

//Retorna o pid de um processo
int processGetPid(Process *p) {
	return p->pid;
}

//Retorna o pid do pai de um processo
int processGetParentPid(Process *p) {
	return p->ppid;
}

//Retorna o estado atual de um processo
int processGetStatus(Process *p) {
	return p->status;
}

//Retorna o tempo de uso da CPU por um processo
int processGetCpuUsage(Process *p) {
	return p->cpu_usage;
}

//Retorna ponteiro generico de parametros de escalonamento
void* processGetSchedParams(Process *p) {
	return p->sched_params;
}

//Retorna numero do slot ocupado pelo algoritmo de escalonamento associado
//Negativo caso nao haja algoritmo associado.
int processGetSchedSlot(Process *p) {
	return p->sched_slot;
}

//Retorna o proximo processo da lista
Process* processGetNext(Process *p) {
	return p->next;
}

//Retorna o processo anterior da lista
Process* processGetPrev(Process *p) {
	return p->prev;
}

//Modifica o identificador do processo pai de um processo. O processo pai deve
//ter sido criado anteriormente a criacao do processo. Retorna negativo caso o
//pai nao exista ou o pid do processo modificado caso contrario
int processSetParentPid(Process *p, int ppid) {
	Process *found = processGetByPid(p,ppid);
	if (!found) return -1;
	p->ppid = ppid;
	return p->pid;
}

//Modifica o estado de um processo, caso a transicao de estado seja valida.
//Retorna negativo caso a transicao seja invalida ou o pid do processo
//modificado caso contrario
int processSetStatus(Process *p, int status) {
	int ret = p->pid;
	switch (p->status) {
		case PROC_INITIALIZING:
			if (status==PROC_READY) p->status=status;
			else ret=-1;
			break;
		case PROC_READY: 
			if (status==PROC_RUNNING) p->status=status;
			else ret=-1;
			break;
		case PROC_WAITING:
			if (status==PROC_READY) p->status=status;
			else ret=-1;
			break;
		case PROC_RUNNING:
			if (status==PROC_READY || status==PROC_WAITING)
				p->status=status;
			else ret=-1;
			break;
		default:
			ret=-1;
	}
	if (ret == p->pid) schedNotifyProcStatusChange(p);
	return ret;
}

//Adiciona valor ao tempo de uso da CPU. Retorna o tempo de uso ja atualizado
int processAddCpuUsage(Process *p, int add) {
	return p->cpu_usage+=add;
}

//Redireciona ponteiro de parametros de escalonamento para uma estrutura
//especifica. Atencao, a alocacao da estrutura nao pode ser desfeita ate'
//que outro redirecionamento seja feito, nem que seja para NULL, por exemplo.
void processSetSchedParams(Process *p, void *sp) {
	p->sched_params = sp;
}

//Modifica o algoritmo de escalonamento associado, por meio do numero do slot
//registrado para o algoritmo
void processSetSchedSlot(Process *p, int slot) {
	p->sched_slot=slot;
}

//Retorna Processo a partir de seu identificador
Process* processGetByPid(Process *plist, int pid) {
	Process *current = plist;
	while(current!=NULL && current->pid!=pid)
		current=current->next;
	return current;
}

//Retorna Processo a partir de um estado de processo
Process* processGetByStatus(Process *plist, int status) {
	Process *current = plist;
	while(current!=NULL && current->status!=status)
		current=current->next;
	return current;
}

//Retorna Processo a partir de um algoritmo de escalon., identificado por slot
Process* processGetBySchedSlot(Process *plist, int slot) {
        Process *current = plist;
        while(current!=NULL && current->sched_slot!=slot)
                current=current->next;
        return current;
}

//Cria Processo no inicio de uma lista e retorna a cabeca da lista
Process* processCreate(Process *plist) {
	static int pidseed = 0;
	//Alocar e atribuir valores iniciais aos atributos do processo
	Process *newp = malloc(sizeof(Process));
	newp->pid = ++pidseed;
	newp->ppid = 0;
	newp->status = PROC_INITIALIZING;
	newp->cpu_usage = 0;
	newp->sched_params = NULL;
	newp->sched_slot=-1;
	//Se lista de processos nao for vazia, encadear no inicio dela
	if (plist) {
		newp->next = plist;
		newp->prev = plist->prev;
		plist->prev=newp;
	}
	else { //lista vazia, novo processo e' unico existente
		newp->next = NULL;
		newp->prev = newp;
	}
	return newp;
}

//Destroi Processo a partir de pid e retorna cabeca da lista
Process* processDestroy(Process *plist, int pid) {
	Process *found = processGetByPid(plist,pid);
	if (found) {
		SchedInfo* si;
		if (plist==found) { //Processo a ser destruido e' primeiro da lista
			plist=found->next;
			if (plist) plist->prev=found->prev;
		}
		else {
			if (found->next==NULL) plist->prev=found->prev;
			else found->next->prev=found->prev;
			found->prev->next=found->next;
		}
		si = schedGetSchedInfo(found->sched_slot);
		if (si) si->releaseParamsFn(found);
		found->sched_params = NULL;
		found->prev = NULL;
		found->next = NULL;
		free(found);
	}
	return plist;
}

//Imprime toda a lista de processos e suas informacoes
void processDump(Process *plist, void(*dumpSchedParamsFn)(Process*p)) {
	Process *current = plist;
	while(current!=NULL) {
		printf("PID: %d; STAT: %d; PPID: %d; CPU: %d; ",
		       current->pid, current->status, current->ppid, current->cpu_usage);
		dumpSchedParamsFn(current);
		printf("SS: %d; PRV: %d", current->sched_slot, current->prev->pid);
		if (current->next) printf(" NXT:%d", current->next->pid);
		printf("\n");
		current = current->next;
	}
}
