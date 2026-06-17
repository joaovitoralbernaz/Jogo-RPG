#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <conio.h>
#define TAM 7
#define MONSTROS 1

char mapa[TAM][TAM] = {{'H','.','.','.','.','.','.'},
                       {'.','.','.','.','.','.','.'},
                       {'.','.','.','.','.','.','.'},
                       {'.','.','.','.','.','.','.'},
                       {'.','.','.','.','.','.','.'},
                       {'.','.','.','.','.','.','.'},
                       {'.','.','.','.','.','.','S'}
};

struct personagem{
    int vida,atq,def,posx,posy;

}heroi,monstros[MONSTROS];

//int heroi.posx, heroi.posy;

void gotoxy(int x, int y){
    COORD c;
    c.X = x - 1;
    c.Y = y - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void moldura(int linhaInicial, int linhaFinal, int colInicial, int colFinal, int tipo){
    int l, c;
    int borda[2][6] = {{205, 186, 201, 187, 200, 188},
                      {196, 179, 218, 191, 192, 217}};
    for(c = colInicial + 1; c <= colFinal - 1; c++){
        gotoxy(c, linhaInicial); printf("%c", borda[tipo][0]);
        gotoxy(c, linhaFinal); printf("%c", borda[tipo][0]);
    }
    for(l = linhaInicial+1; l <= linhaFinal -1; l++){
        gotoxy(colInicial, l); printf("%c", borda[tipo][1]);
        gotoxy(colFinal, l); printf("%c", borda[tipo][1]);
    }
    gotoxy(colInicial, linhaInicial); printf("%c", borda[tipo][2]);
    gotoxy(colFinal, linhaInicial); printf("%c", borda[tipo][3]);
    gotoxy(colInicial, linhaFinal); printf("%c", borda[tipo][4]);
    gotoxy(colFinal, linhaFinal); printf("%c", borda[tipo][5]);
}

void mostraMapa(){
    int l, c, aux;
    aux = (80 - TAM - 2)/2;
    moldura(1, 23, 1, 80, 0);
    moldura(9, 17, aux, aux+TAM+1, 1);
    for(l = 0; l < TAM; l++){
        for(c = 0; c < TAM; c++){
            gotoxy(aux+1+c, 10+l);
            /*if(mapa[l][c]=='M'){
                printf(".");

        }*/
            printf("%c", mapa[l][c]);
        }
    }
}

void geraMonstros(int qtde){
    int x, y, cont;
    for(cont = 0; cont <= qtde; cont++){
    x = rand()%TAM;
    y = rand()%TAM;
    if(mapa[x][y] == '.'){
    mapa[x][y] = 'M';
    monstros[cont].posx = x;
    monstros[cont].posy = y;
    monstros[cont].atq = 2;
    monstros[cont].def = 1;
    monstros[cont].vida = 7;
    }
    else{
        cont--;
    }
    }
}

void rolarDados(int m){
    int dado, atq, dano;
    dado = rand()%2;
    if(dado == 0){//heroi começa
        atq = rand()%6 + heroi.atq;
        dano = atq - monstros[m].def;
        if(dano > 0)
            monstros[m].vida -=dano;
    }
}

/*void batalha(int x, int y){
    int m;
    for(m = 0; m < MONSTROS; m++){
        if(monstros[0].posx == x && monstros[0].posy == y){
            rolarDados(m);

        }
    }

    //printf("INIMIGO!");
}
*/
void movimenta(char letra){
    mapa[heroi.posx][heroi.posy] = '.';
    if(letra == 's' || letra == 80){
        if(heroi.posx < TAM-1 && mapa[heroi.posx+1][heroi.posy] == '.'){
            heroi.posx++;
        }
    }
    else{
        if(letra == 'w' || letra == 72 && mapa[heroi.posx-1][heroi.posy] == '.'){
            if(heroi.posx > 0){
                heroi.posx--;
            }
        }
        else{
            if(mapa[heroi.posx+1][heroi.posy] == 'M'){
                //batalha(heroi.posx+1,heroi.posy);
            }
        }
    }
     if(letra == 'd' || letra == 77){
        if(heroi.posy< TAM-1 && mapa[heroi.posx][heroi.posy+1] == '.'){
            heroi.posy++;
        }
    }
    else{
        if(letra == 'a' || letra == 75 && mapa[heroi.posx][heroi.posy-1] == '.'){
            if(heroi.posy > 0){
                heroi.posy--;
            }
        }
        else{
            if(mapa[heroi.posx][heroi.posy+1] == 'M'){
               // batalha();
            }
        }
    }
    mapa[heroi.posx][heroi.posy] = 'P';
}

/*int contaMonstros(){
    int cont = 0, x, y;
    for(x = -1; x <= 1; x++){
        for(y = -1; y <= 1; y++){

            int px = heroi.posx + x;
            int py = heroi.posy + y;

            if(mapa[px][py]=='M')
                cont++;
        }
    }
    return cont;

}
*/
int main(){
    char nomeJogo[] = "JOGO RPG!";
    char letra;
    srand(time(NULL));
    heroi.posx = heroi.posy = 0;
    heroi.vida = 10;
    heroi.atq = 4;
    heroi.def = 3;
    strlen(nomeJogo);
    gotoxy((80-strlen(nomeJogo))/2, 5);
    printf("%s", nomeJogo);
    geraMonstros(MONSTROS);
   // int qtde = contaMonstros();
   // gotoxy(10,20);printf("Tem %d monstros" , qtde);
    do{
    mostraMapa();
    letra = getch();
    gotoxy(10, 18); printf("[%d]", letra);
    movimenta(letra);
    }while(letra != 27);
    getch();
}
