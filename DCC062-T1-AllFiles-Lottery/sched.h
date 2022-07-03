/*
*  sched.h - Header da API do escalonador de processos
*
*  Autor: Marcelo Moreno
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*  Organizacao: Universidade Federal de Juiz de Fora
*  Departamento: Dep. Ciencia da Computacao
*
*  => NAO MODIFIQUE ESTE ARQUIVO <=
*
*/

#ifndef SCHED_H
#define SCHED_H

#include "process.h"

#define MAX_NAME_LEN 4

//Tipo estruturado para o registro de algoritmos de escalonamento.
//- name e' o nome do algoritmo com MAX_NAME_LEN caracteres (excluindo \0)
//- initParamsFn e' a funcao para inicializar os parametros de escalonamento 
//               especificamente para o algoritmo
//- scheduleFn e' a funcao para decidir qual o proximo processo a obter a CPU
//             especificamente para o algoritmo
//- releaseParamsFn e' a funcao para liberar os parametros de escalonemnto de um
//                  processo especificamente para o algoritmo
typedef struct sched_info {
        char name[MAX_NAME_LEN+1];
        void (*initParamsFn)(Process *p, void *params);
	void (*notifyProcStatusChangeFn)(Process *p);
        Process* (*scheduleFn)(Process *plist);
        int (*releaseParamsFn)(Process *p);
} SchedInfo;

//Inicializa as informacoes sobre escalonadores
void schedInitSchedInfo(void);

//Retorna informacoes sobre escalonador de um dado slot
SchedInfo* schedGetSchedInfo(int slot);

//Notifica um algoritmo de escalonamento sobre a mudança de estado de um processo sob sua gerência
void schedNotifyProcStatusChange(Process* p);

//Aciona o escalonador de processos, que decide qual algoritmo deve ser usado
//e delega a esse algoritmo a decisao sobre qual processo obtera' a CPU
//Retorna NULL caso nao haja um processo pronto para assumir a CPU (idle)
//ou ponteiro para o processo escolhido
Process* schedSchedule(Process *plist);

//Associa um processo a um algoritmo de escalonamento especifico
//Retorna negativo caso o algoritmo de escalonamento nao seja encontrado
int schedSetScheduler(Process *p, void *params, int slot);

//Registra um algoritmo de escalonamento
//Se bem sucedido, retorna o numero do slot ocupado ou negativo caso contrario
int schedRegisterScheduler(SchedInfo *si);

//Remove um algoritmo de escalonamento
//Retorna numero do slot liberado se bem sucedido ou um negativo caso contrario
int schedUnregisterScheduler(int slot, char *name);

#endif
