#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <conio.h>

#define TAM 7
#define MONSTROS 10
#define QTDE_FASE 3
#define PAREDE 10


void gotoxy(int x, int y) {
    COORD c;
    c.X = x - 1;
    c.Y = y - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

int movimentos = 0, fase = 0;

char mapa[QTDE_FASE][TAM][TAM] = {
    // FASE 0
    {
        {'H', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', 'S'}
    },
    // FASE 1
    {
        {'H', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', 'S'}
    },
    // FASE 2
    {
        {'H', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', 'S'}
    }
};
struct personagem {
    int vida, atq, def, posx, posy, chave, movimento;
} heroi, monstros[MONSTROS];

struct Ranking {
    char nome[50];
    int movimentos;
} historico[100]; // Suporta até 100 partidas salvas na memória

int qtdJogadores = 0; // Conta quantos já jogaram

// Função para limpar o mapa sempre que um NOVO jogo começar
void reiniciaMapa() {
    int f, l, c;
    for(f = 0; f < 3; f++) {// f == fase atual
        for(l = 0; l < TAM; l++) {
            for(c = 0; c < TAM; c++) {
                mapa[f][l][c] = '.';
            }
        }
        // Coloca o herói e a saída de volta nas posições originais
        mapa[f][0][0] = 'H';
        mapa[f][TAM-1][TAM-1] = 'S';
    }
}

void salvarHistorico() {
    FILE *arq = fopen("historico.txt", "w"); // Cria/abre o arquivo em modo de escrita
    if (arq == NULL) {
        gotoxy(3, 22); printf("Erro ao salvar o historico no arquivo!");
        return;
    }

    // Grava cada jogador no formato: Nome;Movimentos
    for (int i = 0; i < qtdJogadores; i++) {
        fprintf(arq,"Ranking || Nome do jogador || Quantidade  de movimentos\n %s  ||  %d\n",  historico[i].nome, historico[i].movimentos);
    }

    fclose(arq); // Fecha o arquivo
}

void carregarHistorico() {
    FILE *arq = fopen("historico.txt", "r"); // Abre o arquivo em modo de leitura
    if (arq == NULL) {
        // Se o arquivo nao existir ainda (primeira vez jogando), apenas comeca com 0
        qtdJogadores = 0;
        return;
    }

    qtdJogadores = 0;
    // Le o arquivo linha por linha salvando na struct ate acabar o arquivo (EOF)
    // O formato " %[^;];%d\n" serve para ler nomes com espaco ate encontrar o ponto e virgula
    while (fscanf(arq, " %[^;];%d\n", historico[qtdJogadores].nome, &historico[qtdJogadores].movimentos) == 2) {
        qtdJogadores++;
        if (qtdJogadores >= 100) break; // Limite maximo do vetor para seguranca
    }

    fclose(arq);
}



void moldura(int linhaInicial, int linhaFinal, int colInicial, int colFinal, int tipo) {
    int l, c;
    int borda[2][6] = {
        {205, 186, 201, 187, 200, 188},
        {196, 179, 218, 191, 192, 217}
    };
    for (c = colInicial + 1; c <= colFinal - 1; c++) {
        gotoxy(c, linhaInicial); printf("%c", borda[tipo][0] );
        gotoxy(c, linhaFinal);   printf("%c", borda[tipo][0]);
    }
    for (l = linhaInicial + 1; l <= linhaFinal - 1; l++) {
        gotoxy(colInicial, l); printf("%c", borda[tipo][1]);
        gotoxy(colFinal,   l); printf("%c", borda[tipo][1]);
    }
    gotoxy(colInicial, linhaInicial); printf("%c", borda[tipo][2]);
    gotoxy(colFinal,   linhaInicial); printf("%c", borda[tipo][3]);
    gotoxy(colInicial, linhaFinal);   printf("%c", borda[tipo][4]);
    gotoxy(colFinal,   linhaFinal);   printf("%c", borda[tipo][5]);
}

/* ------------------------------------------------------------------ */
/*  EXIBICAO                                                          */
/* ------------------------------------------------------------------ */

void mostraMapa() {
    int l, c;
    int aux = (80 - TAM - 2) / 2;

    moldura(1, 23, 1, 80, 0);
    moldura(9, 17, aux, aux + TAM + 1, 1);

    for (l = 0; l < TAM; l++) {
        for (c = 0; c < TAM; c++) {
            gotoxy(aux + 1 + c, 10 + l);
            // CORREÇÃO: Adicionado o índice [fase]
            printf("%c", mapa[fase][l][c]);
        }
    }

    /* painel de status do heroi */
    gotoxy(3, 19); printf("HP Heroi: %d   ATQ: %d   DEF: %d   ",
                          heroi.vida, heroi.atq, heroi.def);
}
/* ------------------------------------------------------------------ */
/*  GERACAO DE MONSTROS                                                 */
/* ------------------------------------------------------------------ */

void geraMonstros(int qtde) {
    int x, y, cont;
    for (cont = 0; cont < qtde; cont++) {
        x = rand() % TAM;
        y = rand() % TAM;
        // CORREÇÃO: Verificando e gravando no mapa da fase atual
        if (mapa[fase][x][y] == '.') {
            mapa[fase][x][y] = 'M';
            monstros[cont].posx = x;
            monstros[cont].posy = y;
            monstros[cont].atq  = 2;
            monstros[cont].def  = 1;
            monstros[cont].vida = 7;
        } else {
            cont--;   /* posicao ocupada - tenta de novo */
        }
    }
}

void geraParede(int qtde) {
    int x, y, cont;
    for (cont = 0; cont < qtde; cont++) {
        x = rand() % TAM;
        y = rand() % TAM;
        // CORREÇÃO: Verificando e gravando no mapa da fase atual
        if (mapa[fase][x][y] == '.') {
            mapa[fase][x][y] = 'M';
            monstros[cont].posx = x;
            monstros[cont].posy = y;
            monstros[cont].atq  = 2;
            monstros[cont].def  = 1;
            monstros[cont].vida = 7;
        } else {
            cont--;   /* posicao ocupada - tenta de novo */
        }
    }
}








/* ------------------------------------------------------------------ */
/*  BATALHA                                                             */
/* FIX: rolarDados estava incompleta; batalha usava indice fixo [0]    */
/* ------------------------------------------------------------------ */

/*
 rolarDados: mesma logica do main.c para decidir quem ataca - sorteia
 um unico dado (0 ou 1) por turno. Se dado == 0, o heroi ataca; senao,
 o monstro ataca. Aqui o sorteio se repete a cada turno para que a
 batalha continue ate um dos dois ficar com vida <= 0.
 Retorna 1 se o heroi venceu, 0 se o monstro venceu.
 */
int rolarDados(int m) {
    int dado;
    int dano;

    gotoxy(3, 20); printf("== BATALHA INICIADA! ==              ");
    Sleep(600);

    while (heroi.vida > 0 && monstros[m].vida > 0) {
        dado = rand() % 2;

        if (dado == 0) {
            /* heroi ataca */
            dano = heroi.atq - monstros[m].def;
            if (dano > 0) monstros[m].vida -= dano;
            gotoxy(3, 21);
            printf("Heroi atacou! Dano: %d  | HP Monstro: %d   ", dano, monstros[m].vida);
        } else {
            /* monstro ataca */
            dano = monstros[m].atq - heroi.def;
            if (dano > 0) heroi.vida -= dano;
            gotoxy(3, 21);
            printf("Monstro atacou! Dano: %d | HP Heroi: %d    ", dano, heroi.vida);
        }
        Sleep(500);
    }

    if (heroi.vida <= 0) {
        gotoxy(3, 22); printf(">>> Heroi foi derrotado! <<<          ");
        return 0;   /* heroi morreu */
    }

    gotoxy(3, 22); printf(">>> Monstro derrotado! <<<            ");
    return 1;   /* heroi venceu */
}

/*
 * batalha: localiza qual monstro esta em (x, y) e inicia o combate.
 * FIX: antes usava indice fixo monstros[0]; agora usa a variavel m.
 * Apos a vitoria do heroi o monstro e removido do mapa.
 * Retorna 1 se heroi venceu, 0 se perdeu.
 */
int batalha(int x, int y) {
    int m;
    for (m = 0; m < MONSTROS; m++) {
        if (monstros[m].vida > 0 &&          /* monstro ainda vivo    */
            monstros[m].posx == x &&
            monstros[m].posy == y) {

            if (rolarDados(m)) {             /* heroi venceu          */
                // CORREÇÃO: Apagando o monstro da fase atual
                mapa[fase][x][y] = '.';
                monstros[m].vida = 0;        /* marca como morto      */
                return 1;
            }
            return 0;   /* heroi perdeu */
        }
    }
    return 1;
}


int contaMonstro(){
    int cont = 0, x, y;
    for(x = -1; x <= 1; x++){
        // CORREÇÃO: O seu loop for estava assim: for(y=-1;x<1;x++)
        for(y = -1; y <= 1; y++){
            int px = heroi.posx + x;
            int py = heroi.posy + y;

            // Evita procurar fora dos limites da matriz TAMxTAM
            if(px >= 0 && px < TAM && py >= 0 && py < TAM) {
                // CORREÇÃO: Usando o mapa da fase atual
                if(mapa[fase][px][py] == 'M'){
                    cont++;
                }
            }
        }
    }
    // CORREÇÃO: Estava retornando 1 fixo, agora retorna a contagem real
    return cont;
}




/*
 * movimenta: mesmo estilo do main.c (checa direto o tile vizinho em
 * mapa[][], sem calcular novo_x/novo_y antes). A diferenca e que aqui
 * a batalha continua sendo chamada quando o tile vizinho e 'M'.
 */
int movimenta(char letra) {
    int continua = 1;

    if(letra == 'w' || letra == 'a' || letra == 's' || letra == 'd' ||
       letra == 72 || letra == 75 || letra == 80 || letra == 77) {
        movimentos++;
    }

    /* Apaga o 'P' da posicao atual na fase atual */
    mapa[fase][heroi.posx][heroi.posy] = '.';

    if (letra == 's' || letra == 80) {                 /* baixo */
        if (heroi.posx < TAM - 1) {
            char tile = mapa[fase][heroi.posx + 1][heroi.posy];
            if (tile == '.') {
                heroi.posx++;
            } else if (tile == 'M') {
                if (batalha(heroi.posx + 1, heroi.posy)) heroi.posx++;
                else continua = 0;
            } else if (tile == 'S') {
                continua = 0; // Sai do loop para passar de fase
            }
        }
    }
    else if (letra == 'w' || letra == 72) {             /* cima */
        if (heroi.posx > 0) {
            char tile = mapa[fase][heroi.posx - 1][heroi.posy];
            if (tile == '.') {
                heroi.posx--;
            } else if (tile == 'M') {
                if (batalha(heroi.posx - 1, heroi.posy)) heroi.posx--;
                else continua = 0;
            } else if (tile == 'S') {
                continua = 0; // Sai do loop para passar de fase
            }
        }
    }

    if (letra == 'd' || letra == 77) {                  /* direita */
        if (heroi.posy < TAM - 1) {
            char tile = mapa[fase][heroi.posx][heroi.posy + 1];
            if (tile == '.') {
                heroi.posy++;
            } else if (tile == 'M') {
                if (batalha(heroi.posx, heroi.posy + 1)) heroi.posy++;
                else continua = 0;
            } else if (tile == 'S') {
                continua = 0; // Sai do loop para passar de fase
            }
        }
    }
    else if (letra == 'a' || letra == 75) {              /* esquerda */
        if (heroi.posy > 0) {
            char tile = mapa[fase][heroi.posx][heroi.posy - 1];
            if (tile == '.') {
                heroi.posy--;
            } else if (tile == 'M') {
                if (batalha(heroi.posx, heroi.posy - 1)) heroi.posy--;
                else continua = 0;
            } else if (tile == 'S') {
                continua = 0; // Sai do loop para passar de fase
            }
        }
    }

    /* Redesenha o 'P' na posicao final da fase atual */
    mapa[fase][heroi.posx][heroi.posy] = 'P';

    return continua;
}

void exibirHistorico() {
    int i, j;
    struct Ranking aux;

    system("cls");
    moldura(1, 23, 1, 80, 0);
    gotoxy(30, 3); printf("=== HISTORICO DE JOGADORES ===");

    if (qtdJogadores == 0) {
        gotoxy(25, 10); printf("Nenhum jogador registrado ainda!");
    } else {
        // Ordena o vetor (Bubble Sort) para os melhores (menos movimentos) ficarem no topo
        for (i = 0; i < qtdJogadores - 1; i++) {
            for (j = 0; j < qtdJogadores - i - 1; j++) {
                if (historico[j].movimentos > historico[j+1].movimentos) {
                    aux = historico[j];
                    historico[j] = historico[j+1];
                    historico[j+1] = aux;
                }
            }
        }

        // Exibe o Top 10 (ou todos, se tiver menos que 10)
        gotoxy(20, 6); printf("RANKING | NOME DO JOGADOR         | MOVIMENTOS");
        gotoxy(20, 7); printf("----------------------------------------------");
        for (i = 0; i < qtdJogadores && i < 10; i++) {
            gotoxy(20, 8 + i);
            printf(" %d \t| %-20s \t| %d", i + 1, historico[i].nome, historico[i].movimentos);
        }
    }

    gotoxy(25, 20); printf("Pressione qualquer tecla para voltar...");
    getch();
}

void jogar() {
    char nomeJogador[50];
    char letra;
    int continua = 1;
    int qtde;

    system("cls");
    moldura(1, 23, 1, 80, 0);
    gotoxy(25, 10); printf("Digite o nome do jogador: ");
    fflush(stdin); // Limpa o teclado
    gets(nomeJogador); // Pega o nome com espaços

    // Resetando todas as variáveis para um jogo novo
    reiniciaMapa();
    movimentos = 0;
    fase = 0;
    heroi.vida = 10;
    heroi.atq  = 4;
    heroi.def  = 3;
    heroi.chave = 0;

    system("cls");

    // === DO WHILE EXTERNO (CONTROLA AS FASES) ===
    do {
        heroi.posx = 0;
        heroi.posy = 0;

        geraMonstros(MONSTROS / 3 + 3);
        geraParede(PAREDE/ 3 +3);
        mapa[fase][heroi.posx][heroi.posy] = 'H';

        // === DO WHILE INTERNO (MOVIMENTAÇÃO DA FASE ATUAL) ===
        do {
            qtde = contaMonstro();
            mostraMapa();

            // Exibe movimentos na tela para o jogador ver
            gotoxy(3, 16); printf("Movimentos: %d", movimentos);

            letra = getch();
            gotoxy(3, 17); printf("Tem %d monstros ao redor    ", qtde);
            gotoxy(3, 18); printf("Tecla: [%d]   ", letra);

            continua = movimenta(letra);

        } while (letra != 27 && continua == 1);

       if (heroi.vida > 0 && letra != 27) {
            fase++;
            system("cls");
            if (fase < 3) {
                gotoxy(3, 12); printf(">>> PARABENS! PREPARANDO A FASE %d... <<<", fase + 1);
                Sleep(2000);
                system("cls");
            } else {
                gotoxy(3, 12); printf(">>> PARABENS! VOCE ZEROU O JOGO! <<<");

                // Grava na memoria (RAM)
                strcpy(historico[qtdJogadores].nome, nomeJogador);
                historico[qtdJogadores].movimentos = movimentos;
                qtdJogadores++;

                // === NOVIDADE: SALVA NO ARQUIVO DO COMPUTADOR ===
                salvarHistorico();

                Sleep(2000);
            }
        } else {
            break;
        }
    } while (fase < 3);

    // Se morreu ou apertou ESC
    if (heroi.vida <= 0) {
        system("cls");
        gotoxy(30, 12); printf("VOCE MORREU! FIM DE JOGO.");
        Sleep(2000);
    }
}

int main() {
    char opcao;
    srand(time(NULL));

    // === NOVIDADE: CARREGA O ARQUIVO ASSIM QUE O JOGO ABRE ===
    carregarHistorico();

    do {
        system("cls");
        moldura(1, 23, 1, 80, 0);

        gotoxy(32, 5); printf("=== JOGO RPG ===");

        gotoxy(32, 9);  printf("[1] Iniciar Jogo");
        gotoxy(32, 11); printf("[2] Historico");
        gotoxy(32, 13); printf("[3] Sair");

        gotoxy(32, 17); printf("Escolha: ");
        opcao = getch();

        switch (opcao) {
            case '1':
                jogar();
                break;
            case '2':
                exibirHistorico();
                break;
            case '3':
                system("cls");
                gotoxy(30, 12); printf("Saindo do jogo... Ate logo!");
                Sleep(1500);
                break;
            default:
                gotoxy(32, 19); printf("Opcao Invalida!");
                Sleep(1000);
                break;
        }

    } while (opcao != '3');

    return 0;
}
