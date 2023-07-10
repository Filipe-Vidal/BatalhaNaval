#include <iostream>
#define tabMax 22
#include <cstdlib>
#include <ctime>
#if defined(__linux__)
    #define plat 'l'
#else
    #define plat 'w'
#endif

using namespace std;

struct posicao
{
    int lin, col;
};

struct navio
{
    //primeira posição e última posição do navio
    posicao posini, posult;
    bool vert;
    int acertos=0, tam;
};

//Adiciona cor ao texto
string cor(char fundoTexto[50], string texto)
{
    if(fundoTexto=="bra-az")
        return (string)"\x1B[94;107m"+texto+(string)"\x1B[30;107m";
    if(fundoTexto=="bra-verm")
        return (string)"\x1B[91;107m"+texto+(string)"\x1B[30;107m";
    if(fundoTexto=="bra-verd")
        return (string)"\x1B[92;107m"+texto+(string)"\x1B[30;107m";
}

//Dá um pause no programa, esperando o usuário inserir qualquer caractere, e depois limpa a tela
void pausaLimpa(bool pausa=true, char texto[100] = "Pressione enter para continuar.")
{
    if(pausa){
        cout << texto;
        cin.get();
    }
    if(plat=='l')
        system("clear");
    else
        system("cls");

}

// Imprime o tabuleiro de forma organizada
// 0: água intocada, representado por "-"
// 1: água em que foi arremessada uma bomba, representada por "O"
// 2: parte de navio explodida, representado por "X"
void imprimeTab(int mat[tabMax][tabMax], int tam)
{
    cout << "\x1B[30;107m" << "   ";
    for(int i=0; i<tam-2; i++){
        cout << (char)('A' + i) << " ";
    }
    cout << "\x1B[30;107m";

    cout << endl;
    for(int i=1; i<tam-1; i++){
        cout << "\x1B[30;107m";
        if(i<10)
            cout << "0" << i << " ";
        else
            cout << i << " ";
        cout << "\x1B[30;107m";

        for(int j=1; j<tam-1; j++){
            if(mat[i][j]==0){
                if(j==tam-1)
                    cout << cor("bra-az", "-");
                else
                    cout << cor("bra-az", "- ");
                //cout << i << "-" << j << " ";
            }
            if(mat[i][j]==1){
                if(j==tam-1)
                    cout << cor("bra-verm", "*");
                else
                    cout << cor("bra-verm", "* ");
            }
            if(mat[i][j]==2){
                if(j==tam-1)
                    cout << cor("bra-verd", "X");
                else
                    cout << cor("bra-verd", "X ");
            }
        }
        cout << endl;
    }
}

// Preenche toda a matriz com 0
void preencheTab(int mat[tabMax][tabMax])
{
    for(int i=0; i<tabMax; i++)
        for(int j=0; j<tabMax; j++)
            mat[i][j] = 0;
}

// Posiciona o navio de um dado tamanho no tabuleiro
void posicionaNav(int mat[tabMax][tabMax], int tamNav, posicao pos, bool vert=false)
{
    if(vert)
        for(int i=0; i<tamNav; i++)
            mat[pos.lin+i][pos.col] = 2;
    else
        for(int i=0; i<tamNav; i++)
            mat[pos.lin][pos.col+i] = 2;
}

// Verifica o entorno de um ponto, se há um navio próximo retorna true, caso contrário retorna false
bool verificaEntorno(posicao pos, int tamNav, int mat[tabMax][tabMax], bool vert, int tamTab)
{
    if(vert){
        for(int i=0; i<tamNav+2; i++){
            for(int j=0; j<3; j++){
                if(mat[pos.lin-1+i][pos.col-1+j]!=0 or pos.lin-1+i>tamTab-2)
                    return true;
            }
        }
    }
    else{
        for(int i=0; i<3; i++){
            for(int j=0; j<tamNav+2; j++){
                if(mat[pos.lin+i-1][pos.col+j-1]!=0 or pos.col-1+j>tamTab-2)
                    return true;
            }
        }
    }
    return false;
}

// Gera um navio em uma posição aleatória, em uma direção aleatória, respeitando a distância de outros navios
void geraNavio(int tamNav, int mat[tabMax][tabMax], int tamTab)
{
    bool vert = rand()%2;
    posicao pos;

    do{
        pos.lin = (rand()%(tamTab-2))+1;
        pos.col = (rand()%(tamTab-2))+1;
    }while(verificaEntorno(pos, tamNav, mat, vert, tamTab));
    posicionaNav(mat, tamNav, pos, vert);
}

//Gera o tabuleiro conforme a dificuldade escolhida
void geraTab(int mat[tabMax][tabMax], int tamTab, int n1, int n2, int n3, int n4)
{
    preencheTab(mat);
    for(int j=0; j<n4; j++){
        geraNavio(4, mat, tamTab);
    }
    for(int j=0; j<n3; j++){
        geraNavio(3, mat, tamTab);
    }
    for(int j=0; j<n2; j++){
        geraNavio(2, mat, tamTab);
    }
    for(int j=0; j<n1; j++){
        geraNavio(1, mat, tamTab);
    }
}

//Registra uma jogada, autenticando os valores inseridos
posicao jogada(string jogador, int tam)
{
    posicao jogada;
    char col;
    cout << jogador << ", é a sua vez de jogar\n";
    while(true){
        cout << "Posição: ";
        cin >> jogada.lin;
        if(cin.fail()){    //Verifica se não houve erro na inserção dos dados
            cin.clear();   //Sem essa estrtura o código entrava em loop ao inserir um caractere não numérico
            cin.ignore();
        }
        cin >> col;
        if(jogada.lin>=1 and jogada.lin<=tam-2 and ((col>='a' and col<='a'+tam-3) or (col>='A' and col<='A'+tam-3)))
            break;
        else{
            cout << cor("bra-verm", "POSIÇÂO INVÀLIDA! ") << "Por favor, digite um número de 1 a " << tam-2
            << " e uma letra de A a " << (char)('A'+tam-3) << endl;
        }
    }
    if(col>='a')
        jogada.col = (int)col-'a'+1;
    else
        jogada.col = (int)col-'A'+1;
    return jogada;
}

//Determina a qauntidade de navios e o tamanho do tabuleiro
void dificuldade(int &n1, int &n2, int &n3, int &n4, int &tam, int &uniNav)
{
    int dif;
    cout << "Selecione a dificulade em que vocês desejam jogar:\n";
    cout << "          FÀCIL   MÈDIO   DIFÌCIL\n";
    cout << "           [1]     [2]      [3]\n\n";
    while(true){
        cout << "Dificuldade: ";
        cin >> dif;
        if(dif>=1 and dif<=3)
            break;
        else
            cout << "Valor inválido. Por favor, digite um número de 1 a 3.\n";
    }

    switch(dif){
        case 1: n1=4; n2=3; n3=2; n4=1; tam=12, uniNav=20; break;
        case 2: n1=8; n2=6; n3=4; n4=2; tam=17, uniNav=40; break;
        case 3: n1=12; n2=9; n3=6; n4=3; tam=22, uniNav=60; break;
    }
}

//Imprime o cabeçalho do programa
void titulo()
{
    cout << "--------------------------------------------------------------------------------\n";
    cout << "                                Batalha Naval\n";
    cout << "--------------------------------------------------------------------------------\n";
}

//Identifica as características de um navio em uma dada posição X e Y
navio caractNav(posicao pos, int tab[tabMax][tabMax], int tela[tabMax][tabMax])
{
    navio nav;

    if(tab[pos.lin+1][pos.col]==2 or tab[pos.lin-1][pos.col]==2)
        nav.vert = true;
    else
        nav.vert = false;

    if(nav.vert){ //Determina as posições inicial e final do navio
        nav.posini.lin = pos.lin;
        nav.posini.col = pos.col;
        for(int i=1; i<=3; i++){
            if(tab[pos.lin-i][pos.col]==2)
                nav.posini.lin = pos.lin-i;
            else
                break;
        }

        nav.posult.lin = pos.lin;
        nav.posult.col = pos.col;
        for(int i=1; i<=3; i++){
            if(tab[pos.lin+i][pos.col]==2)
                nav.posult.lin = pos.lin+i;
            else
                break;
        }
        nav.tam = nav.posult.lin-nav.posini.lin+1;
    }
    else{
        nav.posini.lin = pos.lin;
        nav.posini.col = pos.col;
        for(int i=1; i<=3; i++){
            if(tab[pos.lin][pos.col-i]==2)
                nav.posini.col = pos.col-i;
            else
                break;
        }

        nav.posult.lin = pos.lin;
        nav.posult.col = pos.col;
        for(int i=1; i<=3; i++){
            if(tab[pos.lin][pos.col+i]==2)
                nav.posult.col = pos.col+i;
            else
                break;
        }
        nav.tam = nav.posult.col-nav.posini.col+1;
    }

    if(nav.vert){ //Determina o número de posições do navio que foram acertadas
        for(int i=0; i<nav.tam; i++){
            if(tela[nav.posini.lin+i][nav.posini.col]==2)
                nav.acertos++;
        }
    }
    else{
        for(int i=0; i<nav.tam; i++){
            if(tela[nav.posini.lin][nav.posini.col+i]==2)
                nav.acertos++;
        }
    }

    return nav;
}

//Preenche a posição ao redor de um navio completamente destruído
void preencheRedor(navio nav, int tela[tabMax][tabMax])
{
    if(nav.acertos==nav.tam){
        if(nav.vert){
            for(int i=-1; i<=nav.tam; i++)
                for(int j=-1; j<2; j++)
                    if(tela[nav.posini.lin+i][nav.posini.col+j]==0)
                        tela[nav.posini.lin+i][nav.posini.col+j]=1;
        }
        else{
            for(int i=-1; i<= nav.tam; i++)
                for(int j=-1; j<2; j++)
                    if(tela[nav.posini.lin+j][nav.posini.col+i]==0)
                        tela[nav.posini.lin+j][nav.posini.col+i]=1;
        }
    }
}

//Valida a resposta a perguntas de sim ou não
char verificaSN(char frase[500])
{
    char resp;
    while(true){
            cout << frase;
            cin >> resp;
            if(resp=='s' or resp=='S' or resp=='n' or resp=='N')
                return resp;
            cout << "Resposta INVÁLIDA, por favor responda somente com S ou N.\n";
        }
}

//Função geral do jogo: coleta o nome dos jogadores, determina a ordem de jogada, a contagem de pontos e chama as funções anteriores
void jogo(char preencher, char jogarNovamente)
{
    srand(time(NULL));
    int tab[tabMax][tabMax], tela[tabMax][tabMax];
    int n1, n2, n3, n4, tam, cont=0, uniNav, njog=0, pjog1=0, pjog2=0, n1p=0, n2p=0, n3p=0, n4p=0;
    string jog1, jog2;
    posicao pos;
    navio nav;

    titulo();
    cout << "Nome do jogador 1: ";
    cin >> jog1;
    cout << "Nome do jogador 2: ";
    cin >> jog2;
    cout << endl;

    preencheTab(tab);
    preencheTab(tela);
    dificuldade(n1, n2, n3, n4, tam, uniNav);
    geraTab(tab, tam, n1, n2, n3, n4);

    while(cont<uniNav)
    {
        pausaLimpa(false);
        titulo();
        cout << "PONTOS\n------\n" << jog1 << ": " << pjog1 << endl << jog2 << ": " << pjog2 << "\n\n";
        cout << "NÚMERO DE NAVIOS NAUFRAGADOS\n----------------------------\n";
        cout << "Tamanho 1: " << n1p << "/" << n1 << endl;
        cout << "Tamanho 2: " << n2p << "/" << n2 << endl;
        cout << "Tamanho 3: " << n3p << "/" << n3 << endl;
        cout << "Tamanho 4: " << n4p << "/" << n4 << "\n\n";
        imprimeTab(tela, tam);


        while(true){ //Determina a ordem de jogada e se a posição é válida
            if(njog%2==0)
                pos = jogada(jog1, tam);
            else
                pos = jogada(jog2, tam);
            if(tela[pos.lin][pos.col]==0)
                break;
            cout << cor("bra-verm", "Jogada INVÀLIDA!") << "\nEssa posição já foi jogada anteriormente.\n";
        }


        if(tab[pos.lin][pos.col]==2){ //Faz a verificação dos acertos, controle dos pontos e do número de navios naufragados
            tela[pos.lin][pos.col] = 2;
            cont++;
            nav = caractNav(pos, tab, tela);
            if(preencher=='s' or preencher=='S')
                preencheRedor(nav, tela);
            if(nav.acertos==nav.tam){
                switch(nav.tam){
                    case 1: n1p++; break;
                    case 2: n2p++; break;
                    case 3: n3p++; break;
                    case 4: n4p++; break;
                }
            }

            if(njog%2==0){
                if(nav.acertos==1)
                    pjog1+=3;
                else if(nav.acertos==nav.tam)
                    pjog1+=2;
                else
                    pjog1+=1;
            }
            else{
                if(nav.acertos==1)
                    pjog2+=3;
                else if(nav.acertos==nav.tam)
                    pjog2+=2;
                else
                    pjog2+=1;
            }
        }
        else{
            tela[pos.lin][pos.col] = 1;
        }
        if((jogarNovamente=='s' or jogarNovamente=='S') and tab[pos.lin][pos.col]==2){
            if(njog%2==0)
                cout << cor("bra-verd", jog1+(string)", você pode jogar novamente.\n");
            else
                cout << cor("bra-verd", jog2+(string)", você pode jogar novamente.\n");
            continue;
        }
        njog++;
    }

    pausaLimpa();
    titulo();
    cout << "FIM DE JOGO!\n" << "Todas os navios foram naufragados.\n\n";
    imprimeTab(tela, tam);
    if(pjog1>pjog2)
        cout << "\nO vencedor foi " << cor("bra-verd", jog1) << endl;
    else if(pjog2>pjog1)
        cout << "\nO vencedor foi " << cor("bra-verd", jog2) << endl;
    else
        cout << "\nO jogo terminou em empate" << endl;
    cout << "PONTOS\n" << jog1 << ": " << pjog1 << endl << jog2 << ": " << pjog2 << "\n";

}

int main()
{
    cout << "\x1B[30;107m";
    pausaLimpa(true, "Bem vindo ao Batalha Naval, pressione enter para iniciar o jogo.");
    titulo();
    cout << "Regras: \n- Os navios são distribuidos no tabuleiro de forma aleatória pelo computador\n";
    cout << "- Os navios não se tocam, eles estão sempre separados por, pelo menos, uma posição do tabuleiro\n";
    cout << "- Os jogadores jogam de forma alternada, competindo entre si para acertar o maior número de barcos\n";
    cout << "- A cada jogada, um competidor deve informar a posição (linha e coluna) em que deseja lançar uma bomba no tabuleiro\n";
    cout << "- As linhas são indicadas por números e as colunas são indicadas por letras\n";
    cout << "- O primeiro acerto a uma embarcação vale 3 pontos, os seguintes valem 1 ponto e o último acerto vale 2 pontos\n";
    cout << "- O jogo acaba quando todos os navios forem destruídos\n";
    cout << "\n           DIVIRTAM-SE!\n\n";

    while(true){
        cout << endl;
        pausaLimpa();
        titulo();

        char preencher, jogarNovamente;
        cout << "\n               --------------\n               MENU DE OPÇÔES\n               --------------\n";
        preencher = verificaSN("Ativar preenchimento automático do entorno de um navio destruído? [S/N] >> ");
        jogarNovamente = verificaSN("Ativar jogada dupla para quem acertou? [S/N] >> ");

        pausaLimpa();
        jogo(preencher, jogarNovamente);

        pausaLimpa();
        titulo();
        char jogar;
        jogar = verificaSN("Vocês desejam jogar novamente? [S/N] >> ");
        if(jogar=='n' or jogar=='N'){
            cout << "Obrigado por jogar! :)";
            break;
        }
    }
    return 0;
}
