/*
  Sudoku em C
  Autor: Igor Gabriel
  Data: 03/11/2025
  Descrição: Implementação simples de Sudoku no terminal.
*/

#ifdef _WIN32
#include <windows.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAMANHO 9                      // tamanho do tabuleiro
#define BLOCO 3                        // tamanho da submatriz
#define LIMITE_SUB (BLOCO - 1)         // fim de submatriz
#define FIM_LINHA_SUB1 (BLOCO - 1)     // fim da linha 1 de submatrizes
#define FIM_LINHA_SUB2 (2 * BLOCO - 1) // fim da linha 2 de submatrizes

// declarar tabuleiro
typedef struct {

  int matriz[TAMANHO][TAMANHO];
  int jogadasPlayer[TAMANHO][TAMANHO]; // onde o player jogou

} Tabuleiro;

typedef struct {

  int linha;
  int coluna;
  int numero;

} Jogada;

// lista de tipos de linha pra interface
typedef enum {

  LINHA_VERTICAL_SUB, // separar numeros dentro da submatriz
  LINHA_HORIZONTAL_SUB,
  LINHA_VERTICAL_SEP, // separar submatriz
  LINHA_HORIZONTAL_SEP,
  LINHA_HORIZONTAL_EXT,
  LINHA_VERTICAL_EXT

} TipoLinha;

// lista de cores pra interface
typedef enum {

  AZUL,
  VERDE,
  VERMELHO,
  AMARELO,
  PRETO,
  RESET

} Cores;

#define ANSI_AZUL "\x1b[38;2;100;150;255m"
#define ANSI_VERDE "\x1b[38;2;120;200;140m"
#define ANSI_VERMELHO "\x1b[38;2;255;100;100m"
#define ANSI_AMARELO "\x1b[38;2;255;220;120m"
#define ANSI_PRETO "\033[38;5;0m"
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
void habilitarCores();
void removerNumeros(Tabuleiro *t);
void criarSudoku(Tabuleiro *t);
void gerarDiagonais(Tabuleiro *t);

int existeNumeroLinha(Tabuleiro *t, int linha, int numero, int *colunaConflito);
int existeNumeroColuna(Tabuleiro *t, int coluna, int numero, int *linhaConflito);
int existeNumeroSubmatriz(Tabuleiro *t, int linha, int coluna, int numero, int *linhaConflito, int *colunaConflito);
int checarLinhaCheia(Tabuleiro *t, int linha);
int checarColunaCheia(Tabuleiro *t, int coluna);
int lerNumero(const char *prompt);
int espacoVazio(Tabuleiro *t);
int preencherResto(Tabuleiro *t, int linha, int coluna);

const char *codigoCor(Cores cor);
// fim declaracoes

int main() {
  srand(time(NULL));

  Tabuleiro jogo;

  inicializarMatriz(&jogo);
  criarSudoku(&jogo);

  while (espacoVazio(&jogo) == 1) {

    habilitarCores();
    imprimirMatriz(&jogo);

    Jogada jogar = pedirJogada(&jogo);
    fazerJogada(&jogo, jogar);

    limparTela();
  }

  printf("Parabens! Tabuleiro completo!\nJogo finalizado.\n");

  return 0;
}

//  zera matriz e jogadas
void inicializarMatriz(Tabuleiro *t) {
  for (int linha = 0; linha < TAMANHO; linha++) {
    for (int coluna = 0; coluna < TAMANHO; coluna++) {

      t->matriz[linha][coluna] = 0;
      t->jogadasPlayer[linha][coluna] = 0;
    }
  }
}

void criarSudoku(Tabuleiro *t) {
  gerarDiagonais(t);
  preencherResto(t, 0, 0);
  removerNumeros(t);
}

// preenche submatrizes diagonais
void gerarDiagonais(Tabuleiro *t) {
  for (int bloco = 0; bloco < TAMANHO; bloco += 3) {
    int numeros[9];
    for (int i = 0; i < 9; i++) {
      numeros[i] = i + 1;
    }

    // embaralhar numeros
    for (int i = 8; i > 0; i--) {
      int j = rand() % (i + 1);
      int tmp = numeros[i];
      numeros[i] = numeros[j];
      numeros[j] = tmp;
    }

    // preenche o bloco 3x3 com o array embaralhado
    int pos = 0;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        t->matriz[bloco + i][bloco + j] = numeros[pos++];
      }
    }
  }
}

// preenche submatrizes nao diagonais
int preencherResto(Tabuleiro *t, int linha, int coluna) {
  if (coluna >= TAMANHO) { // chegou ao fim da linha
    linha++;
    coluna = 0;
  }

  if (linha >= TAMANHO) { // fim da matriz com tudo valido
    return 1;
  }

  // pula blocos diagonais ja preenchidos
  int blocoLinha = linha / BLOCO;
  int blocoColuna = coluna / BLOCO;

  if (blocoLinha == blocoColuna) { //  bloco preenchido, pula
    return preencherResto(t, linha, coluna + 1);
  }
  if (coluna >= TAMANHO) { //  terminou linha, pula pra proxima e volta coluna
    return preencherResto(t, linha + 1, 0);
  }
  if (t->matriz[linha][coluna] != 0) { //  ja tem numero, proxima coluna
    return preencherResto(t, linha, coluna + 1);
  }

  int numeros[9];
  for (int i = 0; i < 9; i++)
    numeros[i] = i + 1;

  // embaralha
  for (int i = 8; i > 0; i--) {
    int j = rand() % (i + 1);
    int tmp = numeros[i];
    numeros[i] = numeros[j];
    numeros[j] = tmp;
  }

  for (int k = 0; k < 9; k++) {
    int num = numeros[k];
    if (!existeNumeroLinha(t, linha, num, NULL) &&
        !existeNumeroColuna(t, coluna, num, NULL) &&
        !existeNumeroSubmatriz(t, linha, coluna, num, NULL, NULL)) {

      t->matriz[linha][coluna] = num; // numero valido, coloca

      if (preencherResto(t, linha, coluna + 1) == 1) { //  checa se numero mantem jogo valido
        return 1;
      }

      t->matriz[linha][coluna] = 0; //  numero deixou jogo invalido, desfaz
    }
  }

  return 0;
}

void removerNumeros(Tabuleiro *t) {
  int total = TAMANHO * TAMANHO; // 81

  // randomiza dificuldade
  int chance = rand() % 100;
  int remover;

  if (chance < 30) {
    remover = 25 + rand() % 10;
  } else if (chance < 80) {
    remover = 35 + rand() % 10;
  } else {
    remover = 45 + rand() % 6;
  }

  int pos[81];
  for (int i = 0; i < total; i++)
    pos[i] = i;

  // embaralha posicoes
  for (int i = total - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    int temp = pos[i];
    pos[i] = pos[j];
    pos[j] = temp;
  }

  // remove as escolhidas por posicoes lineares
  for (int i = 0; i < remover; i++) {
    int linha = pos[i] / TAMANHO; //pega linha
    int coluna = pos[i] % TAMANHO;  //pega coluna
    t->matriz[linha][coluna] = 0;
  }

  int pistas = total - remover;
  if (remover < 34) {
    printf("\nSudoku gerado com %d pistas (%d removidos. Dificuldade = Facil).\n", pistas, remover);
  } else if (remover < 44) {
    printf("\nSudoku gerado com %d pistas (%d removidos. Dificuldade = Medio).\n", pistas, remover);
  } else {
    printf("\nSudoku gerado com %d pistas (%d removidos. Dificuldade = Dificil).\n", pistas, remover);
  }
}

// imprime matriz com separadodres
// bagunca
void imprimirMatriz(Tabuleiro *t) {
  imprimirIndicadorColuna();

  for (int linha = 0; linha < TAMANHO; linha++) {
    imprimirIndicadorLinha(linha); // imprime indicador de linha atual
    imprimirLinha(LINHA_VERTICAL_EXT);
    for (int coluna = 0; coluna < TAMANHO; coluna++) {

      if (t->matriz[linha][coluna] == 0) {
        printf("   ");
      } else if (t->jogadasPlayer[linha][coluna] == 1) { // se numero for jogado pelo player, imprimir numero amarelo
        printf("%s %d %s", codigoCor(AMARELO), t->matriz[linha][coluna], codigoCor(RESET));
      } else { // numero predefinido, sem cor
        printf("%s %d ", codigoCor(RESET), t->matriz[linha][coluna]);
      }

      if (coluna == FIM_LINHA_SUB1 || coluna == FIM_LINHA_SUB2) { // separa submatrizes verticalmente

        imprimirLinha(LINHA_VERTICAL_SEP);

      } else if (coluna < TAMANHO - 1) { // separa numeros dentro de submatrizes verticalmente
        imprimirLinha(LINHA_VERTICAL_SUB);
      }
    }

    imprimirLinha(LINHA_VERTICAL_EXT);

    imprimirIndicadorLinhaVazio(); // separa as linhas

    if (linha == FIM_LINHA_SUB1 || linha == FIM_LINHA_SUB2) { // separa submatrizes horizontalmente
      imprimirLinha(LINHA_VERTICAL_EXT);
      imprimirLinha(LINHA_HORIZONTAL_SEP);
    } else if (linha < TAMANHO - 1) { // separa numeros dentro de submatrizes horizontalmente
      imprimirLinha(LINHA_VERTICAL_EXT);
      imprimirLinha(LINHA_HORIZONTAL_SUB);
    }
  }

  imprimirLinha(LINHA_HORIZONTAL_EXT);
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

      if (i == FIM_LINHA_SUB1 || i == FIM_LINHA_SUB2) { // separa submatrizes verticalmente
        printf("%s||%s", codigoCor(VERDE), codigoCor(RESET));
      } else if (i < TAMANHO - 1) { // junta linhas com colunas dentro da sub
        printf("%s+%s", codigoCor(AZUL), codigoCor(RESET));
      }
    }
    imprimirLinha(LINHA_VERTICAL_EXT);
    printf("\n"); // fim da linha, quebra pra proxima

    break;

  case LINHA_VERTICAL_SEP:
    printf("%s||%s", codigoCor(VERDE), codigoCor(RESET));
    break;

  case LINHA_HORIZONTAL_SEP:
    for (int i = 0; i < TAMANHO; i++) {
      printf("%s===%s", codigoCor(VERDE), codigoCor(RESET));

      if (i == FIM_LINHA_SUB1 || i == FIM_LINHA_SUB2) { // junta linha do separador com coluna sep
        printf("%s++%s", codigoCor(VERDE), codigoCor(RESET));
      } else if (i < TAMANHO - 1) { // junta linha do separador com coluna sub
        printf("%s+%s", codigoCor(VERDE), codigoCor(RESET));
      }
    }

    imprimirLinha(LINHA_VERTICAL_EXT);

    printf("\n"); // fim da linha, quebra pra proxima

    break;

  case LINHA_HORIZONTAL_EXT:
    for (int i = 0; i < 13; i++) {
      printf("%s~~~%s", codigoCor(VERMELHO), codigoCor(RESET));
    }

    printf("\n\n");

    break;

  case LINHA_VERTICAL_EXT:
    printf("%s|%s", codigoCor(VERMELHO), codigoCor(RESET));
    break;
  }
}

// imprime indicador "col" acima do tabuleiro
//  e separadores visua
void imprimirIndicadorColuna() {

  imprimirIndicadorLinhaVazio(); // linha de espaco inicial

  // repete "col" pras colunas
  for (int i = 0; i < TAMANHO; i++) {
    printf("%s Col%s", codigoCor(VERMELHO), codigoCor(RESET));

    if (i == FIM_LINHA_SUB1 || i == FIM_LINHA_SUB2) { // espaco vazio por causa dos separadores verticais
      printf(" ");
    }
  }

  imprimirIndicadorLinhaVazio();

  // imprime o indice da coluna
  for (int c = 0; c < TAMANHO; c++) {
    printf("  %s%d%s ", codigoCor(VERMELHO), c + 1, codigoCor(RESET));

    if (c == FIM_LINHA_SUB1 || c == FIM_LINHA_SUB2) { // espaco vazio por causa dos separadores verticais
      printf(" ");
    }
  }

  imprimirIndicadorLinhaVazio(); // linha de espaco final

  for (int l = 0; l < TAMANHO; l++) {

    printf("%s  v %s", codigoCor(VERMELHO), codigoCor(RESET)); // indicador de coluna

    if (l == FIM_LINHA_SUB1 || l == FIM_LINHA_SUB2) { // espaco vazio por causa dos separadores verticais
      printf(" ");
    }
  }
  printf("\n");
}

// imprime indicador de linha atual
void imprimirIndicadorLinha(int linha) {
  printf("%sLin %d -> %s", codigoCor(VERMELHO), linha + 1, codigoCor(RESET));
}

// espaco vazio pra alinhar tabuleiro com indicadores
void imprimirIndicadorLinhaVazio() {
  printf("\n         ");
}

// pedir e validar jogada
Jogada pedirJogada(Tabuleiro *t) {
  Jogada jogar;

  do {
    jogar.linha = lerNumero("Qual linha jogar? \n");

    if (checarLinhaCheia(t, jogar.linha - 1)) {
      printf("Jogada Invalida! Linha %d ja esta cheia! Selecione outra linha.\n", jogar.linha);
      continue; // retorna pro comeco
    }

    jogar.coluna = lerNumero("Qual coluna jogar? \n");

    if (checarColunaCheia(t, jogar.coluna - 1)) {
      printf("Jogada Invalida! coluna %d ja esta cheia! Selecione outra posicao.\n", jogar.coluna);
    }
    if (t->matriz[jogar.linha - 1][jogar.coluna - 1] != 0) {
      printf("Jogada invalida! A posicao linha %d coluna %d ja esta ocupada com o numero %d\n", jogar.linha, jogar.coluna, t->matriz[jogar.linha - 1][jogar.coluna - 1]);
      continue;
    }

    int linhaConflito, colunaConflito; // guarda qual linha e coluna deu conflito numero entrada do jogador na submatriz
    int conflito;                      // que linha/coluna conflitou

    jogar.numero = lerNumero("Qual numero jogar? \n");

    if (existeNumeroLinha(t, jogar.linha - 1, jogar.numero, &conflito) == 1) {
      printf("Jogada invalida! O numero %d ja existe na linha %d, coluna %d\n", jogar.numero, jogar.linha, conflito + 1);
      continue;
    }
    if (existeNumeroColuna(t, jogar.coluna - 1, jogar.numero, &conflito) == 1) {
      printf("Jogada invalida! O numero %d ja existe na linha %d, coluna %d\n", jogar.numero, conflito + 1, jogar.coluna);
      continue;
    }
    if (existeNumeroSubmatriz(t, jogar.linha - 1, jogar.coluna - 1, jogar.numero, &linhaConflito, &colunaConflito)) {
      printf("Jogada invalida! O numero %d ja existe na submatriz 3x3 na linha %d, coluna %d\n", jogar.numero, linhaConflito + 1, colunaConflito + 1);
      continue;
    }

    break; // checks passaram, sai do while

  } while (1);

  return jogar;
}

// faz jogada com valores da funcao pedir
void fazerJogada(Tabuleiro *t, Jogada jogar) {
  t->matriz[jogar.linha - 1][jogar.coluna - 1] = jogar.numero;
  t->jogadasPlayer[jogar.linha - 1][jogar.coluna - 1] = 1; //  para imprimirMatriz saber que eh um numero do player
}

int existeNumeroLinha(Tabuleiro *t, int linha, int numero, int *colunaConflito) {
  for (int c = 0; c < TAMANHO; c++) {
    if (t->matriz[linha][c] == numero) {
      if (colunaConflito != NULL) {
        *colunaConflito = c;
      }
      return 1;
    }
  }
  return 0;
}

int existeNumeroColuna(Tabuleiro *t, int coluna, int numero, int *linhaConflito) {
  for (int l = 0; l < TAMANHO; l++) {
    if (t->matriz[l][coluna] == numero) {
      if (linhaConflito != NULL) {
        *linhaConflito = l; // salva conflito pra avisar o player
      }
      return 1;
    }
  }
  return 0;
}

// teste se numero existe no bloco 3x3
int existeNumeroSubmatriz(Tabuleiro *t, int linha, int coluna, int numero, int *linhaConflito, int *colunaConflito) {
  // pega indice inicial do bloco
  int inicioLinha = (linha / BLOCO) * BLOCO;
  int inicioColuna = (coluna / BLOCO) * BLOCO;

  // compara dentro do bloco
  for (int l = inicioLinha; l < inicioLinha + BLOCO; l++) {
    for (int c = inicioColuna; c < inicioColuna + BLOCO; c++) {
      if (t->matriz[l][c] == numero) {
        if (linhaConflito != NULL && colunaConflito != NULL) {
          *linhaConflito = l;
          *colunaConflito = c;
        }
        return 1;
      }
    }
  }
  return 0;
}

// testa se eh possivel jogar na linha
int checarLinhaCheia(Tabuleiro *t, int linha) {
  for (int c = 0; c < TAMANHO; c++) {
    if (t->matriz[linha][c] == 0) {
      return 0;
    }
  }
  return 1;
}
// testa se eh possivel jogar na coluna
int checarColunaCheia(Tabuleiro *t, int coluna) {
  for (int l = 0; l < TAMANHO; l++) {
    if (t->matriz[l][coluna] == 0) {
      return 0;
    }
  }
  return 1;
}

// vaidar input
int lerNumero(const char *prompt) {

  char input[50];
  int valido = 0;
  int numero = 0;

  while (valido == 0) {

    printf("%s", prompt);

    fgets(input, sizeof(input), stdin);

    // remove o \n se existir
    input[strcspn(input, "\n")] = '\0';

    // so aceitar numeros, 1 a 9
    if (strlen(input) == 1 && isdigit(input[0]) && input[0] != '0') {
      numero = input[0] - '0';
      valido = 1;
    } else {
      printf("Jogada invalida! Digite apenas numeros de 1 a 9.\n");
    }
  }

  return numero;
}

// checar se ha espaco pra jogar no tabuleiro
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

// limpar terminal, chamar depois de jogada
void limparTela() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

#ifdef _WIN32
void habilitarCores() { // habilitar cores no terminal do windows
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD modo = 0;
  GetConsoleMode(hOut, &modo);
  modo |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
  SetConsoleMode(hOut, modo);
}
#else
void habilitarCores() {
}
#endif

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
  case PRETO:
    return ANSI_PRETO;
  default:
    return ANSI_RESET;
  }
}
