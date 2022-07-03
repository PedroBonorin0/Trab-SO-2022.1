/*
*  process.h - Definicao de estruturas e informacoes de processos
*
*  Autor: Marcelo Moreno
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*  Organizacao: Universidade Federal de Juiz de Fora
*  Departamento: Dep. Ciencia da Computacao
*
*  => NAO MODIFIQUE ESTE ARQUIVO <=
*
*/

#ifndef PROCESS_H
#define PROCESS_H

#include <stdlib.h>

#define PROC_INITIALIZING 0
#define PROC_WAITING 2
#define PROC_READY 4
#define PROC_RUNNING 8
#define PROC_TERMINATING 16

typedef struct proc Process;

//Retorna o pid de um processo
int processGetPid(Process *p);

//Retorna o pid do pai de um processo
int processGetParentPid(Process *p);

//Retorna o estado atual de um processo
int processGetStatus(Process *p);

//Retorna o tempo de uso da CPU por um processo
int processGetCpuUsage(Process *p);

//Retorna ponteiro generico de parametros de escalonamento
void* processGetSchedParams(Process *p);

//Retorna numero do slot ocupado pelo algoritmo de escalonamento associado
//Negativo caso nao haja algoritmo associado.
int processGetSchedSlot(Process *p);

//Retorna o proximo processo da lista
Process* processGetNext(Process *p);

//Retorna o processo anterior da lista
Process* processGetPrev(Process *p);

//Modifica o identificador do processo pai de um processo. O processo pai deve
//ter sido criado anteriormente a criacao do processo. Retorna negativo caso o
//pai nao exista ou o pid do processo modificado caso contrario
int processSetParentPid(Process *p, int ppid);

//Modifica o estado de um processo, caso a transicao de estado seja valida.
//Retorna negativo caso a transicao seja invalida ou o pid do processo
//modificado caso contrario
int processSetStatus(Process *p, int status);

//Adiciona valor ao tempo de uso da CPU. Retorna o tempo de uso ja atualizado
int processAddCpuUsage(Process *p, int add);

//Redireciona ponteiro de parametros de escalonamento para uma estrutura
//especifica. Atencao, a alocacao da estrutura nao pode ser desfeita ate'
//que outro redirecionamento seja feito, nem que seja para NULL, por exemplo.
void processSetSchedParams(Process *p, void *sp);

//Modifica o algoritmo de escalonamento associado, por meio do numero do slot
//registrado para o algoritmo
void processSetSchedSlot(Process *p, int slot);

//Retorna Processo a partir de seu identificador
Process* processGetByPid(Process *plist, int pid);

//Retorna Processo a partir de um estado de processo
Process* processGetByStatus(Process *plist, int status);

//Retorna Processo a partir de um algoritmo de escalon., identificado por slot
Process* processGetBySchedSlot(Process *plist, int slot);

//Cria Processo no inicio de uma lista e retorna a cabeca da lista
Process* processCreate(Process *plist);

//Destroi Processo a partir de pid e retorna cabeca da lista
Process* processDestroy(Process *plist, int pid);

//Imprime toda a lista de processos e suas informacoes
void processDump(Process *plist, void (*dumpSchedParamsFn)(Process *pid));

#endif
