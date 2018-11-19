/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 743506
 * Aluno: André Matheus Bariani Trava
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51
#define TAM_STRING_INDICE (TAM_MARCA + TAM_NOME)


#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!"
#define INICIO_BUSCA 				"********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM				"********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 			"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO				"********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO      "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO    "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 			"Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 			"Busca por %s.\nNos percorridos:\n"


/* Registro do Produto */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;


/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_ip *chave;	/* vetor das chaves e rrns [m-1]*/
	int *desc;			/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_is *chave;	/* vetor das chaves e rrns [m-1]*/
	int *desc;			/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_is;

typedef struct
{
	int rrn_novo_no;
	char chave_promovida[TAM_PRIMARY_KEY];
} Btree_ip_insert_return;

typedef struct {
	int raiz;
} Indice;

/* Variáveis globais */
char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO + 1];
char ARQUIVO_IP[2000*sizeof(Chave_ip)];
char ARQUIVO_IS[2000*sizeof(Chave_is)];
/* Ordem das arvores */
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

 /********************FUNÇÕES DO MENU*********************/
 void cadastrar(Indice* iprimary, Indice* ibrand);

 int alterar(Indice iprimary);

 void buscar(Indice iprimary,Indice ibrand);

 void listar(Indice iprimary,Indice ibrand);

 /*******************************************************/

void gerarChave(Produto *p);

 /* Realiza os scanfs na struct Produto */
 void ler_entrada(char* registro, Produto *novo);

 /* Insere produto novo no arquivo de dados */
 void inserir_arquivo(Produto *p);

 /* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
 int carregar_arquivo();

Produto recuperar_registro(int rrn);

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);

/* (Re)faz o índice de produtos  */
void criar_ibrand(Indice *ibrand);

// /*Escreve um nó da árvore no arquivo de índice,
// * O terceiro parametro serve para informar qual indice se trata */
// void write_btree(void *salvar, int rrn, char ip);
//
// /*Lê um nó do arquivo de índice e retorna na estrutura*/
// void *read_btree(int rrn, char ip);
//
// /* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
// * É conveniente que essa função também inicialize os campos necessários com valores nulos*/
// void *criar_no(char ip);
//
// /*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
// void libera_no(void *node, char ip);

/*
*   Caro aluno,
*   caso não queira trabalhar com void*, é permitido dividir as funções que utilizam
* em duas, sendo uma para cada índice...
* Um exemplo, a write_btree e read_btree ficariam como:
*
*   void write_btree_ip(node_Btree_ip *salvar, int rrn),  node_Btree_ip *read_btree_ip(int rrn),
*   void write_btree_is(node_Btree_is *salvar, int rrn) e node_Btree_is *read_btree_is(int rrn).
*/

/*Escreve um nó da árvore no arquivo de índice*/
void write_btree_ip(node_Btree_ip *salvar, int rrn);
void write_btree_is(node_Btree_is *salvar, int rrn);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
node_Btree_ip *read_btree_ip(int rrn);
node_Btree_is *read_btree_is(int rrn);

/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
node_Btree_ip *criar_no_ip();
node_Btree_is *criar_no_is();

Btree_ip_insert_return *divide_no(int rrn_X, int rrn_filho_direito, char * p);
Btree_ip_insert_return *insere_aux_btree_ip(int rrn, char * p);
void insere_btree_ip(Indice *iprimary, char * p);

node_Btree_ip *busca_no_ip(node_Btree_ip *no_atual, char * k);
node_Btree_is *busca_no_is(node_Btree_is *no_atual, char * k);

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip);

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto j);

/* Exibe o produto */
int exibir_registro(int rrn);

int main()
{
	char *p; /* # */
  /* Arquivo */
	int carregarArquivo = 0;
	nregistros = 0; nregistrosip = 0; nregistrosis = 0;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = ordem_ip*3 + 4 + (-1 + ordem_ip)*14;
	tamanho_registro_is = ordem_is*3 + 4 + (-1 + ordem_is)* (TAM_STRING_INDICE +9);

	/* Índice primário */
	Indice iprimary;
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Produtos */
	Indice ibrand;
	criar_ibrand(&ibrand);

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo Produto */
			cadastrar(&iprimary, &ibrand);
			break;
		case 2: /* Alterar o desconto de um Produto */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar um Produto */
			printf(INICIO_BUSCA);
			buscar(iprimary, ibrand);
			break;
		case 4: /* Listar todos os Produtos */
			printf(INICIO_LISTAGEM);
			listar(iprimary, ibrand);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if(!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IP; *p!='\0'; p+=tamanho_registro_ip)
				{
					fwrite( p , 1 ,tamanho_registro_ip,stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if(!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IS; *p!='\0'; p+=tamanho_registro_is)
				{
					fwrite( p , 1 ,tamanho_registro_is,stdout);
					puts("");

				}
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary) {
	 if(!nregistros) {
		 iprimary->raiz = -1;
	 }
}

/* (Re)faz o índice de produtos  */
void criar_ibrand(Indice *ibrand) {
	 if(!nregistros)
	 	ibrand->raiz = -1;
}

node_Btree_ip *criar_no_ip() {
	node_Btree_ip *no_ip = (node_Btree_ip *) malloc (sizeof(node_Btree_ip));
	no_ip->num_chaves = 0;
	no_ip->chave = (Chave_ip *) malloc ((ordem_ip - 1) * sizeof(Chave_ip));

	no_ip->desc = (int *) malloc ((ordem_ip) * sizeof(int));
	for (int i = 0; i < ordem_ip; i++) {
		no_ip->desc[i] = -1;
	}

	no_ip->folha = 'N';
	return no_ip;
}

node_Btree_is *criar_no_is() {
	node_Btree_is *no_is = (node_Btree_is *) malloc (sizeof(node_Btree_is));
	no_is->num_chaves = 0;
	no_is->chave = (Chave_is *) malloc ((ordem_is - 1) * sizeof(Chave_is));

	no_is->desc = (int *) malloc ((ordem_is) * sizeof(int));
	for (int i = 0; i < ordem_is; i++) {
		no_is->desc[i] = -1;
	}

	no_is->folha = 'N';
	return no_is;
}

Btree_ip_insert_return *divide_no(int rrn_X, int rrn_filho_direito, char * k) {
	node_Btree_ip *X = read_btree_ip(rrn_X);
	int i = X->num_chaves - 1;
	int chave_alocada = 0;

	node_Btree_ip *Y = criar_no_ip();
	Y->folha = X->folha;
	Y->num_chaves = floor((ordem_ip - 1)/2);

	for(int j = Y->num_chaves ; j >= 0 ; j--) {
		if(!chave_alocada && (strcmp(k, X->chave[i].pk) > 0)) {
			strncpy(Y->chave[j].pk, k, TAM_PRIMARY_KEY);
			Y->desc[j + 1] = rrn_filho_direito;
			chave_alocada = 1;
		} else {
			strncpy(Y->chave[j].pk, X->chave[j].pk, TAM_PRIMARY_KEY);
			Y->chave[j].rrn = X->chave[j].rrn;
			Y->desc[j + 1] = X->desc[i + 1];
			i--;
		}
	}

	if(!chave_alocada) {
		while(i >= 1 && (strcmp(k, X->chave[i].pk) < 0)) {
			strncpy(X->chave[i + 1].pk, X->chave[i].pk, TAM_PRIMARY_KEY);
			X->chave[i + 1].rrn = X->chave[i].rrn;
			X->desc[i + 2] = X->desc[i + 1];
			i--;
		}
		strncpy(X->chave[i + 1].pk, k, TAM_PRIMARY_KEY);
		X->desc[i + 2] = rrn_filho_direito;
	}

	char chave_promovida[TAM_PRIMARY_KEY];
	int aux = floor(ordem_ip/2);
	strncpy(chave_promovida, X->chave[aux + 1].pk, TAM_PRIMARY_KEY);
	Y->desc[0] = X->desc[aux + 2];
	X->num_chaves = aux;

	write_btree_ip(Y, nregistrosip);

	Btree_ip_insert_return *ret = (Btree_ip_insert_return *) malloc(sizeof(Btree_ip_insert_return));
	ret->rrn_novo_no = nregistrosip;
	strncpy(ret->chave_promovida, chave_promovida, TAM_PRIMARY_KEY);
	nregistrosip++;
	free(X);
	free(Y);
	return ret;
}

Btree_ip_insert_return *insere_aux_btree_ip(int rrn, char * k) {
	node_Btree_ip *X = read_btree_ip(rrn);
	if(X->folha == 'F') {
		if(X->num_chaves < (ordem_ip - 1)) {
			int i = X->num_chaves;
			while(i >= 0  && (strcmp(k, X->chave[i].pk) < 0)) {
				strncpy(X->chave[i + 1].pk, X->chave[i].pk, TAM_PRIMARY_KEY);
				X->chave[i + 1].rrn = X->chave[i].rrn;
				i--;
			}
			strncpy(X->chave[i].pk, k, TAM_PRIMARY_KEY);
			X->chave[i].rrn = nregistros;
			X->num_chaves++;
			write_btree_ip(X, rrn);
			free(X);
			return NULL;
		} else {
			free(X);
			return divide_no(rrn, -1, k);
		}
	} else {
		int i = X->num_chaves;
		while(i >= 0 && (strcmp(k, X->chave[i].pk) < 0)) {
			i--;
		}
		i++;

		Btree_ip_insert_return *ret = insere_aux_btree_ip(X->desc[i], k);

		if(ret != NULL) {
			int filho_direito = ret->rrn_novo_no;
			char *promovida = ret->chave_promovida;
			strncpy(k, promovida, TAM_PRIMARY_KEY);
			if( X->num_chaves < (ordem_ip - 1)) {
				int i = X->num_chaves;
				while(i >= 0 && (strcmp(k, X->chave[i].pk) == -1)) {
					strncpy(X->chave[i + 1].pk, X->chave[i].pk, TAM_PRIMARY_KEY);
					X->chave[i + 1].rrn = X->chave[i].rrn;
					X->desc[i + 2] = X->desc[i + 1];
					i--;
				}
				strncpy(X->chave[i + 1].pk, k, TAM_PRIMARY_KEY);
				X->desc[i + 2] = filho_direito;

				free(X);
				free(ret);
				return NULL;
			} else {
				return divide_no(rrn, filho_direito, k);
			}
		} else {
			free(X);
			free(ret);
			return NULL;
		}
	}
}

void insere_btree_ip(Indice *iprimary, char * k) {
	node_Btree_ip *raiz = read_btree_ip(iprimary->raiz);
	if(raiz == NULL) {
		node_Btree_ip *novo = criar_no_ip();
		novo->folha = 'F';
		novo->num_chaves = 1;
		strncpy(novo->chave[0].pk, k, TAM_PRIMARY_KEY);
		novo->chave[0].pk[TAM_PRIMARY_KEY] = '\0';
		novo->chave[0].rrn = nregistros;

		iprimary->raiz = nregistros;
		nregistrosip++;
		write_btree_ip(novo, 0);
		free(novo);
	} else {
		free(raiz);
		Btree_ip_insert_return *ret = insere_aux_btree_ip(iprimary->raiz, k);

		if(ret != NULL) {
			int filho_direito = ret->rrn_novo_no;
			char *chave_promovida = ret->chave_promovida;

			node_Btree_ip *novo = criar_no_ip();
			novo->folha = 'N';
			novo->num_chaves = 1;
			strncpy(novo->chave[0].pk, chave_promovida, TAM_PRIMARY_KEY);
			novo->chave[0].rrn = nregistros;

			novo->desc[0] = iprimary->raiz;
			novo->desc[1] = filho_direito;

			write_btree_ip(novo, nregistrosip);
			free(novo);
			iprimary->raiz = nregistrosip;
			nregistrosip++;
		}
	}
}

void write_btree_ip(node_Btree_ip *salvar, int rrn) {
	char temp[tamanho_registro_ip];

	// Desalocação de temp só por segurança
	for(int i = 0 ; i < tamanho_registro_ip ; i++)
		temp[i] = '\0';

	char aux[14];
	sprintf(aux, "%03d", salvar->num_chaves);
	aux[3] = '\0';
	strcat(temp, aux);

	for(int i = 0 ; i < (ordem_ip - 1) ; i++) {
		if(i < salvar->num_chaves) {
			strcat(temp, salvar->chave[i].pk);

			sprintf(aux, "%04d", salvar->chave[i].rrn);
			aux[4] = '\0';
			strcat(temp, aux);
		} else {
			strcat(temp, "##############");
		}
	}

	temp[3+(14*(ordem_ip - 1))] = salvar->folha;

	for (int i = 0; i < ordem_ip ; i++) {
		if(salvar->desc[i] == -1) {
			strcat(temp, "***");
		} else {
			sprintf(aux, "%03d", salvar->desc[i]);
			aux[3] = '\0';
			strcat(temp, aux);
		}
	}

	temp[tamanho_registro_ip] = '\0';
	strncpy(ARQUIVO_IP + (rrn)*tamanho_registro_ip, temp, tamanho_registro_ip);

	// Desalocação de temp só por segurança
	for(int i = 0 ; i < tamanho_registro_ip ; i++)
		temp[i] = '\0';
}

void write_btree_is(node_Btree_is *salvar, int rrn) {
	return;
}

node_Btree_ip *read_btree_ip(int rrn) {
	if(rrn == -1)
		return NULL;

	char temp[tamanho_registro_ip], aux[14];
	strncpy(temp, ARQUIVO_IP + (rrn)*tamanho_registro_ip, tamanho_registro_ip);
	temp[tamanho_registro_ip] = '\0';

	node_Btree_ip *no_ip = criar_no_ip();

	strncpy(aux, temp, 3);
	aux[3] = '\0';
	no_ip->num_chaves = atoi(aux);

	for(int i = 0 ; i < no_ip->num_chaves ; i++) {
		strncpy(no_ip->chave[i].pk, temp+3+(14*i), TAM_PRIMARY_KEY - 1);
		no_ip->chave[i].pk[TAM_PRIMARY_KEY] = '\0';

		strncpy(aux, temp+13+(14*i), 4);
		aux[4] = '\0';
		no_ip->chave[i].rrn = atoi(aux);
	}

	no_ip->folha = temp[3+(14*(ordem_ip - 1))];

	for(int i = 0 ; i < ordem_ip ; i++) {
		strncpy(aux, temp+3+(14*(ordem_ip - 1))+(i*3) + 1, 3);
		aux[3] = '\0';
		if(strcmp(aux, "***") == 0) {
			no_ip->desc[i] = -1;
		} else {
			no_ip->desc[i] = atoi(aux);
		}
	}

	return no_ip;
}

node_Btree_is *read_btree_is(int rrn) {
	return NULL;
}

node_Btree_ip *busca_no_ip(node_Btree_ip *no_atual, char * k) {
	int i = 0;
	while(i <= no_atual->num_chaves && strcmp(k, no_atual->chave[i].pk) == 1)
		i++;

	if(i <= no_atual->num_chaves && strcmp(k, no_atual->chave[i].pk) == 0)
		return no_atual;

	if(no_atual->folha == 'F')
		return NULL;
	else
		return busca_no_ip(read_btree_ip(no_atual->desc[i]), k);
}

node_Btree_is *busca_no_is(node_Btree_is *no_atual, char * k) {
	return NULL;
}

/********************FUNÇÕES DO MENU*********************/
void cadastrar(Indice* iprimary, Indice* ibrand) {
	char registro[TAM_NOME];
	Produto produto_aux;

	ler_entrada(registro, &produto_aux);
	inserir_arquivo(&produto_aux);

	inserir_registro_indices(iprimary, ibrand, produto_aux);
	nregistros++;
}

int alterar(Indice iprimary) {
	 return 0;
}

void buscar(Indice iprimary,Indice ibrand) {
	 return;
}

void listar(Indice iprimary,Indice ibrand) {
	 return;
}
/********************************************************/


/*************FUNÇÕES REAPROVEITADAS DO T01**************/

void gerarChave(Produto *p) {
	p->pk[0] = toupper(p->nome[0]);
	p->pk[1] = toupper(p->nome[1]);
	p->pk[2] = toupper(p->marca[0]);
	p->pk[3] = toupper(p->marca[1]);
	p->pk[4] = p->data[0];
	p->pk[5] = p->data[1];
	p->pk[6] = p->data[3];
	p->pk[7] = p->data[4];
	p->pk[8] = p->ano[0];
	p->pk[9] = p->ano[1];
	p->pk[10] = '\0';
}

/* Realiza os scanfs na struct Produto */
void ler_entrada(char* registro, Produto *novo) {
 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->nome, registro);

 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->marca, registro);

 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->data, registro);

 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->ano, registro);

 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->preco, registro);

 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->desconto, registro);

 	scanf("%[^\n]%*c", registro);
 	strcpy(novo->categoria, registro);

 	gerarChave(novo);
}

/* Insere produto novo no arquivo de dados */
void inserir_arquivo(Produto *p) {
	char temp[TAM_REGISTRO + 1];

	sprintf(temp, "%s@%s@%s@%s@%s@%s@%s@",	p->nome, p->marca,
											p->data, p->ano,
											p->preco, p->desconto,
											p->categoria);

	temp[192] = '\0';

	int size = strlen(temp);
	size =  192 - size;
	for( int i = 0 ; i < size ; i++ )
		strcat(temp, "#");

	strcat(ARQUIVO, temp);
}

/********************************************************/

void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto j) {
	insere_btree_ip(iprimary, j.pk);
	nregistrosip++;
}

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

Produto recuperar_registro(int rrn)
{
	char temp[193], *p;
	strncpy(temp, ARQUIVO + ((rrn)*192), 192);
	temp[192] = '\0';
	Produto j;
	p = strtok(temp,"@");
	strcpy(j.nome,p);
	p = strtok(NULL,"@");
	strcpy(j.marca,p);
	p = strtok(NULL,"@");
	strcpy(j.data,p);
	p = strtok(NULL,"@");
	strcpy(j.ano,p);
	p = strtok(NULL,"@");
	strcpy(j.preco,p);
	p = strtok(NULL,"@");
	strcpy(j.desconto,p);
	p = strtok(NULL,"@");
	strcpy(j.categoria,p);
	gerarChave(&j);
	return j;
}

/* Exibe o Produto */
int exibir_registro(int rrn)
{
	if(rrn < 0)
		return 0;

	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
    char *cat, categorias[TAM_CATEGORIA];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	printf("%s\n", j.ano);
	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	strcpy(categorias, j.categoria);

	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	printf("\n");

	return 1;
}
