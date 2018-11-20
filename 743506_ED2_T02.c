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

/* Estruturas auxiliares para inserção em arvore-B */
typedef struct
{
	Chave_ip chave_promovida;	// chave promovida em caso de divisão de no
	int rrn_novo_no;			// rrn do novo nó alocado
} Btree_ip_insert_return;

typedef struct
{
	Chave_is chave_promovida;	// chave promovida em caso de divisão de no
	int rrn_novo_no;			// rrn do novo nó alocado
} Btree_is_insert_return;

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

/*
*	Neste presente trabalho, foram feitas funções para atender cada tipo de chave
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

/* Funções de inserção em arvore-B para índice primario */
Btree_ip_insert_return *divide_no_ip(int rrn_X, int rrn_filho_direito, Chave_ip chave_nova);
Btree_ip_insert_return *insere_aux_btree_ip(int rrn, Chave_ip chave_nova);
void insere_btree_ip(Indice *iprimary, Chave_ip chave_nova);

/* Funções de inserção em arvore-B para índice secundário */
Btree_is_insert_return *divide_no_is(int rrn_X, int rrn_filho_direito, Chave_is chave_nova);
Btree_is_insert_return *insere_aux_btree_is(int rrn, Chave_is chave_nova);
void insere_btree_is(Indice *ibrand, Chave_is chave_nova);

/* Funções de busca em arvore-B, o último parametro "print" é um booleano para ativar os prints
	dos nos do caminho percorrido */
int busca_no_ip(int rrn, char * k, int print);
int busca_no_is(Indice iprimary, int rrn, char * k, int print);

/* Percurso pré-ordem em uma arvore-B que faz os prints dos nós percorridos*/
void pre_order_ip(int rrn, int nivel);

/* Percurso em-ordem em uma arvore-B que faz os prints dos nós percorridos*/
void in_order_is(int rrn);

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no_ip(node_Btree_ip *no);
void libera_no_is(node_Btree_is *no);

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
		 return;
	 }

	 iprimary->raiz = -1;

	 int nregistrostotal = nregistros;

	 nregistros = 0;

	 /* Vai inserindo cada registro do arquivo na árvore-B */
	 for(int i = 0 ; i < nregistrostotal ; i++) {
		Produto p = recuperar_registro(i);

		Chave_ip ip;

	 	ip.rrn = nregistros;
	 	strncpy(ip.pk, p.pk, TAM_PRIMARY_KEY);

	 	insere_btree_ip(iprimary, ip);

		nregistros++;
	 }
}

/* (Re)faz o índice de produtos  */
void criar_ibrand(Indice *ibrand) {
	 if(!nregistros) {
	 	ibrand->raiz = -1;
		return;
	}

	ibrand->raiz = -1;

	/* Vai inserindo cada registro do arquivo na árvore-B */
	for(int i = 0 ; i < nregistros ; i++) {
		Produto p = recuperar_registro(i);

		/* Prepara chave secundária para inserção em árvore-B */
		Chave_is is;
		strncpy(is.pk, p.pk, TAM_PRIMARY_KEY);
		char temp[TAM_STRING_INDICE];
		temp[0] = '\0';

		strcat(temp, p.marca);
		strcat(temp, "$");
		strcat(temp, p.nome);
		temp[strlen(p.marca) + strlen(p.nome) + 1] = '\0';
		strncpy(is.string, temp, TAM_STRING_INDICE);

		insere_btree_is(ibrand, is);
	}
}

node_Btree_ip *criar_no_ip() {
	node_Btree_ip *no_ip = (node_Btree_ip *) calloc (1, sizeof(node_Btree_ip));
	no_ip->num_chaves = 0;
	no_ip->chave = calloc ((ordem_ip - 1), sizeof(Chave_ip));

	no_ip->desc = calloc ((ordem_ip), sizeof(int));
	for (int i = 0; i < ordem_ip; i++) {
		no_ip->desc[i] = -1;
	}

	no_ip->folha = 'N';
	return no_ip;
}

node_Btree_is *criar_no_is() {
	node_Btree_is *no_is = (node_Btree_is *) malloc (sizeof(node_Btree_is));
	no_is->num_chaves = 0;
	no_is->chave = calloc ((ordem_is - 1), sizeof(Chave_is));

	no_is->desc = calloc ((ordem_is), sizeof(int));
	for (int i = 0; i < ordem_is; i++) {
		no_is->desc[i] = -1;
	}

	no_is->folha = 'N';
	return no_is;
}

void libera_no_ip(node_Btree_ip *no) {
	free(no->chave);
	free(no->desc);
	free(no);
}

void libera_no_is(node_Btree_is *no) {
	free(no->chave);
	free(no->desc);
	free(no);
}

Btree_ip_insert_return *divide_no_ip(int rrn_X, int rrn_filho_direito, Chave_ip chave_nova) {
	node_Btree_ip *X = read_btree_ip(rrn_X);
	int i = X->num_chaves - 1;
	int chave_alocada = 0;

	/* Aloca novo nó */
	node_Btree_ip *Y = criar_no_ip();
	Y->folha = X->folha;
	Y->num_chaves = floor((ordem_ip - 1)/2);

	/* Coloca metade (arredondado para baixo) das chaves em X para Y,
		colocando chave_nova em Y */
	for(int j = Y->num_chaves - 1 ; j >= 0 ; j--) {
		if(!chave_alocada && (strcmp(chave_nova.pk, X->chave[i].pk) > 0)) {
			strncpy(Y->chave[j].pk, chave_nova.pk, TAM_PRIMARY_KEY);
			Y->chave[j].rrn = chave_nova.rrn;
			Y->desc[j + 1] = rrn_filho_direito;
			chave_alocada = 1;
		} else {
			strncpy(Y->chave[j].pk, X->chave[i].pk, TAM_PRIMARY_KEY);
			Y->chave[j].rrn = X->chave[i].rrn;
			Y->desc[j + 1] = X->desc[i + 1];
			i--;
		}
	}

	/* Se chave_nova não puder ir ao nó Y, mover todas as chaves em X para Y até
	 	colocar a chave_nova em X*/
	if(!chave_alocada) {
		while(i >= 0 && (strcmp(chave_nova.pk, X->chave[i].pk) < 0)) {
			strncpy(X->chave[i + 1].pk, X->chave[i].pk, TAM_PRIMARY_KEY);
			X->chave[i + 1].rrn = X->chave[i].rrn;
			X->desc[i + 2] = X->desc[i + 1];
			i--;
		}
		strncpy(X->chave[i + 1].pk, chave_nova.pk, TAM_PRIMARY_KEY);
		X->chave[i + 1].rrn = chave_nova.rrn;
		X->desc[i + 2] = rrn_filho_direito;
	}

	/* Prepara chave para promoção */
	Chave_ip chave_promovida;
	int aux = floor(ordem_ip/2);
	strncpy(chave_promovida.pk, X->chave[aux].pk, TAM_PRIMARY_KEY);
	chave_promovida.rrn = X->chave[aux].rrn;
	Y->desc[0] = X->desc[aux + 1];
	X->num_chaves = aux;

	/* Salva os nós X e Y */
	write_btree_ip(Y, nregistrosip);
	write_btree_ip(X, rrn_X);

	/* Prepara struct de retorno contendo dados da chave_promovida e o rrn do nó Y */
	Btree_ip_insert_return *ret = calloc(1, sizeof(Btree_ip_insert_return));
	strncpy(ret->chave_promovida.pk, chave_promovida.pk, TAM_PRIMARY_KEY);
	ret->chave_promovida.rrn = chave_promovida.rrn;
	ret->rrn_novo_no = nregistrosip;
	nregistrosip++;

	libera_no_ip(X);
	libera_no_ip(Y);

	return ret;
}

Btree_ip_insert_return *insere_aux_btree_ip(int rrn, Chave_ip chave_nova) {
	node_Btree_ip *X = read_btree_ip(rrn);
	/* Se for folha */
	if(X->folha == 'F') {
		/* E tiver espaço */
		if(X->num_chaves < (ordem_ip - 1)) {
			int i = X->num_chaves;
			while(i > 0  && (strcmp(chave_nova.pk, X->chave[i - 1].pk) < 0)) {
				strncpy(X->chave[i].pk, X->chave[i - 1].pk, TAM_PRIMARY_KEY);
				X->chave[i].rrn = X->chave[i - 1].rrn;
				i--;
			}
			strncpy(X->chave[i].pk, chave_nova.pk, TAM_PRIMARY_KEY);
			X->chave[i].rrn = chave_nova.rrn;
			X->num_chaves++;
			write_btree_ip(X, rrn);
			libera_no_ip(X);
			return NULL;
		} else {
			/* Se não, divide o X */
			libera_no_ip(X);
			return divide_no_ip(rrn, -1, chave_nova);
		}
	} else {
		/* Se não procura folha ideal para chave_nova */
		int i = X->num_chaves - 1;
		while(i >= 0 && (strcmp(chave_nova.pk, X->chave[i].pk) < 0)) {
			i--;
		}
		i++;

		Btree_ip_insert_return *ret = insere_aux_btree_ip(X->desc[i], chave_nova);

		/* Se há chave promovida, insere ela em X */
		if(ret != NULL) {
			strncpy(chave_nova.pk, ret->chave_promovida.pk, TAM_PRIMARY_KEY);
			chave_nova.rrn = ret->chave_promovida.rrn;
			int rrn_filho_direito = ret->rrn_novo_no;

			if( X->num_chaves < (ordem_ip - 1)) {
				int i = X->num_chaves - 1;
				while(i >= 0 && (strcmp(chave_nova.pk, X->chave[i].pk) < 0)) {
					strncpy(X->chave[i + 1].pk, X->chave[i].pk, TAM_PRIMARY_KEY);
					X->chave[i + 1].rrn = X->chave[i].rrn;
					X->desc[i + 2] = X->desc[i + 1];
					i--;
				}
				strncpy(X->chave[i + 1].pk, chave_nova.pk, TAM_PRIMARY_KEY);
				X->chave[i + 1].rrn = ret->chave_promovida.rrn;
				X->desc[i + 2] = rrn_filho_direito;
				X->num_chaves++;

				write_btree_ip(X, rrn);

				libera_no_ip(X);
				free(ret);
				return NULL;
			} else {
				/* Se não couber em X, divide ele */
				libera_no_ip(X);
				free(ret);
				return divide_no_ip(rrn, rrn_filho_direito, chave_nova);
			}
		} else { /* Se há chave promovida, prossiga */
			libera_no_ip(X);
			free(ret);
			return NULL;
		}
	}
}

void insere_btree_ip(Indice *iprimary, Chave_ip chave_nova) {
	/* Leitura do nó raiz da árvore-B */
	node_Btree_ip *raiz = read_btree_ip(iprimary->raiz);

	/* Se raiz for nula, insira novo nó na raiz */
	if(raiz == NULL) {
		node_Btree_ip *novo = criar_no_ip();
		novo->folha = 'F';
		novo->num_chaves = 1;
		strncpy(novo->chave[0].pk, chave_nova.pk, TAM_PRIMARY_KEY);
		novo->chave[0].pk[TAM_PRIMARY_KEY] = '\0';
		novo->chave[0].rrn = chave_nova.rrn;

		iprimary->raiz = nregistrosip;
		nregistrosip++;
		write_btree_ip(novo, 0);
		libera_no_ip(novo);
	} else {
		libera_no_ip(raiz);

		/* Procura por folha ideal para a chave_nova na árvore-B */
		Btree_ip_insert_return *ret = insere_aux_btree_ip(iprimary->raiz, chave_nova);

		/* Se há chave promovida */
		if(ret != NULL) {
			/* Ela será a nova raiz*/
			int filho_direito = ret->rrn_novo_no;
			char *chave_promovida = ret->chave_promovida.pk;
			int rrn = ret->chave_promovida.rrn;

			node_Btree_ip *novo = criar_no_ip();
			novo->folha = 'N';
			novo->num_chaves = 1;
			strncpy(novo->chave[0].pk, chave_promovida, TAM_PRIMARY_KEY);
			novo->chave[0].rrn = rrn;

			novo->desc[0] = iprimary->raiz;
			novo->desc[1] = filho_direito;

			write_btree_ip(novo, nregistrosip);
			libera_no_ip(novo);
			iprimary->raiz = nregistrosip;
			nregistrosip++;
		}
		free(ret);
	}
}

Btree_is_insert_return *divide_no_is(int rrn_X, int rrn_filho_direito, Chave_is chave_nova) {
	node_Btree_is *X = read_btree_is(rrn_X);
	int i = X->num_chaves - 1;
	int chave_alocada = 0;

	/* Aloca novo nó */
	node_Btree_is *Y = criar_no_is();
	Y->folha = X->folha;
	Y->num_chaves = floor((ordem_is - 1)/2);

	/* Coloca metade (arredondado para baixo) das chaves em X para Y,
		colocando chave_nova em Y */
	for(int j = Y->num_chaves - 1 ; j >= 0 ; j--) {
		if(!chave_alocada && (strcmp(chave_nova.string, X->chave[i].string) > 0)) {
			strncpy(Y->chave[j].pk, chave_nova.pk, TAM_PRIMARY_KEY);
			strncpy(Y->chave[j].string, chave_nova.string, TAM_STRING_INDICE);
			Y->desc[j + 1] = rrn_filho_direito;
			chave_alocada = 1;
		} else {
			strncpy(Y->chave[j].pk, X->chave[i].pk, TAM_PRIMARY_KEY);
			strncpy(Y->chave[j].string, X->chave[i].string, TAM_STRING_INDICE);
			Y->desc[j + 1] = X->desc[i + 1];
			i--;
		}
	}

	/* Se chave_nova não puder ir ao nó Y, mover todas as chaves em X para Y até
	 	colocar a chave_nova em X*/
	if(!chave_alocada) {
		while(i >= 0 && (strcmp(chave_nova.string, X->chave[i].string) < 0)) {
			strncpy(X->chave[i + 1].pk, X->chave[i].pk, TAM_PRIMARY_KEY);
			strncpy(X->chave[i + 1].string, X->chave[i].string, TAM_STRING_INDICE);
			X->desc[i + 2] = X->desc[i + 1];
			i--;
		}
		strncpy(X->chave[i + 1].pk, chave_nova.pk, TAM_PRIMARY_KEY);
		strncpy(X->chave[i + 1].string, chave_nova.string, TAM_STRING_INDICE);
		X->desc[i + 2] = rrn_filho_direito;
	}

	/* Prepara chave para promoção */
	Chave_is chave_promovida;
	int aux = floor(ordem_is/2);
	strncpy(chave_promovida.pk, X->chave[aux].pk, TAM_PRIMARY_KEY);
	strncpy(chave_promovida.string, X->chave[aux].string, TAM_STRING_INDICE);
	Y->desc[0] = X->desc[aux + 1];
	X->num_chaves = aux;

	/* Salva os nós X e Y */
	write_btree_is(Y, nregistrosis);
	write_btree_is(X, rrn_X);

	/* Prepara struct de retorno contendo dados da chave_promovida e o rrn do nó Y */
	Btree_is_insert_return *ret = calloc(1, sizeof(Btree_is_insert_return));
	strncpy(ret->chave_promovida.pk, chave_promovida.pk, TAM_PRIMARY_KEY);
	strncpy(ret->chave_promovida.string, chave_promovida.string, TAM_STRING_INDICE);
	ret->rrn_novo_no = nregistrosis;
	nregistrosis++;

	libera_no_is(X);
	libera_no_is(Y);

	return ret;
}

Btree_is_insert_return *insere_aux_btree_is(int rrn, Chave_is chave_nova) {
	node_Btree_is *X = read_btree_is(rrn);
	/* Se for folha */
	if(X->folha == 'F') {
		/* E tiver espaço */
		if(X->num_chaves < (ordem_is - 1)) {
			int i = X->num_chaves;
			while(i > 0  && (strcmp(chave_nova.string, X->chave[i - 1].string) < 0)) {
				strncpy(X->chave[i].pk, X->chave[i - 1].pk, TAM_PRIMARY_KEY);
				strncpy(X->chave[i].string, X->chave[i - 1].string, TAM_STRING_INDICE);
				i--;
			}
			strncpy(X->chave[i].pk, chave_nova.pk, TAM_PRIMARY_KEY);
			strncpy(X->chave[i].string, chave_nova.string, TAM_STRING_INDICE);
			X->num_chaves++;
			write_btree_is(X, rrn);
			libera_no_is(X);
			return NULL;
		} else {
			/* Se não, divide o X */
			libera_no_is(X);
			return divide_no_is(rrn, -1, chave_nova);
		}
	} else {
		/* Se não, procura folha ideal para chave_nova */
		int i = X->num_chaves - 1;
		while(i >= 0 && (strcmp(chave_nova.string, X->chave[i].string) < 0)) {
			i--;
		}
		i++;

		Btree_is_insert_return *ret = insere_aux_btree_is(X->desc[i], chave_nova);

		/* Se há chave promovida, insere ela em X */
		if(ret != NULL) {
			strncpy(chave_nova.pk, ret->chave_promovida.pk, TAM_PRIMARY_KEY);
			strncpy(chave_nova.string, ret->chave_promovida.string, TAM_STRING_INDICE);
			int rrn_filho_direito = ret->rrn_novo_no;

			if( X->num_chaves < (ordem_is - 1)) {
				int i = X->num_chaves - 1;
				while(i >= 0 && (strcmp(chave_nova.string, X->chave[i].string) < 0)) {
					strncpy(X->chave[i + 1].pk, X->chave[i].pk, TAM_PRIMARY_KEY);
					strncpy(X->chave[i + 1].string, X->chave[i].string, TAM_STRING_INDICE);
					X->desc[i + 2] = X->desc[i + 1];
					i--;
				}
				strncpy(X->chave[i + 1].pk, chave_nova.pk, TAM_PRIMARY_KEY);
				strncpy(X->chave[i + 1].string, ret->chave_promovida.string, TAM_STRING_INDICE);
				X->desc[i + 2] = rrn_filho_direito;
				X->num_chaves++;

				write_btree_is(X, rrn);

				libera_no_is(X);
				free(ret);
				return NULL;
			} else { /* Se não couber em X, divide X */
				libera_no_is(X);
				free(ret);
				return divide_no_is(rrn, rrn_filho_direito, chave_nova);
			}
		} else {
			libera_no_is(X);
			free(ret);
			return NULL;
		}
	}
}

void insere_btree_is(Indice *ibrand, Chave_is chave_nova) {
	/* Leitura do nó raiz da árvore-B */
	node_Btree_is *raiz = read_btree_is(ibrand->raiz);

	/* Se raiz for nula, insira novo nó na raiz */
	if(raiz == NULL) {
		node_Btree_is *novo = criar_no_is();
		novo->folha = 'F';
		novo->num_chaves = 1;
		strncpy(novo->chave[0].pk, chave_nova.pk, TAM_PRIMARY_KEY);
		novo->chave[0].pk[TAM_PRIMARY_KEY] = '\0';
		strncpy(novo->chave[0].string, chave_nova.string, TAM_STRING_INDICE);

		ibrand->raiz = nregistrosis;
		nregistrosis++;
		write_btree_is(novo, 0);
		libera_no_is(novo);
	} else {
		libera_no_is(raiz);

		/* Procura por folha ideal para a chave_nova na árvore-B */
		Btree_is_insert_return *ret = insere_aux_btree_is(ibrand->raiz, chave_nova);

		/* Se há chave promovida */
		if(ret != NULL) {
			/* Ela será a nova raiz*/
			int filho_direito = ret->rrn_novo_no;
			char *chave_promovida = ret->chave_promovida.pk;
			char *string = ret->chave_promovida.string;

			node_Btree_is *novo = criar_no_is();
			novo->folha = 'N';
			novo->num_chaves = 1;
			strncpy(novo->chave[0].pk, chave_promovida, TAM_PRIMARY_KEY);
			strncpy(novo->chave[0].string, string, TAM_STRING_INDICE);

			novo->desc[0] = ibrand->raiz;
			novo->desc[1] = filho_direito;

			write_btree_is(novo, nregistrosis);
			libera_no_is(novo);
			ibrand->raiz = nregistrosis;
			nregistrosis++;
		}
		free(ret);
	}
}

void write_btree_ip(node_Btree_ip *salvar, int rrn) {
	/* Prepara temp para ser o registro que será inserido no arquivo de índice */
	char temp[tamanho_registro_ip];

	// Desalocação de temp
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
		} else if(i < (salvar->num_chaves) + 1) {
			sprintf(aux, "%03d", salvar->desc[i]);
			aux[3] = '\0';
			strcat(temp, aux);
		} else {
			strcat(temp, "***");
		}
	}

	temp[tamanho_registro_ip] = '\0';
	strncpy(ARQUIVO_IP + (rrn)*tamanho_registro_ip, temp, tamanho_registro_ip);

	// Desalocação de temp
	for(int i = 0 ; i < tamanho_registro_ip ; i++)
		temp[i] = '\0';
}

void write_btree_is(node_Btree_is *salvar, int rrn) {
	/* Prepara temp para ser o registro que será inserido no arquivo de índice */
	char temp[tamanho_registro_is];

	// Desalocação de temp
	for(int i = 0 ; i < tamanho_registro_is ; i++)
		temp[i] = '\0';

	char aux[14];
	sprintf(aux, "%03d", salvar->num_chaves);
	aux[3] = '\0';
	strcat(temp, aux);

	for(int i = 0 ; i < (ordem_is - 1) ; i++) {
		if(i < salvar->num_chaves) {
			strcat(temp, salvar->chave[i].pk);

			strcat(temp, salvar->chave[i].string);

			int size = strlen(salvar->chave[i].string);
			size = 111 - (size + 10);
			while(size) {
				strcat(temp, "#");
				size--;
			}
		} else {
			int j = 111;
			while(j) {
				strcat(temp, "#");
				j--;
			}
		}
	}

	temp[3+(111*(ordem_is - 1))] = salvar->folha;

	for (int i = 0; i < ordem_is ; i++) {
		if(salvar->desc[i] == -1) {
			strcat(temp, "***");
		} else if(i < (salvar->num_chaves) + 1) {
			sprintf(aux, "%03d", salvar->desc[i]);
			aux[3] = '\0';
			strcat(temp, aux);
		} else {
			strcat(temp, "***");
		}
	}

	temp[tamanho_registro_is] = '\0';
	strncpy(ARQUIVO_IS + (rrn)*tamanho_registro_is, temp, tamanho_registro_is);

	// Desalocação de temp
	for(int i = 0 ; i < tamanho_registro_is ; i++)
		temp[i] = '\0';
}

node_Btree_ip *read_btree_ip(int rrn) {
	if(rrn == -1)
		return NULL;

	/* Leitura do registro no arquivo de índice para salvar em temp */
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
	if(rrn == -1)
		return NULL;

	/* Leitura do registro no arquivo de índice para salvar em temp */
	char temp[tamanho_registro_is], aux[14];
	strncpy(temp, ARQUIVO_IS + (rrn)*tamanho_registro_is, tamanho_registro_is);
	temp[tamanho_registro_is] = '\0';

	node_Btree_is *no_is = criar_no_is();

	strncpy(aux, temp, 3);
	aux[3] = '\0';
	no_is->num_chaves = atoi(aux);

	for(int i = 0 ; i < no_is->num_chaves ; i++) {
		strncpy(no_is->chave[i].pk, temp+3+(111*i), TAM_PRIMARY_KEY - 1);
		no_is->chave[i].pk[TAM_PRIMARY_KEY - 1] = '\0';

		char aux2[TAM_STRING_INDICE], *p;
		strncpy(aux2, temp+13+(111*i), TAM_STRING_INDICE);
		aux2[TAM_STRING_INDICE] = '\0';

		p = strtok(aux2,"#");
		strcpy(no_is->chave[i].string, p);
	}

	no_is->folha = temp[3+(111*(ordem_is - 1))];

	for(int i = 0 ; i < ordem_is ; i++) {
		strncpy(aux, temp+3+(111*(ordem_is - 1))+ 1 + (i*3), 3);
		aux[3] = '\0';
		if(strcmp(aux, "***") == 0) {
			no_is->desc[i] = -1;
		} else {
			no_is->desc[i] = atoi(aux);
		}
	}

	return no_is;
}

int busca_no_ip(int rrn, char * k, int print) {
	if(rrn == -1) {
		return -1;
	}
	int i = 0;
	int first = 1;

	node_Btree_ip * no_atual = read_btree_ip(rrn);

	while(i < no_atual->num_chaves && strcmp(k, no_atual->chave[i].pk) > 0)
		i++;

	if(print) {
		int j = 0;
		while(j < no_atual->num_chaves) {
			if(first) {
				printf("%s", no_atual->chave[j].pk);
				first = 0;
			} else {
				printf(", %s", no_atual->chave[j].pk);
			}
			j++;
		}
	}

	if(i < no_atual->num_chaves && strcmp(k, no_atual->chave[i].pk) == 0) {
		if(print)
			printf("\n");
		int aux = no_atual->chave[i].rrn;
		libera_no_ip(no_atual);
		return aux;
	}

	if(no_atual->folha == 'F') {
		if(print)
			printf("\n");
		libera_no_ip(no_atual);
		return -1;
	} else {
		int aux = no_atual->desc[i];
		libera_no_ip(no_atual);
		if(print)
			printf("\n");
		return busca_no_ip(aux, k, print);
	}
}

void pre_order_ip(int rrn, int nivel) {
	if(rrn == -1)
		return;

	int i = 0;
	int first = 1;

	node_Btree_ip * no_atual = read_btree_ip(rrn);
	printf("%d - ", nivel);

	while(i < no_atual->num_chaves) {
		if(first) {
			printf("%s", no_atual->chave[i].pk);
			first = 0;
		} else {
			printf(", %s", no_atual->chave[i].pk);
		}
		i++;
	}

	i = 0;
	while(i < ordem_ip) {
		if(no_atual->desc[i] != -1) {
			printf("\n");
			pre_order_ip(no_atual->desc[i], nivel + 1);
		}
		i++;
	}

	libera_no_ip(no_atual);
}

int busca_no_is(Indice iprimary, int rrn, char * k, int print) {
	int i = 0;
	int first = 1;

	if(rrn == -1) {
		return -1;
	}

	node_Btree_is * no_atual = read_btree_is(rrn);

	while(i < no_atual->num_chaves && strcmp(k, no_atual->chave[i].string) > 0)
		i++;

	if(print) {
		int j = 0;
		while(j < no_atual->num_chaves) {
			if(first) {
				printf("%s", no_atual->chave[j].string);
				first = 0;
			} else {
				printf(", %s", no_atual->chave[j].string);
			}
			j++;
		}
	}

	if(i <= no_atual->num_chaves && strcmp(k, no_atual->chave[i].string) == 0) {
		if(print)
			printf("\n");
		int aux = busca_no_ip(iprimary.raiz, no_atual->chave[i].pk, 0);
		libera_no_is(no_atual);
		return aux;
	}

	if(no_atual->folha == 'F') {
		if(print)
			printf("\n");
		libera_no_is(no_atual);
		return -1;
	} else {
		int aux = no_atual->desc[i];
		libera_no_is(no_atual);
		if(print)
			printf("\n");
		return busca_no_is(iprimary, aux, k, print);
	}
}

void in_order_is(int rrn) {
	if(rrn == -1)
		return;

	node_Btree_is * no_atual = read_btree_is(rrn);

	int i = 0;

	while(i < no_atual->num_chaves) {
		char temp[101];
		char aux[TAM_STRING_INDICE];
		char marca[TAM_MARCA];
		char nome[TAM_NOME];
		char *p;

		temp[0] = '\0';

		in_order_is(no_atual->desc[i]);

		strncpy(aux, no_atual->chave[i].string, TAM_STRING_INDICE);

		p = strtok(aux,"$");
		strcpy(marca,p);
		p = strtok(NULL,"\0");
		strcpy(nome,p);

		for(int j = 0 ; j < 50 ; j++) {
			if(j < strlen(marca)) {
				temp[j] = marca[j];
			} else {
				temp[j] = '-';
			}
		}

		temp[50] = ' ';

		for(int j = 0 ; j < 50 ; j++) {
			if(j < strlen(nome)) {
				temp[j+51] = nome[j];
			} else {
				temp[j+51] = '-';
			}
		}
		temp[101] = '\0';
		printf("%s\n", temp);

		i++;

		if(i == no_atual->num_chaves) {
			in_order_is(no_atual->desc[i]);
		}
	}

	libera_no_is(no_atual);
}

/********************FUNÇÕES DO MENU*********************/
void cadastrar(Indice* iprimary, Indice* ibrand) {
	char registro[TAM_NOME];
	Produto produto_aux;

	ler_entrada(registro, &produto_aux);
	if(nregistros || iprimary->raiz != -1) {
		/* Busca por chave repetida */
		if(busca_no_ip(iprimary->raiz, produto_aux.pk, 0) != -1) {
			printf(ERRO_PK_REPETIDA, produto_aux.pk);
			return;
		}
	}

	/* Inserção do produto no arquivo de dados */
	inserir_arquivo(&produto_aux);

	/* Inserção do produto nos arquivo de índices */
	inserir_registro_indices(iprimary, ibrand, produto_aux);
	nregistros++;
}

int alterar(Indice iprimary) {
	char pk_alt[TAM_PRIMARY_KEY];

	scanf("%[^\n]%*c", pk_alt);

	int found = busca_no_ip(iprimary.raiz, pk_alt, 0);

	if(found != -1) { /* Se chave foi encontrada */
		/* Código reaproveitado e modificado do T01 */
		char desconto[4];
		int valido = 0;

		while (!valido) {
			scanf("%[^\n]%*c", desconto);

			/* Pre-processando a entrada */
			if(strlen(desconto) == 3) {
				desconto[3] = '\0';

				/* Checando se desconto contem apenas numeros */
				int j = 0;
				int contem_letra = 0;
				while(j < 3) {
					if(isdigit(desconto[j]) == 0) {
						contem_letra = 1;
					}
					j++;
				}
				if(!contem_letra) {
					int d = atoi(desconto);
					if(d <= 100 && d >= 0) {
						char temp[193];
						strncpy(temp, ARQUIVO + ((found)*192), 192);
						temp[192] = '\0';
						int size = strlen(temp);
						int pos = 0;

						/* Procura posicao do campo desconto */
						int arroba = 0;
						for(int i = 0 ; i < size ; i++) {
							if(temp[i] == '@') {
								arroba++;
							}
							if(arroba == 6) {
								pos = i + 1;
								break;
							}
						}

						/* Altera Arquivo de Dados */
						ARQUIVO[((found)*192) + pos] = desconto[0];
						ARQUIVO[((found)*192) + pos+1] = desconto[1];
						ARQUIVO[((found)*192) + pos+2] = desconto[2];
						return 1;
					} else {
						printf(CAMPO_INVALIDO);
					}
				} else {
					printf(CAMPO_INVALIDO);
				}
			} else {
				printf(CAMPO_INVALIDO);
			}
		}
	} else {
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
	printf(REGISTRO_N_ENCONTRADO);
	return 0;
}

void buscar(Indice iprimary,Indice ibrand) {
	int opcao;
	char pk[TAM_PRIMARY_KEY];
	char aux[TAM_STRING_INDICE];
	char busca_aux[TAM_STRING_INDICE];

	aux[0] = '\0';
	busca_aux[0] = '\0';

	scanf("%d%*c", &opcao);

	if(opcao == 1) {
		scanf("%[^\n]%*c", pk);
		printf(NOS_PERCORRIDOS_IP, pk);

		int achou = busca_no_ip(iprimary.raiz, pk, 1);

		printf("\n");

		if(achou != -1){
			exibir_registro(achou);
		}
		else {
			printf(REGISTRO_N_ENCONTRADO);
		}
	} else if(opcao == 2) {
		scanf("%[^\n]%*c", aux);
		strcat(busca_aux, aux);

		strcat(busca_aux, "$");

		scanf("%[^\n]%*c", aux);
		strcat(busca_aux, aux);

		printf(NOS_PERCORRIDOS_IS, busca_aux);

		int achou = busca_no_is(iprimary, ibrand.raiz, busca_aux, 1);

		printf("\n");

		if(achou != -1){
			exibir_registro(achou);
		}
		else {
			printf(REGISTRO_N_ENCONTRADO);
		}
	}
}

void listar(Indice iprimary,Indice ibrand) {
	int opcao;

   	scanf("%d%*c", &opcao);

   	if(opcao == 1) {
		if(!nregistros) {
			printf(REGISTRO_N_ENCONTRADO);
			return;
		}

	   	pre_order_ip(iprimary.raiz, 1);
		printf("\n");
   	} else if(opcao == 2) {
		if(!nregistros) {
			printf(REGISTRO_N_ENCONTRADO);
			return;
		}

		in_order_is(ibrand.raiz);
   	}
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

	sprintf(temp, "%s@%s@%s@%s@%s@%s@%s@%s@", 	p->pk, p->nome,
	 											p->marca, p->data,
												p->ano, p->preco,
												p->desconto, p->categoria);

	temp[192] = '\0';

	int size = strlen(temp);
	size =  192 - size;
	for( int i = 0 ; i < size ; i++ )
		strcat(temp, "#");

	strcat(ARQUIVO, temp);
}

/********************************************************/

void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto j) {
	/* Preparando chave para inserção */
	Chave_ip ip;

	/* Inserção no arquivo de índice primário */
	ip.rrn = nregistros;
	strncpy(ip.pk, j.pk, TAM_PRIMARY_KEY);

	insere_btree_ip(iprimary, ip);

	/* Inserção no arquivo de índice secundário */
	Chave_is is;
	strncpy(is.pk, j.pk, TAM_PRIMARY_KEY);

	char aux[TAM_STRING_INDICE];
	aux[0] = '\0';

	strcat(aux, j.marca);
	strcat(aux, "$");
	strcat(aux, j.nome);
	aux[strlen(j.marca) + strlen(j.nome) + 1] = '\0';
	strncpy(is.string, aux, TAM_STRING_INDICE);

	insere_btree_is(ibrand, is);
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
	strcpy(j.pk,p);
	p = strtok(NULL,"@");
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
