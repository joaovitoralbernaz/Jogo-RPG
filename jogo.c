#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

#define RESET "\033[0m"

#define HERO "\033[33;48;5;236m☺ \033[0m"        // amarelo
#define MONSTER "\033[31;48;5;236m☠ \033[0m"     // vermelho
#define EXIT "\033[32;48;5;236m⌂ \033[0m"        // verde
#define WALL "\033[90;48;5;236m■ \033[0m"        // cinza
#define CHEST "\033[38;5;208;48;5;236m♦ \033[0m" // laranja
#define BOSS "\033[35;48;5;236m☣ \033[0m"        // roxo
#define FLOOR "\033[48;5;236m  \033[0m"          // branco

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
    int movimentos = 0;
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
    hero.vida = 150;
    hero.vidaMax = 150;
    hero.atq = 25;
    hero.def = 10;
    hero.danoRecebido = 0;
    hero.danoCausado = 0;
    hero.tempo = 0;
    map[hero.posx][hero.posy] = 'H';
}
void createExit(int sizeMap)
{
    int randCol, randRow;

    do
    {
        randCol = rand() % sizeMap;
        randRow = rand() % sizeMap;
    } while (map[randRow][randCol] != '.');

    map[randRow][randCol] = 'S';
}
void createMonstro(int qtdMonster, int map[][sizeMap], int sizeMap)
{
    int x, y, cont;
    for (cont = 0; cont < qtdMonster; cont++)
    {
        x = rand() % sizeMap;
        y = rand() % sizeMap;

        if (map[x][y] == '.')
        {
            map[x][y] = 'M';
            monster[cont].posx = x;
            monster[cont].posy = y;
            monster[cont].atq = rand() % 5 + 10;
            monster[cont].def = rand() % 5 + 10;
            monster[cont].vida = rand() % 100 + 150;
            monster[cont].vidaMax = monster[cont].vida;
        }
        else
        {
            cont--;
        }
    }
}
void createBoss(int map[][sizeMap], int sizeMap)
{
    int x, y;
    do
    {
        x = rand() % sizeMap;
        y = rand() % sizeMap;
    } while (map[x][y] != '.');

    map[x][y] = 'B';
    boss.posx = x;
    boss.posy = y;
    boss.atq = rand() % 10 + 20;
    boss.def = rand() % 10 + 20;
    boss.vida = rand() % 200 + 300;
    boss.vidaMax = boss.vida;
}
void createChest(int map[][sizeMap], int sizeMap)
{
    int x, y;
    do
    {
        x = rand() % sizeMap;
        y = rand() % sizeMap;
    } while (map[x][y] != '.');

    map[x][y] = 'T';
}
void createMoldura(int rowInicial, int colInicial, int rowFinal, int colFinal, int type)
{
    int l, c;
    int borda[2][6] = {
        {205, 186, 201, 187, 200, 188},
        {196, 179, 218, 191, 192, 217}};

    for (c = colInicial + 1; c < colFinal; c++)
    {
        gotoxy(c, rowInicial);
        printf("%c", borda[type][0]);
        gotoxy(c, rowFinal);
        printf("%c", borda[type][0]);
    }
    for (l = rowInicial + 1; l < rowFinal; l++)
    {
        gotoxy(colInicial, l);
        printf("%c", borda[type][1]);
        gotoxy(colFinal, l);
        printf("%c", borda[type][1]);
    }

    gotoxy(colInicial, rowInicial);
    printf("%c", borda[type][2]);
    gotoxy(colFinal, rowInicial);
    printf("%c", borda[type][3]);
    gotoxy(colInicial, rowFinal);
    printf("%c", borda[type][4]);
    gotoxy(colFinal, rowFinal);
    printf("%c", borda[type][5]);
}
void printMap(int sizeMap)
{
    int qtdMonstros = 0;
    int qtdBoss = 0;

    system("cls");

    for (int i = 0; i < sizeMap; i++)
    {
        for (int j = 0; j < sizeMap; j++)
        {
            char aux = map[i][j];

            int visivel =
                abs(i - hero.posx) <= 1 &&
                abs(j - hero.posy) <= 1;

            if (aux != 'H' && aux != '.' && !visivel)
            {
                printf("%s", FLOOR);
                continue;
            }

            if (visivel)
            {
                if (aux == 'M')
                {
                    qtdMonstros++;
                    printf("%s", FLOOR);
                    continue;
                }

                if (aux == 'B')
                {
                    qtdBoss++;
                    printf("%s", FLOOR);
                    continue;
                }
            }

            switch (aux)
            {
            case 'H':
                printf("%s", HERO);
                break;

            case 'S':
                printf("%s", EXIT);
                break;

            case 'T':
                printf("%s", CHEST);
                break;

            case 'P':
                printf("%s", WALL);
                break;

            default:
                printf("%s", FLOOR);
                break;
            }
        }

        printf("\n");
    }

    printf("\n");

    if (qtdMonstros > 0)
    {
        printf("Voce sente a presenca de %d monstro(s) por perto.\n", qtdMonstros);
    }

    if (qtdBoss > 0)
    {
        printf("Voce sente uma energia poderosa nas proximidades.\n");
    }
}
void drawHPBar(int x, int y, int vida, int vidaMax)
{
    int barSize = 20;

    int filled = (vida * barSize) / vidaMax;

    gotoxy(x, y);

    printf("HP [");

    for (int i = 0; i < barSize; i++)
    {
        if (i < filled)
            printf("\033[32m■\033[0m");
        else
            printf("\033[90m□\033[0m");
    }

    printf("] %3d/%3d", vida, vidaMax);
}
int battle(struct personagem *enemy, char *enemyIcon)
{
    int heroDice;
    int enemyDice;
    int heroTurn;

    heroDice = rand() % 6 + 1;
    enemyDice = rand() % 6 + 1;

    heroTurn = (heroDice >= enemyDice);

    system("cls");

    createMoldura(2, 5, 24, 105, 0);

    gotoxy(45, 4);
    printf("BATALHA!");

    gotoxy(20, 7);
    printf("%s HEROI", HERO);

    gotoxy(75, 7);
    printf("%s INIMIGO", MONSTER);

    gotoxy(20, 10);
    printf("Dado Heroi : %d", heroDice);

    gotoxy(75, 10);
    printf("Dado Inimigo : %d", enemyDice);

    gotoxy(40, 14);

    if (heroTurn)
        printf("VOCE COMECA!");
    else
        printf("O INIMIGO COMECA!");

    getch();

    while (hero.vida > 0 && enemy->vida > 0)
    {
        system("cls");

        createMoldura(2, 5, 24, 105, 0);

        gotoxy(20, 5);
        printf("%s HEROI", HERO);

        gotoxy(75, 5);
        printf("%s INIMIGO", MONSTER);

        drawHPBar(20, 8, hero.vida, hero.vidaMax);
        drawHPBar(75, 8, enemy->vida, enemy->vidaMax);

        gotoxy(20, 11);
        printf("ATQ: %d", hero.atq);

        gotoxy(20, 12);
        printf("DEF: %d", hero.def);

        if (heroTurn)
        {
            int damage = hero.atq + (rand() % 6) - (enemy->def / 2);
            hero.danoCausado = damage;

            if (damage < 1)
                damage = 1;

            enemy->vida -= damage;

            if (enemy->vida < 0)
                enemy->vida = 0;

            gotoxy(20, 16);
            printf("Voce atacou!");

            gotoxy(20, 17);
            printf("Causou %d de dano.", damage);
        }
        else
        {
            int damage = enemy->atq + (rand() % 6) - (hero.def / 2);
            hero.danoRecebido = damage;

            hero.vida -= damage;
            if (damage < 1)
                damage = 1;

            if (hero.vida < 0)
                hero.vida = 0;

            gotoxy(20, 16);
            printf("O inimigo atacou!");

            gotoxy(20, 17);
            printf("Voce recebeu %d de dano.", damage);
        }

        heroTurn = !heroTurn;

        gotoxy(20, 21);
        printf("Pressione qualquer tecla para continuar...");

        getch();
    }

    system("cls");

    createMoldura(7, 25, 17, 85, 0);

    if (hero.vida > 0)
    {
        gotoxy(40, 11);
        printf("%s VOCE VENCEU!", HERO);

        gotoxy(35, 13);
        printf("Inimigo derrotado.");

        getch();

        return 1;
    }
    else
    {
        gotoxy(40, 11);
        printf("%s VOCE MORREU!", MONSTER);

        gotoxy(37, 13);
        printf("Fim de jogo.");

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
        case 72: // ↑
            novoX--;
            break;

        case 80: // ↓
            novoX++;
            break;

        case 75: // ←
            novoY--;
            break;

        case 77: // →
            novoY++;
            break;

        default:
            return 0;
        }
    }
    else
    {
        switch (tolower(tecla))
        {
        case 'w':
            novoX--;
            break;

        case 's':
            novoX++;
            break;

        case 'a':
            novoY--;
            break;

        case 'd':
            novoY++;
            break;

        default:
            return 0;
        }
    }

    if (novoX < 0 || novoX >= sizeMap || novoY < 0 || novoY >= sizeMap)
    {
        return 0;
    }

    if (map[novoX][novoY] == 'P')
    {
        return 0;
    }
    else if (map[novoX][novoY] == 'M')
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
            else
            {
                return -1;
            }
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
        else
        {
            return -1;
        }
    }
    else if (map[novoX][novoY] == 'T')
    {
        int item = rand() % 3;

        switch (item)
        {
        case 0:
            hero.vida += 20;
            if (hero.vida > hero.vidaMax)
                hero.vida = hero.vidaMax;
            printf("Voce encontrou uma poção de vida! HP aumentado em 20.");
            break;

        case 1:
            hero.atq += 5;
            printf("Voce encontrou uma arma! ATQ aumentado em 5.");
            break;

        case 2:
            hero.def += 5;
            printf("Voce encontrou uma armadura! DEF aumentado em 5.");
            break;
        }
        map[novoX][novoY] = '.';
    }
    else if (map[novoX][novoY] == 'S' && boss.vida <= 0)
    {
        return 2;
    }
    else
    {
        printf("A porta esta trancada. Parece que algo poderoso ainda esta vivo por aqui...");
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
    FILE *arq;

    arq = fopen("ranking.txt", "a");

    if (arq == NULL)
    {
        printf("Erro ao abrir ranking.\n");
        return;
    }

    fprintf(arq, "%s;%d;%d;%d;%d\n",
            nome,
            fase,
            hero.tempo,
            hero.danoCausado,
            hero.danoRecebido);

    fclose(arq);
}
int carregarRanking(Ranking rank[])
{
    FILE *arq;
    int qtd = 0;

    arq = fopen("ranking.txt", "r");

    if (arq == NULL)
        return 0;

    while (fscanf(arq,
                  "%29[^;];%d;%d;%d;%d\n",
                  rank[qtd].nome,
                  &rank[qtd].fase,
                  &rank[qtd].tempo,
                  &rank[qtd].danoCausado,
                  &rank[qtd].danoRecebido) == 5)
    {
        qtd++;
    }

    fclose(arq);

    return qtd;
}
void ordenarPorTempo(Ranking rank[], int qtd)
{
    Ranking aux;

    for (int i = 0; i < qtd - 1; i++)
    {
        for (int j = i + 1; j < qtd; j++)
        {
            if (rank[j].tempo < rank[i].tempo)
            {
                aux = rank[i];
                rank[i] = rank[j];
                rank[j] = aux;
            }
        }
    }
}
void ordenarPorDanoCausado(Ranking rank[], int qtd)
{
    Ranking aux;

    for (int i = 0; i < qtd - 1; i++)
    {
        for (int j = i + 1; j < qtd; j++)
        {
            if (rank[j].danoCausado > rank[i].danoCausado)
            {
                aux = rank[i];
                rank[i] = rank[j];
                rank[j] = aux;
            }
        }
    }
}
void ordenarPorDanoRecebido(Ranking rank[], int qtd)
{
    Ranking aux;

    for (int i = 0; i < qtd - 1; i++)
    {
        for (int j = i + 1; j < qtd; j++)
        {
            if (rank[j].danoRecebido < rank[i].danoRecebido)
            {
                aux = rank[i];
                rank[i] = rank[j];
                rank[j] = aux;
            }
        }
    }
}
void mostrarRanking(Ranking rank[], int qtd)
{
    system("cls");

    createMoldura(2, 2, 30, 120, 0);

    gotoxy(55, 4);
    printf("RANKING");

    gotoxy(5, 7);
    printf("POS");

    gotoxy(12, 7);
    printf("NOME");

    gotoxy(35, 7);
    printf("FASE");

    gotoxy(48, 7);
    printf("TEMPO");

    gotoxy(62, 7);
    printf("DANO CAUS.");

    gotoxy(88, 7);
    printf("DANO RECEB.");

    for (int i = 0; i < qtd && i < 15; i++)
    {
        gotoxy(5, 9 + i);
        printf("%d", i + 1);

        gotoxy(12, 9 + i);
        printf("%s", rank[i].nome);

        gotoxy(35, 9 + i);
        printf("%d", rank[i].fase);

        gotoxy(48, 9 + i);
        printf("%d", rank[i].tempo);

        gotoxy(62, 9 + i);
        printf("%d", rank[i].danoCausado);

        gotoxy(88, 9 + i);
        printf("%d", rank[i].danoRecebido);
    }

    gotoxy(40, 28);
    printf("Pressione qualquer tecla para voltar ao menu");

    getch();
}
void menuRanking()
{
    Ranking rank[100];
    int qtd;
    int opcao;

    qtd = carregarRanking(rank);

    system("cls");

    createMoldura(2, 5, 28, 105, 0);

    gotoxy(48, 4);
    printf("RANKING");

    if (qtd == 0)
    {
        gotoxy(35, 14);
        printf("Nenhum registro encontrado.");

        getch();
        return;
    }

    gotoxy(15, 10);
    printf("1 - Ordenar por tempo");

    gotoxy(15, 12);
    printf("2 - Ordenar por dano causado");

    gotoxy(15, 14);
    printf("3 - Ordenar por dano recebido");

    gotoxy(15, 16);
    printf("0 - Voltar");

    gotoxy(15, 20);
    printf("Opcao: ");

    scanf("%d", &opcao);

    switch (opcao)
    {
    case 1:
        ordenarPorTempo(rank, qtd);
        break;

    case 2:
        ordenarPorDanoCausado(rank, qtd);
        break;

    case 3:
        ordenarPorDanoRecebido(rank, qtd);
        break;

    case 0:
        return;

    default:
        gotoxy(15, 23);
        printf("Opcao invalida.");
        getch();
        return;
    }

    mostrarRanking(rank, qtd);
}
void menu()
{
    system("cls");

    createMoldura(2, 20, 20, 90, 0);

    gotoxy(47, 4);
    printf("DUNGEON GAME");

    gotoxy(35, 8);
    printf("1 - Iniciar Jogo");

    gotoxy(35, 10);
    printf("2 - Ranking");

    gotoxy(35, 12);
    printf("3 - Sair");

    gotoxy(35, 16);
    printf("Escolha uma opcao: ");
}
int menuVitoria()
{
    int op;

    system("cls");

    printf("\n===== FASE CONCLUIDA =====\n");
    printf("1 - Continuar jogando\n");
    printf("2 - Voltar ao menu\n");
    printf("Opcao: ");
    scanf("%d", &op);

    return op;
}
int menuMorte()
{
    int op;

    system("cls");

    printf("\n===== VOCE MORREU =====\n");
    printf("1 - Reiniciar fase\n");
    printf("2 - Voltar ao menu\n");
    printf("Opcao: ");
    scanf("%d", &op);

    return op;
}
void carregarFase(int fase)
{

    switch (fase)
    {
    case 1:

        createmap(10);
        createHero();
        createMonstro(3, map, 10);
        createBoss(map, 10);
        createChest(map, 10);
        createExit(10);
        break;

    case 2:
    
        createmap(15);
        createHero();
        createMonstro(5, map, 15);
        createBoss(map, 15);
        createChest(map, 15);
        createChest(map, 15);
        createExit(15);
        break;

    case 3:
    
        createmap(20);
        createHero();
        createMonstro(8, map, 20);
        createBoss(map, 20);
        createChest(map, 20);
        createChest(map, 20);
        createChest(map, 20);
        createExit(20);
        break;
    }
}
void IniciarJogo()
{
    char nome[30];

    system("cls");

    printf("Nome do jogador: ");
    scanf("%29s", nome);

    int faseAtual = 1;

    while (faseAtual <= 3)
    {
        carregarFase(faseAtual);

        int resultado = 0;

        while (1)
        {
            printMap(MAX);

            resultado = moveHero(MAX);

            if (resultado == -1)
                break;

            if (resultado == 2)
                break;
        }

        if (resultado == -1)
        {
            int op = menuMorte();

            if (op == 1)
            {
                continue; // reinicia a mesma fase
            }
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
                printf("\nPARABENS! VOCE ZEROU O JOGO!\n");

                salvarRanking(nome, 3);

                getch();
                return;
            }

            int op = menuVitoria();

            if (op == 1)
            {
                faseAtual++;
            }
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
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    system("chcp 65001 > nul");

    srand(time(NULL));
}

/* Itens a fazer:
ranking em txt
ordem no ranking (por tempo, por dano causado, por dano recebido)
main
menu inicial com opções de iniciar jogo, ver ranking e sair
voltar ao menu inicial apos perder uma fase

*/