#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

#define RESET "\033[0m"
/*
#define HERO    "\033[33;48;5;236m\U0001F9D1\u200D\U0001F4BB\033[0m"  // emoji largura 2
#define MONSTER "\033[31;48;5;236m\u2620 \033[0m"     // ☠ + espaço = largura 2
#define EXIT    "\033[32;48;5;236m\U0001F6AA\033[0m"   // 🚪 largura 2
#define WALL    "\033[90;48;5;236m\u25A0 \033[0m"      // ■ + espaço = largura 2
#define CHEST   "\033[38;5;208;48;5;236m\u2754\033[0m" // ❔ largura 2
#define BOSS    "\033[35;48;5;236m\u2623 \033[0m"       // ☣ + espaço = largura 2
#define FLOOR   "\033[48;5;236m  \033[0m"               // 2 espaços = largura 2
*/

#define HERO    "\033[33;48;5;236mH \033[0m"
#define MONSTER "\033[31;48;5;236mM \033[0m"
#define EXIT    "\033[32;48;5;236mS \033[0m"
#define WALL    "\033[90;48;5;236mW \033[0m"
#define CHEST   "\033[38;5;208;48;5;236mC \033[0m"
#define BOSS    "\033[35;48;5;236mB \033[0m"
#define FLOOR   "\033[48;5;236m  \033[0m"


#define MONSTERS 10

#define MAX_MAP 20
char map[MAX_MAP][MAX_MAP];

int sizeMap;

void gotoxy(int x, int y)
{
    COORD c;
    c.X = x - 1;
    c.Y = y - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

struct personagem
{
    int vida, atq, def, posx, posy, movimento, vidaMax, danoRecebido, danoCausado, tempo;
} hero, monster[MONSTERS], boss;

typedef struct
{
    char nome[30];
    int tempo, fase, danoCausado, danoRecebido;
} Ranking;

void createmap(int sizeMap)
{
    for (int i = 0; i < sizeMap; i++)
    {
        for (int j = 0; j < sizeMap; j++)
        {
            map[i][j] = '.';
        }
    }
}

void createHero()
{
    hero.posx = 0;
    hero.posy = 0;
    hero.vida = 100;
    hero.vidaMax = 100;
    hero.atq = 10;
    hero.def = 10;
    hero.danoRecebido = 0;
    hero.danoCausado = 0;
    hero.tempo = 0;
    map[hero.posx][hero.posy] = 'H';
}

void createExit(int sizeMap)
{
    int x, y;
    do
    {
        x = rand() % sizeMap;
        y = rand() % sizeMap;
    } while (map[y][x] != '.');
    map[y][x] = 'S';
}

void createWall(int qtde ,int sizeMap)
{
     int x, y, cont;
    for (cont = 0; cont < qtde; cont++)
    {
        x = rand() % sizeMap;
        y = rand() % sizeMap;

        if (map[x][y] == '.')
        {
            map[x][y] = 'P';
        }
        else
        {
            cont--;
        }
    }
}

void createMonstro(int qtdMonster, int sizeMap)
{
    int x, y, cont;
    for (cont = 0; cont < qtdMonster; cont++)
    {
        x = rand() % sizeMap;
        y = rand() % sizeMap;

        int minimo = 100;
        int maximo = 200;


        if (map[x][y] == '.')
        {
            map[x][y] = 'M';
            monster[cont].posx = x;
            monster[cont].posy = y;
            monster[cont].atq = rand() % 6;
            monster[cont].def = rand() % 6;
            monster[cont].vida = rand() % (maximo - minimo) + minimo;
            monster[cont].vidaMax = monster[cont].vida;
        }
        else
        {
            cont--;
        }
    }
}

void createBoss(int sizeMap)
{
    int maximo = 400;
    int minimo = 250;
    int x, y;
    do
    {
        x = rand() % sizeMap;
        y = rand() % sizeMap;
    } while (map[x][y] != '.');

    map[x][y] = 'B';
    boss.posx = x;
    boss.posy = y;
    boss.atq = rand() % 15;
    boss.def = rand() % 10;
    boss.vida = rand() % (maximo - minimo + 1) + minimo;
    boss.vidaMax = boss.vida;
}

void createChest(int sizeMap)
{
    int x, y;
    do
    {
        x = rand() % sizeMap;
        y = rand() % sizeMap;
    } while (map[x][y] != '.');
    map[x][y] = 'T';
}

/*void createMoldura(int linhaInicial, int linhaFinal, int colInicial, int colFinal, int tipo) {
    int l, c;
    const char *horiz[2]   = { "\u2550", "\u2500" }; // ═  ─
    const char *vert[2]    = { "\u2551", "\u2502" }; // ║  │
    const char *canto[2][4] = {
        { "\u2554", "\u2557", "\u255A", "\u255D" }, // ╔ ╗ ╚ ╝
        { "\u250C", "\u2510", "\u2514", "\u2518" }  // ┌ ┐ └ ┘
    };

    for (c = colInicial + 1; c <= colFinal - 1; c++) {
        gotoxy(c, linhaInicial); printf("%s", horiz[tipo]);
        gotoxy(c, linhaFinal);   printf("%s", horiz[tipo]);
    }
    for (l = linhaInicial + 1; l <= linhaFinal - 1; l++) {
        gotoxy(colInicial, l); printf("%s", vert[tipo]);
        gotoxy(colFinal,   l); printf("%s", vert[tipo]);
    }
    gotoxy(colInicial, linhaInicial); printf("%s", canto[tipo][0]);
    gotoxy(colFinal,   linhaInicial); printf("%s", canto[tipo][1]);
    gotoxy(colInicial, linhaFinal);   printf("%s", canto[tipo][2]);
    gotoxy(colFinal,   linhaFinal);   printf("%s", canto[tipo][3]);
}
*/

void createMoldura(int linhaInicial, int linhaFinal, int colInicial, int colFinal, int tipo) {
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

void printMap(int sizeMap)
{
    int qtdMonstros = 0;
    int qtdBoss = 0;

    system("cls");

    // Moldura Externa Fixa Compacta
    createMoldura(1, 28, 2, 92, 0);
    createMoldura(3, 5, 39, 54, 1);
    gotoxy(41, 4);  printf("DUNGEON GAME");
    // Cálculos para Centralizar Perfeitamente o Mapa Baseado no tamanho dele
    int outer_width = 90;   // 92 - 2
    int outer_height = 27;  // 28 - 1
    int map_width = (sizeMap * 2) + 2;
    int map_height = sizeMap + 2;

    int colInicial = 2 + (outer_width - map_width) / 2;
    int colFinal = colInicial + map_width - 1;
    int linhaInicial = 1 + (outer_height - map_height) / 2;
    int linhaFinal = linhaInicial + map_height - 1;

    // Moldura Interna adaptada e centralizada para o tamanho do mapa
    createMoldura(linhaInicial, linhaFinal, colInicial, colFinal, 1);

    for (int i = 0; i < sizeMap; i++)
    {
        for (int j = 0; j < sizeMap; j++)
        {
            gotoxy(colInicial + 1 + (j * 2), linhaInicial + 1 + i);

            char aux = map[i][j];
            int visivel = abs(i - hero.posx) <= 1 && abs(j - hero.posy) <= 1;

            if (aux != 'H' && aux != '.' && !visivel)
            {
                printf("%s", FLOOR);
                continue;
            }

            if (visivel)
            {
                if (aux == 'M') { qtdMonstros++; printf("%s", MONSTER); continue; }
                if (aux == 'B') { qtdBoss++; printf("%s", BOSS); continue; }
            }

            switch (aux)
            {
            case 'H': printf("%s", HERO); break;
            case 'S': printf("%s", EXIT); break;
            case 'T': printf("%s", CHEST); break;
            case 'P': printf("%s", WALL); break;
            default:  printf("%s", FLOOR); break;
            }
        }
    }

    int linhaMensagem = linhaFinal + 1;

    if (qtdMonstros > 0 && linhaMensagem < 28)
    {
        gotoxy(colInicial, linhaMensagem);
        printf("Voce sente a presenca de %d monstro(s).", qtdMonstros);
        linhaMensagem++;
    }

    if (qtdBoss > 0 && linhaMensagem < 28)
    {
        gotoxy(colInicial, linhaMensagem);
        printf("Voce sente uma energia poderosa por perto.");
    }

    gotoxy(2, 27);
}

void drawHPBar(int x, int y, int vida, int vidaMax)
{
    int barSize = 12;
    int filled = (vida * barSize) / vidaMax;

    gotoxy(x, y);
    printf("HP [");
    for (int i = 0; i < barSize; i++)
    {
        if (i < filled) printf("\033[32m#\033[0m");
        else printf("\033[90m#\033[0m");
    }
    printf("] %3d/%3d", vida, vidaMax);
}

int battle(struct personagem *enemy, char *enimyIcon)
{
    int heroDice = rand() % 6 + 1;
    int enemyDice = rand() % 6 + 1;
    int heroTurn = (heroDice >= enemyDice);

    system("cls");
    createMoldura(1, 28, 2, 92, 0); // Moldura Externa
    createMoldura(3, 25, 5, 89, 1); // Moldura Interna Mais Estreita

    gotoxy(44, 5);  printf("BATALHA!");
    gotoxy(12, 7);  printf("%s HEROI", HERO);
    gotoxy(54, 7);  printf("%s INIMIGO", enimyIcon);
    gotoxy(12, 9);  printf("Dado Heroi : %d", heroDice);
    gotoxy(54, 9);  printf("Dado Inimigo : %d", enemyDice);

    gotoxy(38, 13);
    if (heroTurn) printf("VOCE COMECA!");
    else          printf("O INIMIGO COMECA!");

    getch();

    while (hero.vida > 0 && enemy->vida > 0)
    {
        system("cls");
        createMoldura(1, 28, 2, 92, 0);
        createMoldura(3, 25, 5, 89, 1);

        drawHPBar(12, 7, hero.vida, hero.vidaMax);
        drawHPBar(54, 7, enemy->vida, enemy->vidaMax);

        gotoxy(12, 10); printf("ATQ: %d  |  DEF: %d", hero.atq, hero.def);
        gotoxy(54, 10); printf("ATQ: %d  |  DEF: %d", enemy->atq, enemy->def);

        if (heroTurn)
        {
            int damage = hero.atq + (rand() % 6) - enemy->def;
            hero.danoCausado += damage;
            if (damage < 1) damage = 0;
            enemy->vida -= damage;
            if (enemy->vida < 0) enemy->vida = 0;

            gotoxy(12, 14); printf("Voce atacou!");
            gotoxy(12, 15); printf("Causou %d de dano.", damage);
        }
        else
        {
            int damage = enemy->atq + (rand() % 6) - hero.def;
            hero.danoRecebido += damage;
            if (damage < 1) damage = 1;
            hero.vida -= damage;
            if (hero.vida < 0) hero.vida = 0;

            gotoxy(12, 14); printf("O inimigo atacou!");
            gotoxy(12, 15); printf("Voce recebeu %d de dano.", damage);
        }

        heroTurn = !heroTurn;

        gotoxy(12, 20);
        printf("Pressione qualquer tecla para continuar...");
        getch();
    }

    system("cls");
    createMoldura(1, 28, 2, 92, 0);
    createMoldura(9, 19, 25, 69, 1);

    if (hero.vida > 0)
    {
        gotoxy(36, 13); printf("%s VOCE VENCEU!", HERO);
        gotoxy(36, 15); printf("Inimigo derrotado.");
        getch();
        return 1;
    }
    else
    {
        gotoxy(36, 13); printf("%s VOCE MORREU!", enimyIcon);
        gotoxy(36, 15); printf("Fim de jogo.");
        getch();
        return 0;
    }
}

int moveHero(int sizeMap)
{
    int tecla = getch();
    int novoX = hero.posx;
    int novoY = hero.posy;

    if (tecla == 0 || tecla == 224)
    {
        tecla = getch();
        switch (tecla)
        {
        case 72: novoX--; break; // ↑
        case 80: novoX++; break; // ↓
        case 75: novoY--; break; // ←
        case 77: novoY++; break; // →
        default: return 0;
        }
    }
    else
    {
        switch (tolower(tecla))
        {
        case 'w': novoX--; break;
        case 's': novoX++; break;
        case 'a': novoY--; break;
        case 'd': novoY++; break;
        default: return 0;
        }
    }

    if (novoX < 0 || novoX >= sizeMap || novoY < 0 || novoY >= sizeMap) return 0;
    if (map[novoX][novoY] == 'P') return 0;

    if (map[novoX][novoY] == 'M')
    {
        int monsterIndex = -1;
        for (int i = 0; i < MONSTERS; i++)
        {
            if (monster[i].posx == novoX && monster[i].posy == novoY)
            {
                monsterIndex = i;
                break;
            }
        }

        if (monsterIndex != -1)
        {
            int resultadoBatalha = battle(&monster[monsterIndex], MONSTER);
            if (resultadoBatalha == 1)
            {
                map[novoX][novoY] = '.';
                monster[monsterIndex].vida = 0;
            }
            else return -1;
        }
    }
    else if (map[novoX][novoY] == 'B')
    {
        int resultadoBatalha = battle(&boss, BOSS);
        if (resultadoBatalha == 1)
        {
            map[novoX][novoY] = '.';
            boss.vida = 0;
        }
        else return -1;
    }
    else if (map[novoX][novoY] == 'T')
    {
        int item = rand() % 3;

        system("cls");
        createMoldura(1, 28, 2, 92, 0);
        createMoldura(9, 19, 15, 79, 1);

        gotoxy(41, 11); printf("BAU ABERTO!");

        switch (item)
        {
        case 0:
            hero.vida += 20;
            if (hero.vida > hero.vidaMax) hero.vida = hero.vidaMax;
            gotoxy(20, 14); printf("Voce encontrou uma pocao de vida!  HP +20");
            break;
        case 1:
            hero.atq += 5;
            gotoxy(20, 14); printf("Voce encontrou uma arma!           ATQ +5");
            break;
        case 2:
            hero.def += 5;
            gotoxy(20, 14); printf("Voce encontrou uma armadura!       DEF +5");
            break;
        }

        gotoxy(20, 16); printf("Pressione qualquer tecla para continuar...");
        getch();
        map[novoX][novoY] = '.';
    }
    else if (map[novoX][novoY] == 'S')
    {
        if (boss.vida <= 0) return 2;

        system("cls");
        createMoldura(1, 28, 2, 92, 0);
        createMoldura(11, 17, 25, 69, 1);
        gotoxy(35, 14); printf("A porta esta trancada...");
        getch();
        return 0;
    }

    map[novoX][novoY] = '.';
    map[hero.posx][hero.posy] = '.';

    hero.posx = novoX;
    hero.posy = novoY;
    map[hero.posx][hero.posy] = 'H';

    return 0;
}

void salvarRanking(char nome[], int fase)
{
    FILE *arq = fopen("ranking.txt", "a");
    if (arq == NULL) return;
    fprintf(arq, "%s;%d;%d;%d;%d\n", nome, fase, hero.tempo, hero.danoCausado, hero.danoRecebido);
    fclose(arq);
}

int carregarRanking(Ranking rank[])
{
    FILE *arq = fopen("ranking.txt", "r");
    if (arq == NULL) return 0;
    int qtd = 0;
    while (fscanf(arq, "%29[^;];%d;%d;%d;%d\n", rank[qtd].nome, &rank[qtd].fase, &rank[qtd].tempo, &rank[qtd].danoCausado, &rank[qtd].danoRecebido) == 5)
    {
        qtd++;
    }
    fclose(arq);
    return qtd;
}

void ordenarPorTempo(Ranking rank[], int qtd)
{
    Ranking aux;
    for (int i = 0; i < qtd - 1; i++) {
        for (int j = i + 1; j < qtd; j++) {
            if (rank[j].tempo < rank[i].tempo) { aux = rank[i]; rank[i] = rank[j]; rank[j] = aux; }
        }
    }
}

void ordenarPorDanoCausado(Ranking rank[], int qtd)
{
    Ranking aux;
    for (int i = 0; i < qtd - 1; i++) {
        for (int j = i + 1; j < qtd; j++) {
            if (rank[j].danoCausado > rank[i].danoCausado) { aux = rank[i]; rank[i] = rank[j]; rank[j] = aux; }
        }
    }
}

void ordenarPorDanoRecebido(Ranking rank[], int qtd)
{
    Ranking aux;
    for (int i = 0; i < qtd - 1; i++) {
        for (int j = i + 1; j < qtd; j++) {
            if (rank[j].danoRecebido < rank[i].danoRecebido) { aux = rank[i]; rank[i] = rank[j]; rank[j] = aux; }
        }
    }
}

void mostrarRanking(Ranking rank[], int qtd)
{
    system("cls");
    createMoldura(1, 28, 2, 92, 0);
    createMoldura(3, 26, 4, 90, 1);

    gotoxy(44, 4);  printf("RANKING");
    gotoxy(6, 6);   printf("POS");
    gotoxy(12, 6);  printf("NOME");
    gotoxy(32, 6);  printf("FASE");
    gotoxy(44, 6);  printf("TEMPO");
    gotoxy(56, 6);  printf("DANO CAUS.");
    gotoxy(72, 6);  printf("DANO RECEB.");

    for (int i = 0; i < qtd && i < 15; i++)
    {
        gotoxy(6, 8 + i);   printf("%d", i + 1);
        gotoxy(12, 8 + i);  printf("%s", rank[i].nome);
        gotoxy(32, 8 + i);  printf("%d", rank[i].fase);
        gotoxy(44, 8 + i);  printf("%d", rank[i].tempo);
        gotoxy(56, 8 + i);  printf("%d", rank[i].danoCausado);
        gotoxy(72, 8 + i);  printf("%d", rank[i].danoRecebido);
    }

    gotoxy(26, 25); printf("Pressione qualquer tecla para voltar ao menu");
    getch();
}

void menuRanking()
{
    Ranking rank[100];
    int qtd = carregarRanking(rank);
    int opcao;

    system("cls");
    createMoldura(1, 28, 2, 92, 0);
    createMoldura(5, 23, 20, 74, 1);

    gotoxy(43, 7); printf("RANKING");

    if (qtd == 0)
    {
        gotoxy(32, 14); printf("Nenhum registro encontrado.");
        getch();
        return;
    }

    gotoxy(26, 11); printf("1 - Ordenar por tempo");
    gotoxy(26, 13); printf("2 - Ordenar por dano causado");
    gotoxy(26, 15); printf("3 - Ordenar por dano recebido");
    gotoxy(26, 17); printf("0 - Voltar");
    gotoxy(26, 20); printf("Opcao: ");
    scanf("%d", &opcao);

    switch (opcao)
    {
    case 1: ordenarPorTempo(rank, qtd); break;
    case 2: ordenarPorDanoCausado(rank, qtd); break;
    case 3: ordenarPorDanoRecebido(rank, qtd); break;
    case 0: return;
    default:
        gotoxy(26, 22); printf("Opcao invalida.");
        getch();
        return;
    }
    mostrarRanking(rank, qtd);
}

void menu()
{
    system("cls");
    createMoldura(1, 28, 2, 92, 0);
    createMoldura(6, 22, 25, 69, 1);

    gotoxy(41, 9);  printf("DUNGEON GAME");
    gotoxy(36, 12); printf("1 - Iniciar Jogo");
    gotoxy(36, 14); printf("2 - Ranking");
    gotoxy(36, 16); printf("3 - Sair");
    gotoxy(36, 19); printf("Escolha uma opcao: ");
}

int menuVitoria()
{
    int op;
    system("cls");
    createMoldura(1, 28, 2, 92, 0);
    createMoldura(9, 21, 25, 69, 1);

    gotoxy(40, 11); printf("FASE CONCLUIDA");
    gotoxy(33, 14); printf("1 - Continuar jogando");
    gotoxy(33, 16); printf("2 - Voltar ao menu");
    gotoxy(33, 19); printf("Opcao: ");
    op = getch() - '0';
    return op;
}

int menuMorte()
{
    int op;
    system("cls");
    createMoldura(1, 28, 2, 92, 0);
    createMoldura(9, 21, 25, 69, 1);

    gotoxy(41, 11); printf("VOCE MORREU");
    gotoxy(35, 14); printf("1 - Reiniciar fase");
    gotoxy(35, 16); printf("2 - Voltar ao menu");
    gotoxy(35, 19); printf("Opcao: ");
    op = getch() - '0';
    return op;
}

void carregarFase(int fase)
{
    switch (fase)
    {
    case 1:
        sizeMap = 10;
        createmap(sizeMap);
        createHero();
        createWall(20, sizeMap);
        createMonstro(3, sizeMap);
        createBoss(sizeMap);
        createChest(sizeMap);
        createExit(sizeMap);
        break;
    case 2:
        sizeMap = 15;
        createmap(sizeMap);
        createHero();
        createWall(30,sizeMap);
        createMonstro(5, sizeMap);
        createBoss(sizeMap);
        createChest(sizeMap);
        createChest(sizeMap);
        createExit(sizeMap);
        break;
    case 3:
        sizeMap = 20;
        createmap(sizeMap);
        createHero();
        createWall(35,sizeMap);
        createMonstro(8, sizeMap);
        createBoss(sizeMap);
        createChest(sizeMap);
        createChest(sizeMap);
        createChest(sizeMap);
        createExit(sizeMap);
        break;
    }
}

void IniciarJogo()
{
    char nome[30];
    system("cls");
    createMoldura(1, 28, 2, 92, 0);
    createMoldura(11, 17, 25, 69, 1);

    gotoxy(32, 14); printf("Nome do jogador: ");
    scanf("%29s", nome);

    int faseAtual = 1;

    while (faseAtual <= 3)
    {
        carregarFase(faseAtual);
        int resultado = 0;

        while (1)
        {
            printMap(sizeMap);
            resultado = moveHero(sizeMap);
            if (resultado == -1 || resultado == 2) break;
        }

        if (resultado == -1)
        {
            int op = menuMorte();
            if (op == 1) continue;
            else
            {
                salvarRanking(nome, faseAtual);
                return;
            }
        }

        if (resultado == 2)
        {
            if (faseAtual == 3)
            {
                system("cls");
                createMoldura(1, 28, 2, 92, 0);
                createMoldura(11, 17, 25, 69, 1);
                gotoxy(33, 14); printf("PARABENS! VOCE ZEROU O JOGO!");
                salvarRanking(nome, 3);
                getch();
                return;
            }

            int op = menuVitoria();
            if (op == 1) faseAtual++;
            else
            {
                salvarRanking(nome, faseAtual);
                return;
            }
        }
    }
}

int main()
{
    /*SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    system("chcp 65001 > nul");*/

    srand(time(NULL));
    int opcao;

    do
    {
        menu();
        opcao = getch() - '0';

        switch (opcao)
        {
        case 1: IniciarJogo(); break;
        case 2: menuRanking(); break;
        case 3:
            system("cls");
            createMoldura(1, 28, 2, 92, 0);
            createMoldura(11, 17, 30, 64, 1);
            gotoxy(39, 14); printf("Saindo do jogo...\n");
            break;
        default:
            system("cls");
            createMoldura(1, 28, 2, 92, 0);
            createMoldura(11, 17, 30, 64, 1);
            gotoxy(40, 14); printf("Opcao invalida!\n");
            getch();
            break;
        }
    } while (opcao != 3);

    return 0;
}
