# PalavrasCruzadas_C


## Descrição:

Este repositório contém o código e 3 "fases" de um jogo de palavras cruzadas escritos em C, que foi feito em grupo como trabalho final da disciplina de Computação 1/2022.1 - UFRJ.

Inspirado e adaptado pelo projeto proposto em https://ecs-network.serv.pacific.edu/past-courses/2013-spring-ecpe-170/lab/lab-c-programming-project

## Como executar?

Compile o arquivo-fonte jogo.c e execute, na pasta que contém as "fases". Usamos o gcc como compilador.

Como a disciplina usava C 99 como padrão, as flags com as quais compilávamos incluiam -ansi -pedantic.

## Como funciona?

O jogo carrega fases pré-elaboradas em arquivos .txt contendo o tamanho do jogo, palavras, dicas e outras informações necessárias. 

Do arquivo monta-se duas matrizes bidimensionais: uma que é o jogo em si, com espaços em branco no lugar das palavras, alterada conforme o jogador deseja, e um gabarito para posterior comparação.


Tem um sistema simples para 1 save, criando um arquivo binário para conter a matriz a ser salva.


## O que tem nos arquivos de fase?

O arquivo texto de fase contém as seguintes informações:

● Tamanho do matriz (nº de linhas e nº de colunas)

● Número de palavras

● Palavras, contendo as seguintes informações para cada uma delas:

     ○ Linha inicial 
  
     ○ Coluna inicial
  
     ○ Sentido
  
     ○ Dica da palavra
