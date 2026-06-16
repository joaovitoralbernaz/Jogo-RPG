#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <conio.h>

#define TAM 7
#define MONSTROS 3

char mapa[TAM][TAM] = {
    {'H', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', 'S'}
};

struct personagem {
    int vida, atq, def, posx, posy;
} heroi, monstros[MONSTROS];

void gotoxy(int x, int y) {
    COORD c;
    c.X = x - 1;
    c.Y = y - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
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
/*  EXIBIÇÃO                                                          */
/* ------------------------------------------------------------------ */

void mostraMapa() {
    int l, c;
    int aux = (80 - TAM - 2) / 2;

    moldura(1, 23, 1, 80, 0);
    moldura(9, 17, aux, aux + TAM + 1, 1);

    for (l = 0; l < TAM; l++) {
        for (c = 0; c < TAM; c++) {
            gotoxy(aux + 1 + c, 10 + l);
            printf("%c", mapa[l][c]);
        }
    }

    /* painel de status do herói */
    gotoxy(3, 19); printf("HP Heroi: %d   ATQ: %d   DEF: %d   ",
                          heroi.vida, heroi.atq, heroi.def);
}

/* ------------------------------------------------------------------ */
/*  GERAÇÃO DE MONSTROS                                                 */
/* ------------------------------------------------------------------ */

void geraMonstros(int qtde) {
    int x, y, cont;
    for (cont = 0; cont < qtde; cont++) {
        x = rand() % TAM;
        y = rand() % TAM;
        if (mapa[x][y] == '.') {
            mapa[x][y] = 'M';
            monstros[cont].posx = x;
            monstros[cont].posy = y;
            monstros[cont].atq  = 2;
            monstros[cont].def  = 1;
            monstros[cont].vida = 7;
        } else {
            cont--;   /* posição ocupada — tenta de novo */
        }
    }
}

/* ------------------------------------------------------------------ */
/*  BATALHA                                                             */
/* FIX: rolarDados estava incompleta; batalha usava índice fixo [0]    */
/* ------------------------------------------------------------------ */

/*
 * rolarDados: decide quem ataca primeiro e resolve o combate.
 * Retorna 1 se o herói venceu, 0 se o monstro venceu.
 *
 * Regra simples de turno:
 *   - Sorteia 0 ou 1; quem tirar 1 ataca primeiro.
 *   - Dano = ATQ do atacante - DEF do defensor (mínimo 1).
 *   - Turnos se alternam até um dos dois chegar a vida <= 0.
 */
int rolarDados(int m) {
    int dado_heroi  = rand() % 2;
    int dado_monstro = rand() % 2;
    int dano;

    gotoxy(3, 20); printf("== BATALHA INICIADA! ==              ");
    Sleep(600);

    /* Herói começa se tirou >= monstro (critério de desempate pró-herói) */
    while (heroi.vida > 0 && monstros[m].vida > 0) {

        if (dado_heroi >= dado_monstro) {
            /* herói ataca */
            dano = heroi.atq - monstros[m].def;
            if (dano < 1) dano = 1;
            monstros[m].vida -= dano;
            gotoxy(3, 21);
            printf("Heroi atacou! Dano: %d  | HP Monstro: %d   ", dano, monstros[m].vida);
        } else {
            /* monstro ataca */
            dano = monstros[m].atq - heroi.def;
            if (dano < 1) dano = 1;
            heroi.vida -= dano;
            gotoxy(3, 21);
            printf("Monstro atacou! Dano: %d | HP Heroi: %d    ", dano, heroi.vida);
        }
        Sleep(500);

        /* alterna quem vai primeiro no próximo turno */
        dado_heroi   = rand() % 2;
        dado_monstro = rand() % 2;
    }

    if (heroi.vida <= 0) {
        gotoxy(3, 22); printf(">>> Heroi foi derrotado! <<<          ");
        return 0;   /* herói morreu */
    }

    gotoxy(3, 22); printf(">>> Monstro derrotado! <<<            ");
    return 1;   /* herói venceu */
}

/*
 * batalha: localiza qual monstro está em (x, y) e inicia o combate.
 * FIX: antes usava índice fixo monstros[0]; agora usa a variável m.
 * Após a vitória do herói o monstro é removido do mapa.
 * Retorna 1 se herói venceu, 0 se perdeu.
 */
int batalha(int x, int y) {
    int m;
    for (m = 0; m < MONSTROS; m++) {
        if (monstros[m].vida > 0 &&          /* monstro ainda vivo    */
            monstros[m].posx == x &&
            monstros[m].posy == y) {

            if (rolarDados(m)) {             /* herói venceu          */
                mapa[x][y] = '.';            /* remove 'M' do mapa    */
                monstros[m].vida = 0;        /* marca como morto      */
                return 1;
            }
            return 0;   /* herói perdeu */
        }
    }
    return 1;   /* não encontrou monstro (não deve acontecer) */
}

/* ------------------------------------------------------------------ */
/*  MOVIMENTAÇÃO                                                        */
/* FIX: if/else encadeado impedia que d/a funcionassem depois de s/w.  */
/*       Cada direção agora é um if independente.                      */
/* ------------------------------------------------------------------ */

int movimenta(char letra) {
    int novo_x = heroi.posx;
    int novo_y = heroi.posy;

    /* calcula posição desejada */
    if      (letra == 's' || letra == 80) novo_x++;
    else if (letra == 'w' || letra == 72) novo_x--;

    if      (letra == 'd' || letra == 77) novo_y++;
    else if (letra == 'a' || letra == 75) novo_y--;

    /* verifica limites do mapa */
    if (novo_x < 0 || novo_x >= TAM || novo_y < 0 || novo_y >= TAM)
        return 1;   /* continua o jogo */

    char destino = mapa[novo_x][novo_y];

    if (destino == '.') {
        /* movimento livre */
        mapa[heroi.posx][heroi.posy] = '.';
        heroi.posx = novo_x;
        heroi.posy = novo_y;
        mapa[heroi.posx][heroi.posy] = 'P';

    } else if (destino == 'M') {
        /* encontrou monstro — batalha */
        if (!batalha(novo_x, novo_y)) {
            /* herói morreu */
            return 0;
        }
        /* herói sobreviveu — entra na casa do monstro derrotado */
        mapa[heroi.posx][heroi.posy] = '.';
        heroi.posx = novo_x;
        heroi.posy = novo_y;
        mapa[heroi.posx][heroi.posy] = 'P';

    } else if (destino == 'S') {
        /* chegou à saída */
        gotoxy(3, 22); printf(">>> Voce chegou a saida! VITORIA! <<<");
        return 0;   /* encerra o jogo */
    }
    /* 'H' (posição inicial) ou qualquer outro char: ignora */

    return 1;   /* continua o jogo */
}

/* ------------------------------------------------------------------ */
/*  MAIN                                                                */
/* FIX: strlen(nomeJogo) agora é usado corretamente no cálculo.        */
/* ------------------------------------------------------------------ */

int main() {
    char nomeJogo[] = "JOGO RPG!";
    char letra;
    int continua = 1;

    srand(time(NULL));
    system("cls");

    heroi.posx = 0;
    heroi.posy = 0;
    heroi.vida = 10;
    heroi.atq  = 4;
    heroi.def  = 3;

    /* BUG CORRIGIDO: antes strlen era chamado e resultado ignorado */
    int tamanhoNome = strlen(nomeJogo);
    gotoxy((80 - tamanhoNome) / 2, 5);
    printf("%s", nomeJogo);

    geraMonstros(MONSTROS);
    mapa[heroi.posx][heroi.posy] = 'P';

    do {
        mostraMapa();
        letra = getch();
        gotoxy(3, 18); printf("Tecla: [%d]   ", letra);
        continua = movimenta(letra);
    } while (letra != 27 && continua);   /* ESC ou fim de jogo */

    gotoxy(3, 23); printf("Pressione qualquer tecla para sair...");
    getch();
    return 0;
}
