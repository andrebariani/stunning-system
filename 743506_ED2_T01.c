/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
 *
 * RA: 743506
 * Aluno: André Matheus Bariani Trava
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY	11
#define TAM_NOME 		51
#define TAM_MARCA 		51
#define TAM_DATA 		11
#define TAM_ANO 		3
#define TAM_PRECO 		8
#define TAM_DESCONTO 	4
#define TAM_CATEGORIA 	51


#define TAM_REGISTRO 	192
#define MAX_REGISTROS 	1000
#define MAX_CATEGORIAS 	30
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas para o usuario */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!\n"
#define INICIO_BUSCA 		 		"**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM  			"**********************LISTAR**********************\n"
#define INICIO_ALTERACAO 			"**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO  			"**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO  			"**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO	"*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"



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


/*----- Registros dos Índices -----*/

/* Struct para índice Primário */
typedef struct primary_index{
  char pk[TAM_PRIMARY_KEY];
  int rrn;
} Ip;

/* Struct para índice secundário */
typedef struct secundary_index{
  char pk[TAM_PRIMARY_KEY];
  char string[TAM_NOME];
} Is;

/* Struct para índice secundário de preços */
typedef struct secundary_index_of_final_price{
  float price;
  char pk[TAM_PRIMARY_KEY];
} Isf;

/* Lista ligada para o Índice abaixo*/
typedef struct linked_list{
  char pk[TAM_PRIMARY_KEY];
  struct linked_list *prox;
} ll;

/* Struct para lista invertida */
typedef struct reverse_index{
  char cat[TAM_CATEGORIA];
  ll* lista;
} Ir;

/*----- GLOBAL -----*/
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Atualiza o arquivo de dados, sobrescrevendo quaisquer registros removidos
   e atualizando o numero de registros. */
int atualizar_arquivo(int nreg);

/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto);

/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn);

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int* nregistros);

/* Se "type" for 0, cria indice secundario para produto.
   Se "type" for 1, cria indice secundario para marca*/
void criar_isecondary(Ip *indice_primario, Is* isecondary, int* nregistros, int type);

void criar_isf(Ip *indice_primario, Isf* iprice, int* nregistros);

void criar_ireverse(Ip *indice_primario, Ir* icategory, int* nregistros, int* ncat);

/* Funcoes comparativas para ordenacao dos respectivos indices */
/* Comparacao entre indices primarios. */
int comp_ip(const void* a, const void* b);

/* Comparacao entre indices secundarios pela string
   e se string for igual, compara chave primaria. */
int comp_is_by_str(const void* a, const void* b);

/* Comparacao entre indices secundarios pelo preco */
int comp_isf(const void* a, const void* b);

/* Comparacao entre indices reversos pela categoria */
int comp_ir(const void* a, const void* b);

/* Funcao de busca de indice primario */
int srch_ip(const void* a, const void* b);

/* Funcao de busca de indice reverso */
int srch_ir(const void* a, const void* b);

/* Funcao auxiliar para insercao ordenada na lista reversa */
void inserir_lista(ll **lista_ligada, char* pk);

/* Realiza os scanfs na struct Produto */
void ler_entrada(char* registro, Produto *novo);

/* Insere produto novo no arquivo de dados */
void inserir_arquivo(Produto *novo, int nregistros);

/* Insere novo indice primario e reordena os indices respectivos*/
void adicionar_aos_indices(Ip *indice_primario, Is* iproduct, Is* ibrand, Isf *iprice, Ir* icategory, int *nregistros, int* ncat, Produto j);

/* Remove indice primario.
   Retorna 1 se houve sucesso na remocao, ou 0 se houve falha */
int remover(Ip *indice_primario, char* pk, int *nregistros);

/* Altera o desconto aplicado, modificando o indice secundario e
   e o arquivo de dados.
   Retorna 1 se houve sucesso na alteracao, ou 0 se houve falha */
int alterar(Ip *indice_alterar, Isf *iprice, int *nregistros);

/* Busca linear por todos os registros com o mesmo nome.
   Retorna 1 se foi encontrado no minimo 1 registro, ou
   retorna 0 se nao foi encontrado nenhum. */
int buscar_iproduct(char* registro, Ip *indice_primario, Is* isecondary, int *nregistros);

/* Busca linear por todos os registros com a mesma marca e contendo uma categoria.
   Retorna 1 se foi encontrado no minimo 1 registro, ou
   retorna 0 se nao foi encontrado nenhum. */
int buscar_ibrand(char* registro, char* categoria, Ip *indice_primario, Is* isecondary, Ir* icategory, int *nregistros, int *ncat);

/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat);


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main(){
  /* Arquivo */
	int carregarArquivo = 0, nregistros = 0, ncat = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	/* Índice primário */
	Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
  	if (!iprimary) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_iprimary(iprimary, &nregistros);

	/*Alocar e criar índices secundários*/

	// iproduct
	Is *iproduct = malloc (MAX_REGISTROS * sizeof(Is));
  	if (!iproduct) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_isecondary(iprimary, iproduct, &nregistros, 0);

	// ibrand
	Is *ibrand = malloc (MAX_REGISTROS * sizeof(Is));
  	if (!ibrand) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_isecondary(iprimary, ibrand, &nregistros, 1);

	// iprice
	Isf *iprice = malloc (MAX_REGISTROS * sizeof(Isf));
	if (!iprice) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_isf(iprimary, iprice, &nregistros);

	// icategory
	Ir *icategory = malloc (MAX_CATEGORIAS * sizeof(Ir));
  	if (!icategory) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_ireverse(iprimary, icategory, &nregistros, &ncat);

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		Produto produto_aux;
		char registro[TAM_NOME];

		scanf("%d%*c", &opcao);

		switch(opcao)
		{
			case 1:
				ler_entrada(registro, &produto_aux);

				if(nregistros) {
					/* Pesquisa binaria por chave repetida */
					Ip *found;
					found = (Ip *) bsearch(produto_aux.pk, iprimary, (size_t) nregistros, sizeof(Ip), srch_ip);

					if(found == NULL) { /* Se nao for repetida */
						inserir_arquivo(&produto_aux, nregistros);
						adicionar_aos_indices(iprimary, iproduct, ibrand, iprice, icategory, &nregistros, &ncat, produto_aux);
					} else if(found->rrn == -1) { /* Se for repetida, mas foi removida */
						inserir_arquivo(&produto_aux, nregistros);

						int pos = 0;

						/* Busca linear pela posicao da chave repetida no indice primario */
						for(int i = 0 ; i < nregistros ; i++) {
							if(strcmp(iprimary[i].pk, produto_aux.pk) == 0) {
								pos = i;
							}
						}
						iprimary[pos].rrn = nregistros;

						strcpy(iproduct[pos].string, produto_aux.nome);
						qsort(iproduct, (size_t) (nregistros), sizeof(Is), comp_is_by_str);

						for(int i = 0 ; i < nregistros ; i++) {
							if(strcmp(ibrand[i].pk, produto_aux.pk) == 0) {
								strcpy(ibrand[nregistros].string, produto_aux.marca);
								qsort(ibrand, (size_t) (nregistros), sizeof(Is), comp_is_by_str);
							}
						}

						for(int i = 0 ; i < nregistros ; i++) {
							if(strcmp(iprice[i].pk, produto_aux.pk) == 0) {
								float preco;
								preco = atof(produto_aux.preco) / 100;
								float desconto = atoi(produto_aux.desconto);
								preco = preco * (100 - desconto) * 100;
								iprice[i].price = ((int) preco)/ (float) 100;
								qsort(iprice, (size_t) (nregistros), sizeof(Isf), comp_isf);
							}
						}

						char *cat, categorias[TAM_CATEGORIA];

						strncpy(categorias, produto_aux.categoria, TAM_CATEGORIA);

						cat = strtok(categorias, "|");

						while(cat != NULL){

							Ir *found;
							found = (Ir *) bsearch(cat, icategory, (size_t) ncat, sizeof(Ir), srch_ir);

							if(found) {
								inserir_lista(&(found->lista), produto_aux.pk);
							} else {
								int poscat = ncat;
								strcpy(icategory[poscat].cat, cat);
								icategory[poscat].lista = NULL;
								inserir_lista(&icategory[poscat].lista, produto_aux.pk);
								ncat = ncat + 1;
							}

							cat = strtok(NULL, "|");

						}

						qsort(icategory, (size_t) ncat, sizeof(Ir), comp_ir);

						nregistros++;

					} else {
						printf(ERRO_PK_REPETIDA, produto_aux.pk);
					}
				} else {
					inserir_arquivo(&produto_aux, nregistros);
					adicionar_aos_indices(iprimary, iproduct, ibrand, iprice, icategory, &nregistros, &ncat, produto_aux);
				}

			break;
			case 2:
				/*alterar desconto*/
				printf(INICIO_ALTERACAO);

				if(nregistros) {
					char pk_alt[TAM_PRIMARY_KEY];

					scanf("%[^\n]%*c", pk_alt);

					Ip *found;
					found = (Ip *) bsearch(pk_alt, iprimary, (size_t) nregistros, sizeof(Ip), srch_ip);

					if(found) { /* Se chave foi encontrada */
						if(alterar(found, iprice, &nregistros))
							printf(SUCESSO);
						else
							printf(FALHA);
					} else {
						printf(REGISTRO_N_ENCONTRADO);
						printf(FALHA);
					}
				} else {
					printf(ARQUIVO_VAZIO);
				}
			break;
			case 3:
				/*excluir produto*/
				printf(INICIO_EXCLUSAO);

				if(nregistros){

					char pk_rem[TAM_PRIMARY_KEY];

					scanf("%[^\n]%*c", pk_rem);

					if(remover(iprimary, pk_rem, &nregistros))
						printf(SUCESSO);
					else {
						printf(REGISTRO_N_ENCONTRADO);
						printf(FALHA);
					}
				} else {
					printf(ARQUIVO_VAZIO);
				}
			break;
			case 4:
				/*busca*/
				printf(INICIO_BUSCA);

				if(nregistros) {
					scanf("%d%*c", &opcao);

					switch(opcao) {

						/* Chave Primaria */
						case 1:
							if(nregistros) {
								scanf("%[^\n]%*c", registro);

								Ip *found;
								found = (Ip *) bsearch(registro, iprimary, (size_t) nregistros, sizeof(Ip), srch_ip);

								if(found) {
									if(found->rrn != -1)
										exibir_registro(found->rrn, 0);
									else
										printf(REGISTRO_N_ENCONTRADO);
								} else {
									printf(REGISTRO_N_ENCONTRADO);
								}
							} else {
								printf(ARQUIVO_VAZIO);
							}
						break;

						/* Nome */
						case 2:
							if(nregistros) {
								scanf("%[^\n]%*c", registro);

								if(!buscar_iproduct(registro, iprimary, iproduct, &nregistros))
									printf(REGISTRO_N_ENCONTRADO);

							} else {
								printf(ARQUIVO_VAZIO);
							}
						break;
						/* Marca e Categoria */
						case 3:
							if(nregistros) {
								char categoria[TAM_CATEGORIA];

								scanf("%[^\n]%*c", registro);
								scanf("%[^\n]%*c", categoria);

								if(!buscar_ibrand(registro, categoria, iprimary, ibrand, icategory, &nregistros, &ncat))
									printf(REGISTRO_N_ENCONTRADO);

							} else {
								printf(ARQUIVO_VAZIO);
							}
						break;
					}
				} else {
					printf(ARQUIVO_VAZIO);
				}

			break;
			case 5:
				/*listagens*/
				printf(INICIO_LISTAGEM);

				if(nregistros) {
					scanf("%d%*c", &opcao);

					switch(opcao) {
						/* Codigo */
						case 1:
							if(nregistros) {
								int next = 0;
								for( int i = 0 ; i < nregistros ; i++) {

									if(iprimary[i].rrn != -1) {
										if(next) {
											printf("\n");
										}
										exibir_registro(iprimary[i].rrn, 0);
										next = 1;
									}
								}
								if(!next)
									printf(REGISTRO_N_ENCONTRADO);
							} else {
								printf(REGISTRO_N_ENCONTRADO);
							}
						break;

						/* Categoria */
						case 2:
							if(nregistros) {
								scanf("%[^\n]%*c", registro);

								Ir *found;
								found = (Ir *) bsearch(registro, icategory, (size_t) ncat, sizeof(Ir), srch_ir);

								if(found) {
									ll *aux = found->lista;

									int next = 0;
									while(aux != NULL) {

										Ip *found2;
										found2 = (Ip *) bsearch(aux->pk, iprimary, (size_t) nregistros, sizeof(Ip), srch_ip);

										if(found2) {
											if(found2->rrn != -1) {
												if(next) {
													printf("\n");
												}
												exibir_registro(found2->rrn, 0);
												next = 1;
											}
										}
										aux = aux->prox;
									}
									if(!next)
										printf(REGISTRO_N_ENCONTRADO);
								} else {
									printf(REGISTRO_N_ENCONTRADO);
								}

							} else {
								printf(ARQUIVO_VAZIO);
							}
						break;
						case 3:

						/* Marca */
							if(nregistros) {
								int next = 0;
								for( int i = 0 ; i < nregistros ; i++) {

									Ip *found;
									found = (Ip *) bsearch(ibrand[i].pk, iprimary, (size_t) nregistros, sizeof(Ip), srch_ip);

									if(found) {
										if(found->rrn != -1) {
											if(next) {
												printf("\n");
											}
											exibir_registro(found->rrn, 0);
											next = 1;
										}
									}
								}
								if(!next)
									printf(REGISTRO_N_ENCONTRADO);
							} else {
								printf(REGISTRO_N_ENCONTRADO);
							}
						break;
						case 4:

						/* Preco */
							if(nregistros) {
								int next = 0;
								for( int i = 0 ; i < nregistros ; i++) {

									Ip *found;
									found = (Ip *) bsearch(iprice[i].pk, iprimary, (size_t) nregistros, sizeof(Ip), srch_ip);

									if(found) {
										if(found->rrn != -1) {
											if(next) {
												printf("\n");
											}
											exibir_registro(found->rrn, 1);
											next = 1;
										}
									}
								}
								if(!next)
									printf(REGISTRO_N_ENCONTRADO);
							} else {
								printf(REGISTRO_N_ENCONTRADO);
							}
						break;
					}
				} else {
					printf(ARQUIVO_VAZIO);
				}

			break;
			case 6:
				/*libera espaço*/
				free(iprimary);
				free(iproduct);
				free(ibrand);
				free(iprice);

				for(int i = 0 ; i < ncat ; i++) {
					ll *aux = icategory[i].lista;
					while(aux != NULL) {
						ll *aux2 = aux;
						free(aux);
						aux = aux2->prox;
					}
				}

				free(icategory);

				ncat = 0;

				nregistros = atualizar_arquivo(nregistros);

				iprimary = malloc (MAX_REGISTROS * sizeof(Ip));
			  	if (!iprimary) {
					perror(MEMORIA_INSUFICIENTE);
					exit(1);
				}
				criar_iprimary(iprimary, &nregistros);

				iproduct = malloc (MAX_REGISTROS * sizeof(Is));
			  	if (!iproduct) {
					perror(MEMORIA_INSUFICIENTE);
					exit(1);
				}
				criar_isecondary(iprimary, iproduct, &nregistros, 0);

				ibrand = malloc (MAX_REGISTROS * sizeof(Is));
			  	if (!ibrand) {
					perror(MEMORIA_INSUFICIENTE);
					exit(1);
				}
				criar_isecondary(iprimary, ibrand, &nregistros, 1);

				iprice = malloc (MAX_REGISTROS * sizeof(Isf));
				if (!iprice) {
					perror(MEMORIA_INSUFICIENTE);
					exit(1);
				}
				criar_isf(iprimary, iprice, &nregistros);

				icategory = malloc (MAX_CATEGORIAS * sizeof(Ir));
			  	if (!icategory) {
					perror(MEMORIA_INSUFICIENTE);
					exit(1);
				}
				criar_ireverse(iprimary, icategory, &nregistros, &ncat);

			break;
			case 7:
				/*imprime o arquivo de dados*/
				printf(INICIO_ARQUIVO);
				if(nregistros)
					printf("%s\n", ARQUIVO);
				else
					printf(ARQUIVO_VAZIO);
			break;
			case 8:
				/*imprime os índices secundários*/
				if(nregistros)
					imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);
				else
					printf(ARQUIVO_VAZIO);
			break;
			case 9:
	      		/*Liberar memória e finalizar o programa */

				free(iprimary);
				free(iproduct);
				free(ibrand);
				free(iprice);

				for(int i = 0 ; i < ncat ; i++) {
					ll *aux = icategory[i].lista;
					while(aux != NULL) {
						ll *aux2 = aux;
						free(aux);
						aux = aux2->prox;
					}
				}

				free(icategory);

				return 0;
			break;
			default:
				printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}

/* Gera campo Código do produto de entrada */
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

/* Comparacao de strings */
int comp_ip(const void* a, const void* b) {
	Ip *pa = (Ip *)a;
	Ip *pb = (Ip *)b;
	return strcmp(pa->pk, pb->pk);

}

int srch_ip(const void* a, const void* b) {
	char *pa = (char *)a;
	Ip *pb = (Ip *)b;

	return strcmp(pa, pb->pk);
}

int comp_is_by_str(const void* a, const void* b) {
	Is *pa = (Is *)a;
	Is *pb = (Is *)b;
	int comp = strcmp(pa->string, pb->string);

	if(comp == 0) {
		return strcmp(pa->pk, pb->pk);
	} else {
		return comp;
	}
}

int comp_isf(const void* a, const void* b) {
	Isf *pa = (Isf *)a;
	Isf *pb = (Isf *)b;
	int comp = (int)(pa->price*100 - pb->price*100);
	if(comp == 0) {
		return strcmp(pa->pk, pb->pk);
	}
	return comp;
}

int comp_ir(const void* a, const void* b) {
	Ir *pa = (Ir *)a;
	Ir *pb = (Ir *)b;
	return strcmp(pa->cat, pb->cat);
}

int srch_ir(const void* a, const void* b) {
	char *pa = (char *)a;
	Ir *pb = (Ir *)b;
	return strcmp(pa, pb->cat);
}

/* Cria índice primário */
void criar_iprimary(Ip *indice_primario, int* nregistros) {
	if(*nregistros == 0)
		return;

	for (int i = 0; i < *nregistros; i++) {
		Produto j = recuperar_registro(i);

		strcpy(indice_primario[i].pk, j.pk);
		indice_primario[i].rrn = i;
	}

	qsort(indice_primario, (size_t) *nregistros, sizeof(Ip), comp_ip);

	return;
}


void criar_isecondary(Ip *indice_primario, Is* isecondary, int* nregistros, int type) {
	if(*nregistros == 0)
		return;

	for (int i = 0; i < *nregistros; i++) {
		Produto j = recuperar_registro(indice_primario[i].rrn);

		strcpy(isecondary[i].pk, j.pk);
		type ? strcpy(isecondary[i].string, j.marca) : strcpy(isecondary[i].string, j.nome);
	}

	qsort(isecondary, (size_t) *nregistros, sizeof(Is), comp_is_by_str);

}

void criar_isf(Ip *indice_primario, Isf* iprice, int* nregistros) {
	if(*nregistros == 0)
		return;

	for (int i = 0; i < *nregistros; i++) {
		Produto j = recuperar_registro(indice_primario[i].rrn);

		strcpy(iprice[i].pk, j.pk);

		float preco;
		preco = atof(j.preco) / 100;
		float desconto = atoi(j.desconto);
		preco = preco * (100 - desconto) * 100;

		iprice[i].price = ((int) preco)/ (float) 100;
	}

	qsort(iprice, (size_t) *nregistros, sizeof(Isf), comp_isf);
}

void inserir_lista(ll **lista_ligada, char* pk) {
	if(*lista_ligada == NULL) {
		ll *novo_beg = malloc (sizeof(ll));
		*lista_ligada = novo_beg;
		strcpy(novo_beg->pk, pk);
		novo_beg->prox = NULL;
	} else {
		ll *aux = *lista_ligada;

		if(strcmp(pk, aux->pk) < 0) {
			ll *novo_ini = malloc (sizeof(ll));
			strcpy(novo_ini->pk, pk);
			novo_ini->prox = aux;
			*lista_ligada = novo_ini;
			return;
		}

		while(aux->prox != NULL && strcmp(aux->prox->pk, pk) < 0)
			aux = aux->prox;

		if(strcmp(pk, aux->pk) == 0)
			return;

		ll *novo = malloc (sizeof(ll));
		strcpy(novo->pk, pk);
		novo->prox = aux->prox;
		aux->prox = novo;
	}
}

void criar_ireverse(Ip *indice_primario, Ir* icategory, int* nregistros, int* ncat) {
	if(*nregistros == 0)
		return;

	char *cat, categorias[TAM_CATEGORIA+1];

	for (int i = 0; i < *nregistros; i++) {

		Produto j = recuperar_registro(indice_primario[i].rrn);

		strncpy(categorias, j.categoria, TAM_CATEGORIA+1);

		cat = strtok (categorias, "|");


		while(cat != NULL){
			int rep = 0;

			Ir *found;
			found = bsearch(cat, icategory, (size_t) *ncat, sizeof(Ir), srch_ir);

			if(found) {
				inserir_lista(&(found->lista), j.pk);
			} else {
				strncpy(icategory[*ncat].cat, cat, TAM_CATEGORIA);
				icategory[*ncat].lista = NULL;
				inserir_lista(&(icategory[*ncat].lista), j.pk);
				*ncat = *ncat + 1;
				qsort(icategory, (size_t) *ncat, sizeof(Ir), comp_ir);
			}

			cat = strtok(NULL, "|");
		}
	}

	qsort(icategory, (size_t) *ncat, sizeof(Ir), comp_ir);
}

void adicionar_aos_indices(Ip *indice_primario, Is* iproduct, Is* ibrand, Isf *iprice, Ir* icategory, int *nregistros, int* ncat, Produto j) {

	strcpy(indice_primario[*nregistros].pk, j.pk);
	indice_primario[*nregistros].rrn = *nregistros;
	qsort(indice_primario, (size_t) (*nregistros + 1), sizeof(Ip), comp_ip);

	strcpy(iproduct[*nregistros].pk, j.pk);
	strcpy(iproduct[*nregistros].string, j.nome);
	qsort(iproduct, (size_t) (*nregistros + 1), sizeof(Is), comp_is_by_str);

	strcpy(ibrand[*nregistros].pk, j.pk);
	strcpy(ibrand[*nregistros].string, j.marca);
	qsort(ibrand, (size_t) (*nregistros + 1), sizeof(Is), comp_is_by_str);

	strcpy(iprice[*nregistros].pk, j.pk);
	float preco;
	preco = atof(j.preco) / 100;
	float desconto = atoi(j.desconto);
	preco = preco * (100 - desconto) * 100;
	iprice[*nregistros].price = ((int) preco)/ (float) 100;
	qsort(iprice, (size_t) (*nregistros + 1), sizeof(Isf), comp_isf);

	char *cat, categorias[TAM_CATEGORIA];

	strncpy(categorias, j.categoria, TAM_CATEGORIA);

	cat = strtok(categorias, "|");

	while(cat != NULL){

		Ir *found;
		found = (Ir *) bsearch(cat, icategory, (size_t) *ncat, sizeof(Ir), srch_ir);

		if(found) {
			inserir_lista(&(found->lista), j.pk);
		} else {
			int pos = *ncat;
			strcpy(icategory[pos].cat, cat);
			icategory[pos].lista = NULL;
			inserir_lista(&icategory[pos].lista, j.pk);
			*ncat = *ncat + 1;
		}

		cat = strtok(NULL, "|");

	}

	qsort(icategory, (size_t) *ncat, sizeof(Ir), comp_ir);

	*nregistros = *nregistros + 1;
}

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

void inserir_arquivo(Produto *p, int nregistros) {
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

int buscar_iproduct(char* registro, Ip* iprimary, Is* isecondary, int *nregistros) {
	int found = 0;
	for(int i = 0 ; i < *nregistros ; i++) {
		if(strcmp(isecondary[i].string, registro) == 0) {

			Ip *pk;
			pk = (Ip *) bsearch(isecondary[i].pk, iprimary, (size_t) *nregistros, sizeof(Ip), srch_ip);

			if(pk != NULL && pk->rrn != -1) {
				if(found)
					printf("\n");
				exibir_registro(pk->rrn, 0);
				found = 1;
			}
		}
	}

	return found;
}

int buscar_ibrand(char* registro, char* categoria, Ip *indice_primario, Is* isecondary, Ir* icategory, int *nregistros, int *ncat) {
	int found = 0;
	for(int i = 0 ; i < *nregistros ; i++) {
		if(strcmp(isecondary[i].string, registro) == 0) {

			Ir *cat;
			cat = (Ir *) bsearch(categoria, icategory, (size_t) *ncat, sizeof(Ir), srch_ir);

			if(cat != NULL) {
				ll* aux = cat->lista;

				while(aux != NULL) {
					if(strcmp(isecondary[i].pk, aux->pk) == 0) {
						Ip *pk;
						pk = (Ip *) bsearch(aux->pk, indice_primario, (size_t) *nregistros, sizeof(Ip), srch_ip);

						if(pk != NULL && pk->rrn != -1) {
							if(found)
								printf("\n");
							exibir_registro(pk->rrn, 0);
							found = 1;
						}
						break;
					}
					aux = aux->prox;
				}
			}
		}
	}

	return found;
}


int remover(Ip *indice_primario, char* pk, int *nregistros) {
	if(nregistros) {
		Ip *found;
		found = (Ip *) bsearch(pk, indice_primario, (size_t) *nregistros, sizeof(Ip), srch_ip);

		if(found != NULL && found->rrn != -1) {
			ARQUIVO[found->rrn*192] = '*';
			ARQUIVO[found->rrn*192 + 1] = '|';

			found->rrn = -1;

			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

int alterar(Ip *indice_alterar, Isf *iprice, int* nregistros) {
	char desconto[4];
	if(indice_alterar->rrn != -1) {

		scanf("%[^\n]%*c", desconto);

		/* Pre-processando a entrada */
		if(strlen(desconto) == 3) {
			int d = atoi(desconto);
			if(d <= 100 && d >= 0) {
				char temp[193];
				strncpy(temp, ARQUIVO + ((indice_alterar->rrn)*192), 192);
				temp[192] = '\0';
				int size = strlen(temp);
				int pos = 0;

				/* Procura posicao do campo desconto */
				int arroba = 0;
				for(int i = 0 ; i < size ; i++) {
					if(temp[i] == '@') {
						arroba++;
					}
					if(arroba == 5) {
						pos = i + 1;
						break;
					}
				}
				/* Altera Arquivo de Dados */
				ARQUIVO[((indice_alterar->rrn)*192) + pos] = desconto[0];
				ARQUIVO[((indice_alterar->rrn)*192) + pos+1] = desconto[1];
				ARQUIVO[((indice_alterar->rrn)*192) + pos+2] = desconto[2];

				/* Atualiza indice secundario de preco */
				for(int i = 0 ; i < *nregistros ; i++) {
					if(strcmp(iprice[i].pk, indice_alterar->pk) == 0) {
						float preco;
						Produto j = recuperar_registro(indice_alterar->rrn);
						preco = atof(j.preco) / 100;
						preco = preco * (100 - d) * 100;
						iprice[i].price = ((int) preco)/ (float) 100;
						qsort(iprice, (size_t) *nregistros, sizeof(Isf), comp_isf);
						break;
					}
				}
				return 1;
			}
		} else {
			return 0;
		}
	} else {
		printf(REGISTRO_N_ENCONTRADO);
	}

	return 0;
}

int atualizar_arquivo(int nreg) {

	char temp[193];
	int i = 0; // read position
	int j = 0; // write position
	for(i = 0 ; i < nreg ; i++) {
		strncpy(temp, ARQUIVO + ((i)*TAM_REGISTRO), 192);

		temp[192] = '\0';

		if(temp[0] != '*' || temp[1] != '|') {
			strncpy(ARQUIVO + ((j)*TAM_REGISTRO), temp, 192);
			j++;
		}
	}

	ARQUIVO[j*TAM_REGISTRO] ='\0';

	return j;
}

int exibir_registro(int rrn, char com_desconto)
{
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
  	char *cat, categorias[TAM_CATEGORIA];
	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	if(!com_desconto)
	{
		printf("%s\n", j.preco);
		printf("%s\n", j.desconto);
	}
	else
	{
		sscanf(j.desconto,"%d",&desconto);
		sscanf(j.preco,"%f",&preco);
		preco = preco *  (100-desconto);
		preco = ((int) preco)/ (float) 100 ;
		printf("%07.2f\n",  preco);

	}
	strcpy(categorias, j.categoria);

	cat = strtok (categorias, "|");

	while(cat != NULL){
		printf("%s", cat);
		cat = strtok (NULL, "|");
		if(cat != NULL){
			printf(", ");
		}
	}

	printf("\n");

	return 1;
}

int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
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


/* Imprimir indices secundarios */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat){
	int opPrint = 0;
	ll *aux;
	printf(INICIO_ARQUIVO_SECUNDARIO);
	scanf("%d", &opPrint);
	if(!nregistros)
		printf(ARQUIVO_VAZIO);
	switch (opPrint) {
		case 1:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",iproduct[i].pk, iproduct[i].string);
			}
		break;
		case 2:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",ibrand[i].pk, ibrand[i].string);
			}
		break;
		case 3:
			for(int i = 0; i < ncat; i++){
				printf("%s", icategory[i].cat);
				aux =  icategory[i].lista;
				while(aux != NULL){
					printf(" %s", aux->pk);
					aux = aux->prox;
				}
				printf("\n");
			}
		break;

		case 4:
		for(int i = 0; i < nregistros; i++){
			printf("%s %.2f\n",iprice[i].pk, iprice[i].price);
		}
		break;
	}
}
