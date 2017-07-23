#include <stdio.h>
#include <wchar.h>
#include <locale.h>

int gen;

typedef union {
	unsigned valor :9;
	struct {
		unsigned n :2; 
		unsigned f :4; /*4,5,6,7,q,j,k,1,2,3*/
		unsigned m :3;
		/*detalhes: naipe, face, manilha */
	}d;
} Carta;

struct tela {
	char strings[3][80];
	unsigned compnumcartas :2;
	unsigned compmostra :1;
	Carta comp[3];
	Carta usuario[3];
	unsigned cartasnousu :3;
	unsigned cartasnamesa :2;
	Carta mesa[2];
	unsigned pontosusu :5;
	unsigned pontoscomp :5;
	unsigned valorrod :4;
	unsigned mnova :1;
	Carta defman;
};

enum cor {or,fundo,verm,preta,atras,truco,mesa,titulo};
void mudecor(enum cor c)
{
	char vet[][9]={"0","0;37;44","1;31;47","1;30;47","1;30;43","1;31;40","0;37;46","0;30;44"};
	wprintf(L"\x1b[%sm",vet[c]);
}

wchar_t* carta2wstr(Carta c, wchar_t* endereco)
{
	wchar_t carta=((c.d.n-1)%3+(c.d.n==3))*16+0x1F0A0;
	char tmp=(c.d.f+3)%10;
	carta+=(tmp>6)?((2-2*tmp)%5+10):((tmp+1)%10);
	endereco[0]=carta;
	endereco[1]=0x20;
	endereco[2]=0x20;
	endereco[3]=0x00;
	return endereco;
}

enum direc {aci='A',aba,dir,esq};
void mudepos(char p,int n) /*A=acima,D=esquerda,C=direita,B=abaixo*/
{
	for(;n>0;n--)
		wprintf(L"\x1b[%c",p);
}

void ponhacarta(Carta c){
	char tmp,face,vetn[]={6,0,5,3},vetf[]={'Q','J','K','A'};
	wchar_t naipe;
	tmp=(c.d.f+2)%10;
	if(tmp<6) face=tmp+'2';
	else face=vetf[tmp-6];
	naipe=vetn[c.d.n]+0x2660;
	if(c.d.n%2) mudecor(preta);
	else mudecor(verm);
	wprintf(L"╭───╮");
	mudepos(esq,5);
	mudepos(aba,1);
	wprintf(L"│%c  │",face);
	mudepos(esq,5);
	mudepos(aba,1);
	wprintf(L"│  %lc│",naipe);
	mudepos(esq,5);
	mudepos(aba,1);
	wprintf(L"╰───╯",face);
	mudepos(esq,5);
	mudepos(aci,3);
	wprintf(L"\x1b[0m");
}

void cartaapaga(){
	int i;
	mudecor(fundo);
	mudepos(dir,56);
	for(i=0;i<4;i++)
	{
		wprintf(L"     ");
		mudepos(esq,5);
		mudepos(aba,1);
	}
	mudepos(esq,56);
	mudepos(aci,4);
}

void apagaletra()
{
	mudecor(fundo);
	mudepos(esq,2);
	mudepos(aba,1);
	wprintf(L"  ");
	mudepos(aci,1);	
}

void imprimeg(struct tela t)
{
	int i;
	char face[]="4567QJKA23";
	char naipe[4][8]={"Ouros","Espadas","Copas","Ouros"};
	printf("\n\n\nVoce %2d x %2d Computador\nComputador tem %d cartas\n",t.pontosusu,t.pontoscomp,t.compnumcartas);
	if(t.cartasnamesa&2)printf("Computador: %c de %s",face[t.mesa[1].d.f],naipe[t.mesa[1].d.n]);
	printf("\n");
	if(t.cartasnamesa&1)printf("Voce: %c de %s",face[t.mesa[0].d.f],naipe[t.mesa[0].d.n]);
	printf("\n");
	if(t.mnova)printf("Manilha e o %c",face[(t.defman.d.f+1)%10]);
	printf("\n\n");
	if(t.cartasnousu&1)printf("A) %c de %s",face[t.usuario[0].d.f],naipe[t.usuario[0].d.n]);
	printf("\n");
	if(t.cartasnousu&2)printf("B) %c de %s",face[t.usuario[1].d.f],naipe[t.usuario[1].d.n]);
	printf("\n");
	if(t.cartasnousu&4)printf("C) %c de %s",face[t.usuario[2].d.f],naipe[t.usuario[2].d.n]);
	printf("\n\n");
	printf("%s\n",t.strings[2]);
	printf(">");
	return;
}

void imprime(struct tela t)
{
	int i;
	char f[4][8]={"Truco!","Seis!","NOVE!","DOZE!!!"};
	if(gen) return imprimeg(t);
	mudecor(fundo);
	wprintf(L"\x1b[H\x1b[J\
┌──────────────────────────────────────────────────────────────────────────────┐\n\
│ ┌──────────────────────────────────────────────────────────────────────────┐ │\n\
│ │                           Você     x     Computador                      │ │\n\
│ │                                                               ");
	mudecor(atras);
	wprintf(L"╭───╮");
	mudecor(fundo);
	wprintf(L"      │ │\n│ │     A               ");
	mudecor(mesa);
	wprintf(L"                               ");
	mudecor(fundo);
	wprintf(L"           ");
	mudecor(atras);
	wprintf(L"│▒▒▒│");
	mudecor(fundo);
	wprintf(L"      │ │\n│ │                     ");
	mudecor(mesa);
	wprintf(L"                               ");
	mudecor(fundo);
	wprintf(L"           ");
	mudecor(atras);
	wprintf(L"│▒▒▒│");
	mudecor(fundo);
	wprintf(L"      │ │\n│ │                     ");
	mudecor(mesa);
	wprintf(L"                               ");
	mudecor(fundo);
	wprintf(L"           ");
	mudecor(atras);
	wprintf(L"╰───╯");
	mudecor(fundo);
	wprintf(L"      │ │\n│ │                     ");
	mudecor(mesa);
	wprintf(L"             ");
	mudecor(atras);
	wprintf(L"╭───╮");
	mudecor(mesa);
	wprintf(L"             ");
	mudecor(fundo);
	wprintf(L"           ");
	mudecor(atras);
	wprintf(L"╭───╮");
	mudecor(fundo);
	wprintf(L"      │ │\n│ │     B               ");
	mudecor(mesa);
	wprintf(L"             ");
	mudecor(atras);
	wprintf(L"│▒▒▒│");
	mudecor(mesa);
	wprintf(L"             ");
	mudecor(fundo);
	wprintf(L"           ");
	mudecor(atras);
	wprintf(L"│▒▒▒│");
	mudecor(fundo);
	wprintf(L"      │ │\n│ │                     ");
	mudecor(mesa);
	wprintf(L"             ");
	mudecor(atras);
	wprintf(L"│▒▒▒│");
	mudecor(mesa);
	wprintf(L"             ");
	mudecor(fundo);
	wprintf(L"           ");
	mudecor(atras);
	wprintf(L"│▒▒▒│");
	mudecor(fundo);
	wprintf(L"      │ │\n│ │                     ");
	mudecor(mesa);
	wprintf(L"             ");
	mudecor(atras);
	wprintf(L"╰───╯");
	mudecor(mesa);
	wprintf(L"             ");
	mudecor(fundo);
	wprintf(L"           ");
	mudecor(atras);
	wprintf(L"╰───╯");
	mudecor(fundo);
	wprintf(L"      │ │\n│ │                     ");
	mudecor(mesa);
	wprintf(L"                               ");
	mudecor(fundo);
	wprintf(L"           ");
	mudecor(atras);
	wprintf(L"╭───╮");
	mudecor(fundo);
	wprintf(L"      │ │\n│ │     C               ");
	mudecor(mesa);
	wprintf(L"                               ");
	mudecor(fundo);
	wprintf(L"           ");
	mudecor(atras);
	wprintf(L"│▒▒▒│");
	mudecor(fundo);
	wprintf(L"      │ │\n│ │                     ");
	mudecor(mesa);
	wprintf(L"                               ");
	mudecor(fundo);
	wprintf(L"           ");
	mudecor(atras);
	wprintf(L"│▒▒▒│");
	mudecor(fundo);
	wprintf(L"      │ │\n│ │                     ");
	mudecor(mesa);
	wprintf(L"                               ");
	mudecor(fundo);
	wprintf(L"           ");
	mudecor(atras);
	wprintf(L"╰───╯");
	mudecor(fundo);
	wprintf(L"      │ │\n\
│ │                                                                          │ │\n\
│ ╞══════════════════════════════════════════════════════════════════════════╡ │\n\
│ │                                                                          │ │\n\
│ │                                                                          │ │\n\
│ │                                                                          │ │\n\
│ ├──────────────────────────────────────────────────────────────────────────┤ │\n\
│ │>                                                                         │ │\n\
│ └──────────────────────────────────────────────────────────────────────────┘ │\n\
└─────────────────────────────── ");
	mudecor(titulo);
	wprintf(L"Jogo de Truco");
	mudecor(fundo);
	wprintf(L" ────────────────────────────────┘\n");
	mudepos(aci,22);
	mudepos(dir,30);
	wprintf(L"Você  %2d x %2d  Computador\n",t.pontosusu,t.pontoscomp);
	mudepos(dir,10);
	if(t.cartasnousu&1)
		ponhacarta(t.usuario[0]);
	else apagaletra();
	if(t.valorrod!=1&&t.pontosusu!=11&&t.pontoscomp!=11)
	{
		mudepos(dir,26);
		mudecor(truco);
		mudepos(esq,wprintf(L"%s",f[t.valorrod/3-1])+26);
	}
	if(t.compnumcartas<1)
		cartaapaga();
	else if(t.compmostra)
	{
		mudepos(dir,56);
		ponhacarta(t.comp[0]);
		mudepos(esq,56);
	}
	mudepos(aba,4);
	if(t.cartasnousu&2)
		ponhacarta(t.usuario[1]);
	else apagaletra();
	if(t.compnumcartas<2)
		cartaapaga();
	else if(t.compmostra)
	{
		mudepos(dir,56);
		ponhacarta(t.comp[1]);
		mudepos(esq,56);
	}
	if(t.cartasnamesa&1){
		mudepos(dir,18);
		ponhacarta(t.mesa[0]);
		mudepos(esq,18);
	}
	if(t.cartasnamesa&2){
		mudepos(dir,36);
		ponhacarta(t.mesa[1]);
		mudepos(esq,36);
	}
	mudepos(aba,4);
	if(t.cartasnousu&4)
		ponhacarta(t.usuario[2]);
	else apagaletra();
	if(t.mnova){
		mudepos(dir,27);
		ponhacarta(t.defman);
		mudepos(esq,27);
	}
	if(t.compnumcartas<3)
		cartaapaga();
	else if(t.compmostra)
	{
		mudepos(dir,56);
		ponhacarta(t.comp[2]);
		mudepos(esq,56);
	}
	mudepos(aba,6);
	mudepos(esq,10);
	mudecor(fundo);
	for(i=0;i<3;i++){
		mudepos(dir,4);
		wprintf(L"%s\n",t.strings[i]);
	}
	mudepos(aba,1);
	mudepos(dir,5);
}

int escolhatipo(int i,char* nome)
{
	int le;
	if(i){
		gen=1;
		printf("Jogo de Truco\nAluno: Iuri Rezende Souza\nMatricula: 11111EMT009\n\n\
Escolha o tipo do jogo:\n\
 1. Baralho vazio\n\
 2. Baralho cheio, manilha velha\n\
 3. Baralho cheio, manilha nova\n\
> ");
	}
	else{
		gen=0;
		setlocale(LC_ALL,"");
		wprintf(L"Jogo de Truco\nAluno: Iuri Rezende Souza\nMatrícula: 11111EMT009\n\n\
Este programa usa caracteres Unicode e formatação ANSI.\nExecute \"%s -g\" se você usa terminal sem suporte a essas características.\n\n\
Escolha o tipo do jogo:\n\
 1. Baralho vazio\n\
 2. Baralho cheio, manilha velha\n\
 3. Baralho cheio, manilha nova\n\
> ",nome);
	}
	scanf("%d",&le);
	pegueenter();
	return (le-1)%3;
}