#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM_JOGOS 3

typedef struct _PALAVRA{
    char *gabarito;
    int sentido; /*1 para horizontal, 0 para vertical*/
    char dica[50];
    int l_inicial, c_inicial; /*linha e coluna inicial*/
} PALAVRA;

char** carrega_gabarito (char *nome_arq, char ***jogo_atual, int* linhas, int* colunas, int* n_palavras);
void printa_jogo (char **jogo_atual, int n_linhas, int n_colunas);
void carrega_palavras (char *nome_arq, PALAVRA *palavra, int n_palavras, int linhas, int colunas);
void print_dicas (PALAVRA *palavra, int n_palavras);
int jogo (char **jogo_atual, char **gabarito, int n_palavras, int linhas, int colunas, PALAVRA *palavra);
int escolher_fase();
int checa_opcao (int *indice, int *posicao, int n_palavras, int n_linhas, int n_colunas, char **jogo);
void alterar_jogo_letra (char **jogo_atual, int *posicao);
char verifica_char (char letra);
void apagar_letra(char **jogo_atual, int *posicao);
int inserir_palavra(char **jogo_atual, int n_linhas, int n_colunas, int indice, PALAVRA *palavras);
int comparar_gabarito(char **jogo_atual, char **gabarito, int linhas, int colunas);
void verifica_palavra(char *inserido, int indice, PALAVRA *palavras, int *valido);
int valida_int(void);    
int carrega_save(char **jogo_atual,int n_linhas, int n_colunas);
int retorna_fase_save();
int checa_save();
void salvar_jogo(char **jogo_atual,int n_palavras, int n_linhas, int fase); 
int checa_completo(char **jogo_atual,int n_linhas, int n_colunas);


int main(void)
{
    int n_linhas,n_colunas,n_palavras,fase_escolhida,opcao=2;
    PALAVRA *palavra; 
    char **gabarito,**jogo_atual;
    char fases[3][10];

    strcpy(fases[0], "fase1.txt");
    strcpy(fases[1], "fase2.txt");
    strcpy(fases[2], "fase3.txt");

    if(checa_save())
    {
        while(1)
        {
            puts("\nSave encontrado!\n"
            "Deseja carregar o save?\n"
            "[1] Sim\n"
            "[2] Nao\n");
            opcao = valida_int();
            if(opcao==1 || opcao == 2) break;
        }
    }
    while(1)
    {
        if(opcao==2)
        {
            fase_escolhida = escolher_fase();
        }
        else if(opcao==1)
        {
            fase_escolhida = retorna_fase_save();
        }

        gabarito = carrega_gabarito(fases[fase_escolhida], &jogo_atual, &n_linhas, &n_colunas, &n_palavras);
    

        palavra = (PALAVRA*) malloc(sizeof(PALAVRA)*n_palavras);
        if(palavra == NULL)
        {
            puts("Erro de memoria.\n");
            return 2;
        }
    
        carrega_palavras(fases[fase_escolhida], palavra,n_palavras, n_linhas, n_colunas);
        /*aqui ja carregou tudo, tem o gabarito, o jogo_atual(vazio), e o array de palavras*/

        if(opcao==1)
        {
            carrega_save(jogo_atual,n_linhas,n_colunas);
        }


        while(jogo(jogo_atual, gabarito, n_palavras, n_linhas, n_colunas, palavra));

        while(1)
        {
            puts("\nDeseja salvar o seu jogo?\n"
            "[1] Sim\n"
            "[2] Nao\n");
            opcao = valida_int();
            if(opcao==1 || opcao == 2) break;
        }
        if(opcao==1)
        {
            salvar_jogo(jogo_atual,n_palavras,n_linhas,fase_escolhida);
        } 
        else
        {
            free(jogo_atual);
            free(gabarito);
            free(palavra);
        }
        while(1)
        {
            puts("\nDeseja escolher outro passatempo?\n"
            "[1] Sim\n"
            "[2] Nao\n");
            opcao = valida_int();
            if(opcao==1 || opcao == 2) break;
        }
        if(opcao==2) break;
        opcao = 2;
    }

    /*comparar com o gabarito*/
    
    return 0;
}

char **carrega_gabarito(char *nome_arq, char ***jogo_atual, int* linhas, int* colunas, int* n_palavras)
{
    FILE *pa;
    char *palavra, **gabarito, lixo[50];
    int i, j, k, maior, linha_inicial, coluna_inicial, sentido;

    if((pa = fopen(nome_arq, "r")) == NULL)
    {
        puts("Erro ao abrir arquivo de jogo.");
        return NULL;
    }

    /*lendo tamanho do tabuleiro e numero de palavras*/
    fscanf(pa, "%d %d %d", linhas, colunas, n_palavras);

    /*inicializando tamanho do jogo*/
    gabarito = (char**) malloc (sizeof(char*) * (*linhas));
    if(gabarito == NULL)
    {
        puts("Erro ao carregar tamanho do jogo.");
        return NULL;
    }

    *jogo_atual = (char**) malloc (sizeof(char*)*(*linhas));
    if(*jogo_atual == NULL)
    {
        puts("Erro ao carregar tamanho do jogo.");
        return NULL;
    }

    for(i = 0; i < (*linhas); i++)
    {
        gabarito[i] = (char*) malloc(sizeof(char) * (*colunas));
        if(gabarito[i] == NULL)
        {
            puts("Erro ao carregar tamanho do jogo.");
            return NULL;
        }

        (*jogo_atual)[i] = (char*) malloc(sizeof(char) * (*colunas));
        
        if((*jogo_atual)[i] == NULL)
        {
            puts("Erro ao carregar tamanho do jogo.");
            return NULL;
        }
    
        /*carregando tudo com # */
        for(j = 0; j < (*colunas); j++)
        {
            gabarito[i][j] = '#';
            (*jogo_atual)[i][j] = '#';
        }
    }

    /*carregando as palavras no gabarito, e os "_" no jogo_atual*/
    (*linhas)>(*colunas) ? (maior = *linhas) : (maior= *colunas);/*tamanho da maior palavra nao pode ser mais que 
    o maior valor entre linhas/colunas*/
    palavra = (char*)malloc(sizeof(char)*(maior + 1));
    if(palavra == NULL)
    {
        puts("Erro de memoria.");
        return NULL;
    }

    for(i = 0; i < (*n_palavras); i++)
    {
        fscanf(pa,"%s %d %d %d\n", palavra, &linha_inicial, &coluna_inicial, &sentido);
        /*linha no txt = "palavra" linha_inicial coluna_inicial vertical/horizontal*/
        /*printar palavra no gabarito*/
        if(sentido) /*caso horizontal*/
        {
            for(k=0, j = coluna_inicial; palavra[k] != '\0'; j++, k++)
            {
                gabarito[linha_inicial][j] = palavra[k];
                (*jogo_atual)[linha_inicial][j]='_';
            }
        }
        else /*caso vertical*/
        {
        for(k = 0, j = linha_inicial; palavra[k]!='\0'; j++, k++)
        {
            gabarito[j][coluna_inicial] = palavra[k];
            (*jogo_atual)[j][coluna_inicial] = '_';
        }
    }
    if (i != (*n_palavras) - 1) fgets(lixo, 50, pa); /*pula linha de dica*/
    }

    if (fclose(pa) != 0)
    {
        puts("Erro fechando arquivo.\n");
    }
    return gabarito;
}

void carrega_palavras(char *nome_arq, PALAVRA *palavra, int n_palavras, int linhas, int colunas)
{
    FILE *pa;
    int i, maior;
    char lixo[10];
    
    if ((pa = fopen(nome_arq, "r")) == NULL)
    {
        puts("Erro ao abrir arquivo de jogo.");
        return;
    }
    /*determinar tamanho max da palavra pra alocar no char* da estrutura*/
    linhas > colunas? (maior = linhas) : (maior = colunas);

    fgets(lixo, 10, pa);

    for (i = 0; i < n_palavras; i++)
    {
        palavra[i].gabarito = (char*)malloc(sizeof(char)*(maior + 1));
        if (palavra[i].gabarito == NULL)
        {
            puts("Erro ao alocar memoria para palavras.\n");
            return;
        }
    
        fscanf(pa, "%s %d %d %d\n", palavra[i].gabarito, &palavra[i].l_inicial, &palavra[i].c_inicial, &palavra[i].sentido);
        fgets(palavra[i].dica, 50, pa);
    }
    fclose(pa);
    return;
}

void printa_jogo(char **jogo_atual, int n_linhas, int n_colunas)
{
    int i, j;

    printf("\n\n\t\t\t\t   ");
    for(i = 0;i < n_colunas; i++)
    {
        printf("%c  ", i + 65); /*printa A,B,C,.., etc.   importante limitar hipoteticamente o 
        numero de linhas até 23 entao, pra nao passar de Z*/
    }
    
    printf("\n\t\t\t\t   ");
    for(i = 0; i < n_colunas; i++)
    {
        printf("-  ");
    }
    
    printf("\n");
    for(i = 0; i < n_linhas; i++)  /*printa linha*/
    {
        if (i + 1 < 10) printf("\t\t\t\t%d| ",i+1);
        else printf("\t\t\t\t\b%d| ",i+1);

        for(j = 0; j < n_colunas; j++)
        {
            printf("%c  ",jogo_atual[i][j]);
        }

        puts("|");
    }
    puts("\n");
}

void print_dicas(PALAVRA *palavra, int n_palavras)
{
    int i;
    char sentido[11];

    puts("\t\t\t\t   [DICAS]\n\n");
    puts("\t        Indice da palavra | Posicao inicial | Sentido | Dica\n\n");
    

    for(i = 0; i < n_palavras; i++)
    {
        if(palavra[i].sentido) strcpy(sentido,"Horizontal");
        else strcpy(sentido,"Vertical");
        if(i<10)
        {
            if(palavra[i].sentido)  /*só pra alinar a barra  onde diz o sentido*/
            {
                printf("\t\t[%d] :  %d%c  |  %s |  %s", i+ 1, palavra[i].l_inicial + 1, palavra[i].c_inicial+65,
                sentido,palavra[i].dica);
        
            }
            else
            {
                printf("\t\t[%d] :  %d%c  |  %s   |  %s", i + 1, palavra[i].l_inicial + 1, palavra[i].c_inicial+65,
                sentido, palavra[i].dica);
            }
        }
        else
        {
             if(palavra[i].sentido)  /*só pra alinar a barra  onde diz o sentido*/
            {
                printf("\t\t[%d] : %d%c  |  %s |  %s", i+ 1, palavra[i].l_inicial + 1, palavra[i].c_inicial+65,
                sentido,palavra[i].dica);
        
            }
            else
            {
                printf("\t\t[%d] : %d%c  |  %s   |  %s", i + 1, palavra[i].l_inicial + 1, palavra[i].c_inicial+65,
                sentido, palavra[i].dica);
            }
        }
    }

    puts("\n");
}

int jogo(char **jogo_atual, char **gabarito, int n_palavras, int linhas, int colunas, PALAVRA *palavra)
{
    int indice, posicao[2], opc;
    printa_jogo(jogo_atual, linhas, colunas);
    print_dicas(palavra, n_palavras);

    if(n_palavras > 20) printa_jogo(jogo_atual, linhas, colunas); /*caso o mapa fique distante das dicas*/

    printf("\n[0] - Encerrar programa/Salvar\n"
    "[1] - Inserir palavra ou letra\n"
    "[2] - Apagar letra\n"
    "[3] - Comparar gabarito\n");
    while(1)
        {
            opc = valida_int();
            if(opc<0 || opc>3)
            {
                puts("\n\t[Insira uma opcao valida.]");
        
            }
            else break;
        }
    
    while(1)
    {
        switch(opc)
        {
            case 0:
                return 0;
                break;
            case 1:
                while(checa_opcao(&indice, posicao, n_palavras, linhas, colunas, jogo_atual))
                {
                    puts("\nEntrada invalida. Selecione um indice ou uma posicao precedida de hifen.\n"
                    "Exemplo: \"1\" ou \"-1d\".\n");
                }
                if(indice<0)
                {
                    alterar_jogo_letra(jogo_atual,posicao);
                }
                else if(posicao[0]<0)
                {
                    inserir_palavra(jogo_atual, linhas, colunas, indice, palavra);
                }
                break;
            case 2:
                indice=-12;
                while(checa_opcao(&indice, posicao, n_palavras, linhas, colunas, jogo_atual) || indice>=0)
                {
                    puts("\nEntrada invalida. Selecione uma posicao, precedida de hifen.\n"
                    "Exemplo: \"1\" ou \"-1d\".\n");
                    indice=-12;
                }
                apagar_letra(jogo_atual, posicao);
                break;
            case 3:
                if(!checa_completo(jogo_atual,linhas,colunas))
                {
                    puts("\n\t\t[Atençao!]\n"
                        "\t[O jogo nao esta completo ainda!]\n");
                }
                puts("\n[Voce tem certeza que deseja visualizar o gabarito?]\n"
                "[1] Sim\n"
                "[2] Nao\n");
                while(1)
                {
                    opc = valida_int();
                    if(opc==1 || opc==2) break;
                    puts("\nInsira uma opcao valida.\n");
                }
                if(opc==1)
                {
                if(comparar_gabarito(jogo_atual, gabarito, linhas, colunas)) return 0;
                }
                break;
        }
        printa_jogo(jogo_atual, linhas, colunas);
        print_dicas(palavra, n_palavras);
        if(n_palavras > 20) printa_jogo(jogo_atual, linhas, colunas); /*caso o mapa fique distante das dicas*/

        
        printf("\n[0] - Encerrar programa\n"
        "[1] - Inserir palavra ou letra\n"
        "[2] - Apagar letra\n"
        "[3] - Comparar gabarito\n");
        while(1)
        {
            opc = valida_int();
            if(opc<0 || opc>3)
            {
                puts("\n\t[Insira uma opcao valida.]");
        
            }
            else break;
        }
    }


}

int escolher_fase()
{
    int valido, lixo, valor;
    puts("\n\n[Selecione o passatempo que deseja]\n"
    "[1] Tamanho 5x5, 6 Palavras\n"
    "[2] Tamanho 12x11, 35 palavras\n"
    "[3] Tamanho 14x8, 27 palavras\n"
    "Insira o numero:");

	valido = scanf("%d", &valor);
        while (valido == 0 || (valor < 0 && valor > NUM_JOGOS))
        {
            while((lixo = getchar()) != EOF && lixo != '\n');
            puts("\nInsira um valor válido: ");
            valido = scanf("%d", &valor);
        }
	while ((lixo = getchar()) != EOF && lixo != '\n');
	return valor - 1;
}

/* Posição[0] => Linha escolhida e Posição[1] => Coluna escolhida */
/* Indice seria o indice da palavra na tabela de dicas*/
int checa_opcao(int *indice, int *posicao, int n_palavras, int n_linhas, int n_colunas, char **jogo)
{
    char entrada[5], coluna;
    int opcao =- 1, i, col, lin; /*1 caso indice 0 caso posicao*/
    if(*indice==-12)
    {
        puts("\nInsira a posicao a apagar, precedida de hifen:\n");
    }
    else{
    puts("Insira o indice da palavra ou a posicao a alterar desejada precedida de hifen:\n");
    }
    scanf("%s", entrada);
    /*determinar se é indice ou posicao*/
    if ((*indice = atoi(entrada)) == 0 || (*indice < 1 || *indice > n_palavras)) /*caso nao seja inteiro*/
    {
        if (entrada[0] != '-')
        {
            return 1;
        }
        else
        {
            opcao = 0;
        }
    }
    else 
    {
        posicao[0] =- 1917;
        *indice-=1;
        return 0;
    } /*inteiro validado*/

    if (opcao == 0)
    {
        /* printf("teste strlen= %d e str = %s\n",strlen(entrada),entrada); */
        for (i = 0; i < 5; i++)
        {
            entrada[i] = entrada[i + 1];
        }
        /* printf("teste strlen= %d str=%s \n ",strlen(entrada),entrada); */
        coluna = entrada[strlen(entrada) - 1];

        if (coluna <= 'z' && coluna >= 'a')
        {
            coluna = coluna-32;
        }
        else if (coluna < 'A' || coluna > 'Z')
        {
            return 1;
        } /*coluna validada,falta linha*/

        col = coluna - 65;
        entrada[strlen(entrada) - 1] = '\0';

        if(col >= n_colunas) return 1;

        if((lin = atoi(entrada)) == 0 || lin < 1 || lin > n_linhas)
        {
            return 1;
        }
        else
        {
            posicao[0] = lin - 1;
            posicao[1] = col;
            if(jogo[posicao[0]][posicao[1]] == '#')
            {
                return 1;
            }
            else
            {
                *indice =- 1917;
                return 0;
            }
        }
    }
    /* puts("Se chegou ate aqui deu erro eu acho\n."); */
    return 1;
}

char verifica_char(char letra)
{

    if(letra<'A' || (letra>'Z' && letra<'a') || letra>'z' ) /*se nao for letra*/
        {
            printf("letra=%d\n",(int)letra);
            puts("\nCaractere inserido invalido!\n");
            return 0;
        }
        else if(letra>96 && letra<123)
        {
            letra-=32;
        }
    return letra;

}

void alterar_jogo_letra(char **jogo_atual, int *posicao) 
{
    char string[40],lixo,letra;
    
    while((lixo=getchar()) != EOF && lixo!= '\n');
    
    while(1)
    {
        
        printf("\nInsira a letra que deseja escrever: ");       
        fgets(string,50,stdin);
        if(strlen(string)==2){
        letra = verifica_char(string[0]);
        break;
        }
        else
        {
            puts("\nInsira somente uma letra!\n");
        }
    }

    jogo_atual[posicao[0]][posicao[1]] = letra;
}

void apagar_letra(char **jogo_atual, int *posicao) 
{
    jogo_atual[posicao[0]][posicao[1]] = '_';
}

int comparar_gabarito(char **jogo_atual, char **gabarito, int n_linhas, int n_colunas)
{
    int cont = 0, i, j;

    for (i = 0; i < n_linhas; i++)
    {
        for (j = 0; j < n_colunas; j++)
        {
            if(jogo_atual[i][j] != gabarito[i][j])
            {
                cont++;
                break;
            }
        }
        if(cont>0) break;
    }
    puts("\n\t\t\t\t [GABARITO] \n");
    printa_jogo(gabarito,n_linhas,n_colunas);
    if (cont == 0)
    {
        printf("\n\t\t\t[Parabens! Voce completou esta fase!]\n");
        return 1;
    }
    else 
    {
        printf("\n\t\t\t[ Nao esta correto! Falta mais um pouco! ] \n");
        return 0;
    }
}

int inserir_palavra(char **jogo_atual, int n_linhas, int n_colunas, int indice, PALAVRA *palavras){
    int i, j,valido=0;
    char inserido[40];

    while(!valido)
    {
    printf("\nTamanho da palavra selecionada: %lu\n"
    "Insira a palavra que deseja escrever:",
    (long unsigned int)strlen(palavras[indice].gabarito));
    scanf("%s",inserido);
    verifica_palavra(inserido,indice,palavras,&valido);
    }


    
    /* 
        1 => Horizontal
        0 => Vertical
    */

    if(palavras[indice].sentido == 1)
    {
        for(i = palavras[indice].c_inicial,j=0; j < strlen(inserido) ;j++,i++)
        {
                jogo_atual[palavras[indice].l_inicial][i] = inserido[j];
        }                  
    }
    else
    {
         for(i = palavras[indice].l_inicial,j=0; j < strlen(inserido) ;j++,i++)
        {
                jogo_atual[i][palavras[indice].c_inicial] = inserido[j];
        }         
    }
    return 0;

}

void verifica_palavra(char *inserido, int indice, PALAVRA *palavras, int *valido)
{

    int tam, i;
    char *nova_palavra;
    nova_palavra = (char*)malloc(sizeof(char)*40);

    tam = strlen(inserido);


    for(i=0;i<tam;i++)
    {
        if(inserido[i]<65 || (inserido[i]>90 && inserido[i]<97) || inserido[i]>122 ) /*se nao for letra*/
        {
            puts("\nPalavra contem caracteres invalidos!\n");
            *valido=0;
            return;
        }
        else if(inserido[i]>96 && inserido[i]<123)
        {
            inserido[i]-=32;
        }
    }

    if(tam>strlen(palavras[indice].gabarito)) /*se tamanho n eh o mesmo da original*/
    {
         puts("\nPalavra maior que a indicada!\n");
        *valido =0;
        return;
    }

    

    strcpy(nova_palavra, inserido);
    free(nova_palavra);
    *valido=1;
    return;
}
int valida_int(void)    
{
	int valido,lixo,valor;
	valido=scanf("%d",&valor);
        while(valido==0)
        {
            while((lixo=getchar()) != EOF && lixo!= '\n');
            puts("\nInsira um valor válido: ");
            valido=scanf("%d",&valor);
        }
	while((lixo=getchar()) != EOF && lixo!= '\n');
	return valor;
}

void salvar_jogo(char **jogo_atual,int n_palavras, int n_linhas, int fase) /*chamar da main*/
{
  FILE *pa;
  int i;
  if((pa=fopen("save.bin", "w+b"))==NULL)
  {
    puts("\nErro ao criar arquivo de save.\n");
  }

  fwrite(&fase,sizeof(int),1,pa); /*escreve a fase*/
  for(i=0;i<n_linhas;i++) /*salva cada linha do jogo atual*/
  {
    fwrite(jogo_atual[i],sizeof(char),strlen(jogo_atual[i]),pa);
  }

  return;
}

int checa_save()
{
  FILE *pa;
  if((pa=fopen("save.bin", "rb"))==NULL)
  {
    return 0;
  }
  else
  {
    fclose(pa);
    return 1;
  }
}
int retorna_fase_save()  /*retorna a fase e altera jogo atual*/
{
  FILE *pa;
  int fase;
  if((pa=fopen("save.bin", "rb"))==NULL)
  {
    puts("\nErro carregando save!\n");
  }

  fread(&fase,sizeof(int),1,pa);

  fclose(pa);
  return fase;
}
int carrega_save(char **jogo_atual,int n_linhas,int n_colunas)
{
  FILE *pa;
  int lixo,i;
  if((pa=fopen("save.bin", "rb"))==NULL)
  {
    puts("\nErro carregando save!\n");
    return 1;
  }
  fread(&lixo,sizeof(int),1,pa);

  for(i=0;i<n_linhas;i++)
  {
    fread(jogo_atual[i],sizeof(char),n_colunas,pa);
  }
  return 0;
}

int checa_completo(char **jogo_atual,int n_linhas, int n_colunas)
{
    int i,j;
    int completo = 1;

    for(i=0;i<n_linhas;i++)
    {
        for(j=0;j<n_colunas;j++)
        {
            if(jogo_atual[i][j]=='_')
            {
                completo=0;
            }
        }
    }

    return completo;
}