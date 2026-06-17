#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <conio.h>

#define TAM 7
#define MONSTROS 10

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
    int vida, atq, def, posx, posy, chave, movimento;
} heroi, monstros[MONSTROS];

int movimentos = 0, fase = 0;

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
            printf("%c", mapa[l][c]);
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
        if (mapa[x][y] == '.') {
            mapa[x][y] = 'M';
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
 * rolarDados: mesma logica do main.c para decidir quem ataca - sorteia
 * um unico dado (0 ou 1) por turno. Se dado == 0, o heroi ataca; senao,
 * o monstro ataca. Aqui o sorteio se repete a cada turno para que a
 * batalha continue ate um dos dois ficar com vida <= 0.
 * Retorna 1 se o heroi venceu, 0 se o monstro venceu.
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
                mapa[x][y] = '.';            /* remove 'M' do mapa    */
                monstros[m].vida = 0;        /* marca como morto      */
                return 1;
            }
            return 0;   /* heroi perdeu */
        }
    }
    return 1;   /* nao encontrou monstro (nao deve acontecer) */
}



/*
 * movimenta: mesmo estilo do main.c (checa direto o tile vizinho em
 * mapa[][], sem calcular novo_x/novo_y antes). A diferenca e que aqui
 * a batalha continua sendo chamada quando o tile vizinho e 'M'.
 */
int movimenta(char letra) {
    int continua = 1;

    mapa[heroi.posx][heroi.posy] = '.';   /* apaga o 'P' da posicao atual */

    if (letra == 's' || letra == 80) {                 /* baixo */
        if (heroi.posx < TAM - 1) {
            char tile = mapa[heroi.posx + 1][heroi.posy];
            if (tile == '.') {
                heroi.posx++;
            } else if (tile == 'M') {
                if (batalha(heroi.posx + 1, heroi.posy)) heroi.posx++;
                else continua = 0;
            } else if (tile == 'S') {
                gotoxy(3, 22); printf(">>> Voce chegou a saida! VITORIA! <<<");
                continua = 0;
            }
        }
    }
    else if (letra == 'w' || letra == 72) {             /* cima */
        if (heroi.posx > 0) {
            char tile = mapa[heroi.posx - 1][heroi.posy];
            if (tile == '.') {
                heroi.posx--;
            } else if (tile == 'M') {
                if (batalha(heroi.posx - 1, heroi.posy)) heroi.posx--;
                else continua = 0;
            } else if (tile == 'S') {
                gotoxy(3, 22); printf(">>> Voce chegou a saida! VITORIA! <<<");
                continua = 0;
            }
        }
    }

    if (letra == 'd' || letra == 77) {                  /* direita */
        if (heroi.posy < TAM - 1) {
            char tile = mapa[heroi.posx][heroi.posy + 1];
            if (tile == '.') {
                heroi.posy++;
            } else if (tile == 'M') {
                if (batalha(heroi.posx, heroi.posy + 1)) heroi.posy++;
                else continua = 0;
            } else if (tile == 'S') {
                gotoxy(3, 22); printf(">>> Voce chegou a saida! VITORIA! <<<");
                continua = 0;
            }
        }
    }
    else if (letra == 'a' || letra == 75) {              /* esquerda */
        if (heroi.posy > 0) {
            char tile = mapa[heroi.posx][heroi.posy - 1];
            if (tile == '.') {
                heroi.posy--;
            } else if (tile == 'M') {
                if (batalha(heroi.posx, heroi.posy - 1)) heroi.posy--;
                else continua = 0;
            } else if (tile == 'S') {
                gotoxy(3, 22); printf(">>> Voce chegou a saida! VITORIA! <<<");
                continua = 0;
            }
        }
    }

    mapa[heroi.posx][heroi.posy] = 'P';   /* redesenha o 'P' na posicao final */

    return continua;
}

int contaMonstro(){
    int cont = 0,x,y;
    for(x=-1; x<=1; x++){
        for(y=-1;x<1;x++){
            int px = heroi.posx + 1;
            int py = heroi.posy + 1;

            if(mapa[px][py] == 'M'){
                cont++;
            }

        }
    }

    return 1;
}

int main() {
    char nomeJogo[] = "JOGO RPG!";
    char letra;
    int continua = 1;
    int qtde = contaMonstro();
    srand(time(NULL));
    system("cls");

    heroi.posx = 0;
    heroi.posy = 0;
    heroi.vida = 10;
    heroi.atq  = 4;
    heroi.def  = 3;
    heroi.chave = 0;
    heroi.movimento = 0;

    /* BUG CORRIGIDO: antes strlen era chamado e resultado ignorado */
    int tamanhoNome = strlen(nomeJogo);
    gotoxy((80 - tamanhoNome) / 2, 5);
    printf("%s", nomeJogo);

    geraMonstros(MONSTROS/3 + 3);
    mapa[heroi.posx][heroi.posy] = 'P';

    do {
        mostraMapa();
        letra = getch();
        gotoxy(3,17);printf("Tem %d monstros" , qtde);
        gotoxy(3, 18); printf("Tecla: [%d]   ", letra);
        continua = movimenta(letra);
    } while (letra != 27 && continua ==1 );   /* ESC ou fim de jogo */

    gotoxy(3, 23); printf("Pressione qualquer tecla para sair...");
    getch();
    return 0;
}
