/*
  Sudoku em C
  Autor: Igor Gabriel
  Data: 03/11/2025
  Descrição: Implementação simples de Sudoku no terminal.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO 9 //tamanho do tabuleiro
#define BLOCO 3 //tamanho da submatriz
#define LIMITE_SUB (BLOCO - 1)         // fim de submatriz 
#define FIM_LINHA_SUB1 (BLOCO - 1)     // fim da linha 1 de submatrizes 
#define FIM_LINHA_SUB2 (2 * BLOCO - 1) // fim da linha 2 de submatrizes 

// declarar tabuleiro
typedef struct { 

  int matriz[TAMANHO][TAMANHO]; 
  int jogadasPlayer[TAMANHO][TAMANHO];  //salva onde o player jogou 

} Tabuleiro;

typedef struct {

  int linha;
  int coluna;
  int numero;

} Jogada;

// lista de tipos de linha pra interface
typedef enum { 

  LINHA_VERTICAL_SUB,   //separar numeros dentro da submatriz
  LINHA_HORIZONTAL_SUB,
  LINHA_VERTICAL_SEP,   //separar submatriz
  LINHA_HORIZONTAL_SEP, 
  LINHA_FIM_TABULEIRO

} TipoLinha;

 // lista de cores pra interface
typedef enum {

  AZUL,
  VERDE,
  VERMELHO,
  AMARELO,
  RESET

} Cores;

#define ANSI_AZUL "\x1b[34m"
#define ANSI_VERDE "\x1b[32m"
#define ANSI_VERMELHO "\x1b[31m"
#define ANSI_AMARELO "\x1b[33m"
#define ANSI_RESET "\x1b[0m"

// declaracoes das funcoes
Jogada pedirJogada(Tabuleiro *t);

void inicializarMatriz(Tabuleiro *t);
void imprimirMatriz(Tabuleiro *t);
void imprimirLinha(TipoLinha tipo);
void fazerJogada(Tabuleiro *t, Jogada jogar);
void imprimirIndicadorColuna();
void imprimirIndicadorLinha(int linha);
void imprimirIndicadorLinhaVazio();
void limparTela();

int existeNumeroLinha(Tabuleiro *t, int linha, int numero, int *colunaConflito);
int existeNumeroColuna(Tabuleiro *t, int coluna, int numero, int *linhaConflito);
int existeNumeroSubmatriz(Tabuleiro *t, int linha, int coluna, int numero, int *linhaConflito, int *colunaConflito);
int checarLinhaCheia(Tabuleiro *t, int linha);
int checarColunaCheia(Tabuleiro *t, int coluna);
int lerNumero(const char *prompt);
int espacoVazio(Tabuleiro *t);

const char *codigoCor(Cores cor);

int main() {

  Tabuleiro jogo;
  inicializarMatriz(&jogo);

  while (espacoVazio(&jogo) == 1) {

    imprimirMatriz(&jogo);
    Jogada jogar = pedirJogada(&jogo);

    fazerJogada(&jogo, jogar);
    limparTela();

  }

  printf("Parabens! Tabuleiro completo!\nJogo finalizado.\n");

  return 0;

}

//setar matriz  TODO: gerar matriz randomizada
void inicializarMatriz(Tabuleiro *t) { 
  
  int matrizInicial[TAMANHO][TAMANHO] = {
      {5, 3, 0, 0, 7, 0, 0, 0, 0},
      {6, 0, 0, 1, 9, 5, 0, 0, 0},
      {0, 9, 8, 0, 0, 0, 0, 6, 0},

      {8, 0, 0, 0, 6, 0, 0, 0, 3},
      {4, 0, 0, 8, 0, 3, 0, 0, 1},
      {7, 0, 0, 0, 2, 0, 0, 0, 6},

      {0, 6, 0, 0, 0, 0, 2, 8, 0},
      {0, 0, 0, 4, 1, 9, 0, 0, 5},
      {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

  for (int linha = 0; linha < TAMANHO; linha++) {
    for (int coluna = 0; coluna < TAMANHO; coluna++) {

      t->matriz[linha][coluna] = matrizInicial[linha][coluna];
      t->jogadasPlayer[linha][coluna] = 0;

    }
  }
}

//imprime matriz com separadodres
void imprimirMatriz(Tabuleiro *t) {
  imprimirIndicadorColuna(); 


  for (int linha = 0; linha < TAMANHO; linha++) {
    imprimirIndicadorLinha(linha);  //imprime indicador de linha atual

    for (int coluna = 0; coluna < TAMANHO; coluna++) {

      if (t->jogadasPlayer[linha][coluna] == 1) {   //se numero for jogado pelo player, imprimir numero amarelo
        printf("%s %d %s", codigoCor(AMARELO), t->matriz[linha][coluna], codigoCor(RESET));
      } else {  //numero predefinido, sem cor
        printf("%s %d ", codigoCor(RESET), t->matriz[linha][coluna]);
      }

      if (coluna == FIM_LINHA_SUB1 || coluna == FIM_LINHA_SUB2) {     //separa submatrizes verticalmente
        imprimirLinha(LINHA_VERTICAL_SEP);

      } else if (coluna < TAMANHO - 1) {  //separa numeros dentro de submatrizes verticalmente
        imprimirLinha(LINHA_VERTICAL_SUB);
      }

    }
    
    imprimirIndicadorLinhaVazio(); //separa as linhas

    if (linha == FIM_LINHA_SUB1 || linha == FIM_LINHA_SUB2) {   //separa submatrizes horizontalmente
      imprimirLinha(LINHA_HORIZONTAL_SEP);
    } else if (linha < TAMANHO - 1) {   //separa numeros dentro de submatrizes horizontalmente
      imprimirLinha(LINHA_HORIZONTAL_SUB);
    }
  }

  imprimirLinha(LINHA_FIM_TABULEIRO);

}

// linhas pra interface
void imprimirLinha(TipoLinha tipo) { 
  switch (tipo) {

  case LINHA_VERTICAL_SUB:
    printf("%s|%s", codigoCor(AZUL), codigoCor(RESET));
    break;

  case LINHA_HORIZONTAL_SUB:
    for (int i = 0; i < TAMANHO; i++) {

      printf("%s---%s", codigoCor(AZUL), codigoCor(RESET));

      if (i == FIM_LINHA_SUB1 || i == FIM_LINHA_SUB2) {   //separa submatrizes verticalmente
        printf("%s||%s", codigoCor(VERDE), codigoCor(RESET));
      } else if (i < TAMANHO - 1) { // junta linhas com colunas dentro da sub
        printf("%s+%s", codigoCor(AZUL), codigoCor(RESET));
      }

    }

    printf("\n"); //fim da linha, quebra pra proxima

    break;

  case LINHA_VERTICAL_SEP:
    printf("%s||%s", codigoCor(VERDE), codigoCor(RESET));
    break;

  case LINHA_HORIZONTAL_SEP:
    for (int i = 0; i < TAMANHO; i++) {
      printf("%s===%s", codigoCor(VERDE), codigoCor(RESET));

      if (i == FIM_LINHA_SUB1 || i == FIM_LINHA_SUB2) { //junta linha do separador com coluna sep
        printf("%s++%s", codigoCor(VERDE), codigoCor(RESET));
      } else if (i < TAMANHO - 1) { //junta linha do separador com coluna sub
        printf("%s+%s", codigoCor(VERDE), codigoCor(RESET));
      }

    }

    printf("\n"); //fim da linha, quebra pra proxima

    break;
    
  case LINHA_FIM_TABULEIRO:
    printf("%s~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s\n\n", codigoCor(VERMELHO), codigoCor(RESET));
    break;
  }
}

//imprime indicador "col" acima do tabuleiro
// e separadores visua
void imprimirIndicadorColuna() {

  imprimirIndicadorLinhaVazio();  //linha de espaco inicial

  //repete "col" pras colunas
  for (int i = 0; i < TAMANHO; i++) {
    printf("%sCol%s ", codigoCor(VERMELHO), codigoCor(RESET));

    if (i == FIM_LINHA_SUB1 || i == FIM_LINHA_SUB2) {   //espaco vazio por causa dos separadores verticais
      printf(" ");
    }

  }

  imprimirIndicadorLinhaVazio();

  //imprime o indice da coluna
  for (int c = 0; c < TAMANHO; c++) {
    printf(" %s%d%s  ", codigoCor(VERMELHO), c + 1, codigoCor(RESET));

    if (c == FIM_LINHA_SUB1 || c == FIM_LINHA_SUB2) { //espaco vazio por causa dos separadores verticais
      printf(" ");
    }

  }

  imprimirIndicadorLinhaVazio();  //linha de espaco final

  for (int l = 0; l < TAMANHO; l++) {

    printf("%s v  %s", codigoCor(VERMELHO), codigoCor(RESET));  //indicador de coluna

    if (l == FIM_LINHA_SUB1 || l == FIM_LINHA_SUB2) { //espaco vazio por causa dos separadores verticais
      printf(" ");
    }

  }
  printf("\n");
}

//imprime indicador de linha atual
void imprimirIndicadorLinha(int linha) {
  printf("%sLin %d ->%s", codigoCor(VERMELHO), linha + 1, codigoCor(RESET));
}

//espaco vazio pra alinhar tabuleiro com indicadores
void imprimirIndicadorLinhaVazio() {
  printf("\n        ");
}

//pedir e validar jogada
Jogada pedirJogada(Tabuleiro *t) {
  Jogada jogar;

  do {
    jogar.linha = lerNumero("Qual linha jogar? \n");

    if (checarLinhaCheia(t, jogar.linha - 1)) {
      printf("Jogada Invalida! Linha %d ja esta cheia! Selecione outra linha.\n", jogar.linha);
      continue; //retorna pro comeco
    }

    jogar.coluna = lerNumero("Qual coluna jogar? \n");

    if (checarColunaCheia(t, jogar.coluna - 1)) {
      printf("Jogada Invalida! coluna %d ja esta cheia! Selecione outra posicao.\n", jogar.coluna);
    }

    if (t->matriz[jogar.linha - 1][jogar.coluna - 1] != 0) {
      printf("Jogada invalida! A posicao linha %d coluna %d ja esta ocupada com o numero %d\n", jogar.linha, jogar.coluna, t->matriz[jogar.linha - 1][jogar.coluna - 1]);
      continue;
    }

    int linhaConflito, colunaConflito;  //guarda qual linha e coluna deu conflito numero entrada do jogador na submatriz
    int conflito; //que linha/coluna conflitou

    jogar.numero = lerNumero("Qual numero jogar? \n");

    if (existeNumeroLinha(t, jogar.linha - 1, jogar.numero, &conflito) == 1) {
      printf("Jogada invalida! O numero %d ja existe na linha %d, coluna %d\n", jogar.numero, jogar.linha, conflito + 1);
      continue;
    }

    if (existeNumeroColuna(t, jogar.coluna - 1, jogar.numero, &conflito) == 1) {
      printf("Jogada invalida! O numero %d ja existe na coluna %d, linha %d\n", jogar.numero, jogar.coluna, conflito + 1);
      continue;
    }

    if (existeNumeroSubmatriz(t, jogar.linha - 1, jogar.coluna - 1, jogar.numero, &linhaConflito, &colunaConflito)) {
      printf("Jogada invalida! O numero %d ja existe na submatriz 3x3 na linha %d, coluna %d\n", jogar.numero, linhaConflito + 1, colunaConflito + 1);
      continue;
    }

    break;  // checks passaram, sai do while

  } while (1);  

  return jogar; //todos checks passaram, salva onde vai jogar

}

//faz jogada com valores da funcao pedir
void fazerJogada(Tabuleiro *t, Jogada jogar) {
  t->matriz[jogar.linha - 1][jogar.coluna - 1] = jogar.numero;
  t->jogadasPlayer[jogar.linha - 1][jogar.coluna - 1] = 1;  //  para imprimirMatriz saber que eh um numero do player
}

int existeNumeroLinha(Tabuleiro *t, int linha, int numero, int *colunaConflito) {
  for (int c = 0; c < TAMANHO; c++) {
    if (t->matriz[linha][c] == numero) {
      *colunaConflito = c;  //salva conflito pra mostrar

      return 1;
    }
  }

  return 0;

}

int existeNumeroColuna(Tabuleiro *t, int coluna, int numero, int *linhaConflito) {
  for (int l = 0; l < 9; l++) {
    if (t->matriz[l][coluna] == numero) {
      *linhaConflito = l; //salva conflito pra avisar o player
      return 1;
    }
  }
  return 0;
}

//testa se eh possivel jogar na linha
int checarLinhaCheia(Tabuleiro *t, int linha) {
  for (int c = 0; c < TAMANHO; c++) {
    if (t->matriz[linha][c] == 0) {
      return 0;
    }
  }
  return 1;
}
//testa se eh possivel jogar na coluna
int checarColunaCheia(Tabuleiro *t, int coluna) {
  for (int l = 0; l < TAMANHO; l++) {
    if (t->matriz[l][coluna] == 0) {
      return 0;
    }
  }
  return 1;
}

//teste se numero existe no bloco 3x3
int existeNumeroSubmatriz(Tabuleiro *t, int linha, int coluna, int numero, int *linhaConflito, int *colunaConflito) {
  //pega indice inicial do bloco
  int inicioLinha = (linha / BLOCO) * BLOCO;
  int inicioColuna = (coluna / BLOCO) * BLOCO;

  //compara dentro do bloco
  for (int l = inicioLinha; l < inicioLinha + BLOCO; l++) {
    for (int c = inicioColuna; c < inicioColuna + BLOCO; c++) {
      if (t->matriz[l][c] == numero) {
        *linhaConflito = l;
        *colunaConflito = c;
        return 1;
      }
    }
  }
  return 0;
}

//validar input
int lerNumero(const char *prompt) {

  char input[5];
  int valido = 0;
  int numero = 0;

  while (valido == 0) {
    
    printf("%s", prompt);
    scanf("%s", input);

    if (strlen(input) == 1 && isdigit(input[0]) && input[0] != '0') {
      numero = input[0] - '0';
      valido = 1;
    } else {
      printf("Jogada invalida! Digite apenas numeros de 1 a 9.\n");
    }
  }
  return numero;
}

//checar se ha espaco no tabuleiro
int espacoVazio(Tabuleiro *t) {
  for (int i = 0; i < TAMANHO; i++) {
    for (int j = 0; j < TAMANHO; j++) {
      if (t->matriz[i][j] == 0) {
        return 1;
      }
    }
  }
  return 0;
}

//limpar terminal, chamar depois de jogada
void limparTela() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

const char *codigoCor(Cores cor) {
  switch (cor) {
  case AZUL:
    return ANSI_AZUL;
  case VERDE:
    return ANSI_VERDE;
  case VERMELHO:
    return ANSI_VERMELHO;
  case AMARELO:
    return ANSI_AMARELO;
  default:
    return ANSI_RESET;
  }
}
