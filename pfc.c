#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define EMB 200
/* Defina COMPMOSTRA 1 para mostrar as cartas do computador.
   É só para analisar a estratégia, não pense que é um jogo justo! */
#define COMPMOSTRA 0

typedef union {
	unsigned valor :9;
	struct {
		unsigned n :2; 
		unsigned f :4; /*4,5,6,7,q,j,k,1,2,3*/
		unsigned m :3;
		/*detalhes: naipe, face, manilha */
	}d;
} Carta;


typedef struct rodada { /* enum {naojogou=-2,perdeu,cangou,venceu}; */
	signed pri :2;
	signed seg :2;
	signed ter :2;
}Rodada;

struct{
	unsigned rod :1; /* 0=usuario    */
	unsigned jog :1; /* 1=computador */
}inicio;

char venceurodada(Rodada r)
{
	char c;
	if(r.seg==-2)return 0;
	c=r.pri+r.seg;
	if(c) return c;
	if(r.ter==-2)return 0;
	return r.ter?r.ter:r.pri;
}

union {
	Rodada rod;
	unsigned val :6;
} std;

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
} Tela;

enum {ouros,espadas,copas,paus};

unsigned tipojogo; /*Vazio,CheioVelha,CheioNova*/

void mudecor(int i);

struct baralho{
	Carta c[40];
	unsigned tam :6;
	unsigned pos :6;
};

int compara(Carta c1, Carta c2)
{
	return(c1.valor/4-c2.valor/4);
}

struct baralho embaralhar(int n,char btip)
{
	struct baralho ret;
	int i;
	ret.pos=0;
	switch(btip)
	{
		case 0: /* B vazio */
			ret.tam=27;
			for(i=26;i>2;i--)
				ret.c[i].valor=i+13;
			ret.c[0].valor=76; /* 7 ouros */
			ret.c[1].valor=206; /* 7 copas */
			ret.c[2].valor=259; /* zap */
			ret.c[16].d.m=2; /* espadilha */
			break;
		case 1: /* B cheio, velha */
			ret.tam=40;
			for(i=39;i>=0;i--)
				ret.c[i].valor=i;
			ret.c[3].d.m=ret.c[3].d.n+1;/*zap*/
			ret.c[12].d.m=ret.c[12].d.n+1;
			ret.c[14].d.m=ret.c[14].d.n+1;
			ret.c[29].d.m=ret.c[29].d.n+1;
			break;
		case 2: /* B cheio, nova */
			ret.tam=39;
			Tela.defman.valor=rand()%40;
			for(i=40;i<=78;i++)
				ret.c[i%40].valor=(i+Tela.defman.valor+1)%40;
			for(i=3-Tela.defman.d.n;i<7-Tela.defman.d.n;i++)
				ret.c[i].d.m=ret.c[i].d.n+1;
	}
	for(i=0;i<n;i++)
	{
		int a,b;
		Carta tmp;
		tmp=ret.c[a=rand()%ret.tam];
		ret.c[a]=ret.c[b=rand()%ret.tam];
		ret.c[b]=tmp;
	}
	return ret;
}

void meuputs(char * string) /* Coloca uma string na tela */
{
	strncpy(Tela.strings[0],Tela.strings[1],75);
	strncpy(Tela.strings[1],Tela.strings[2],75);
	strncpy(Tela.strings[2],string,75);
}

void pegueenter() /* Lê o teclado até dar receber um enter */
{
	char a;
	do a=getchar();
	while (a!=10);
}

struct sit { /* Indica a situação em que está a rodada */
	unsigned pediu :1;
	unsigned pedinte :1;
	unsigned valor :4;
	unsigned aberto :2;
	unsigned mescolha :2;
};

int jogada(Carta comp[3],Carta mesa,int rodada,int cartasnamesa,char bmax,struct sit s,Carta defman); /* Função que faz o computador jogar */

char entra(struct sit s)
/* Função que recebe a entrada do usuário
   Retorna */
{
	char c;
	char ret;
	char ponha[70];
	if(s.pediu&&s.aberto){
		char t[4][8]={"Truco!","Seis!","NOVE!","DOZE!!!"};
		s.valor/=3;
		s.valor-=1;
		meuputs(t[s.valor]);
	}
	if(s.aberto)
		meuputs("Você vai jogar [S/N]?");
	else{
		char com[4][7]={" ou T"," ou 6"," ou 9"," ou 12"};
		s.valor/=3;
		snprintf(ponha,70,"Digite %s%s%spara jogar%s%s",
			(Tela.cartasnousu&1)?"A ":"",
			(Tela.cartasnousu&2)?"B ":"",
			(Tela.cartasnousu&4)?"C ":"",
			((s.pedinte||!s.pediu)&&!s.mescolha&&s.valor<4)?com[s.valor]:"",
			((s.pedinte||!s.pediu)&&!s.mescolha&&s.valor<4)?((s.valor)?" para aumentar":" para trucar"):"");
		meuputs(ponha);
	}
	imprime(Tela);
	c=toupper(getchar());
	switch(c)
	{
		case 'A':
		case 'B':
		case 'C': /* Jogadas de cartas */
		ret=0x10+c-'A';
		break;
		case 'T': /* Pedir truco */
		ret=0x23;
		break;
		case '6': /* Aumentar */
		case '9':
		ret=0x20+c-'0';
		break;
		case '1': /* Um possível 12 */
		ret=0x0c;
		break;
		case 10: return 0;
		case 'N': /* Resposta de Sim ou Não */
		case 'S':
		ret=0x40+(c=='S');break;
		default: ret=0;
	}
	if(ret/16){
		if(getchar()==10)
			return ret;
		pegueenter();
		return 0;
	}
	if(ret==12)
		if(getchar()=='2')
			if(getchar()==10)
				return 0x2c;
	pegueenter();
	return 0;
}

#ifndef COMPMOSTRA
#define COMPMOSTRA 0
#endif
#if ((COMPMOSTRA!=0)&&(COMPMOSTRA!=1))
#error COMPMOSTRA inválido!
#define COMPMOSTRA 0
#endif

void rodada()
{
	struct sit situacao;
	struct baralho b;
	int i;
	unsigned char escolha;
	inicio.rod=!inicio.rod;
	inicio.jog=inicio.rod;
	b=embaralhar(200,tipojogo);
	for(i=0;i<3;i++)
		Tela.usuario[i].valor=b.c[i].valor;
	for(i=0;i<3;i++)
		Tela.comp[i].valor=b.c[i+3].valor;
	Tela.compnumcartas=3;
	Tela.cartasnousu=7;
	Tela.cartasnamesa=0;
	Tela.compmostra=COMPMOSTRA;
	Tela.valorrod=1;
	situacao.valor=1;
	std.val=0x2a;
	situacao.mescolha=(Tela.pontosusu==11)+2*(Tela.pontoscomp==11);
	situacao.pediu=0;
	situacao.aberto=situacao.mescolha%3;
	situacao.pedinte=situacao.aberto%2;
	i=0;
	if(situacao.mescolha==3)
	{
		char c;
		meuputs("Primeira rodada às cegas");
		Tela.cartasnamesa=3;
		Tela.mesa[0].valor=Tela.usuario[2].valor;
		Tela.mesa[1].valor=Tela.comp[2].valor;
		Tela.cartasnousu=3;
		Tela.compnumcartas=2;
		c=compara(Tela.mesa[1],Tela.mesa[0]);
		std.rod.pri=(c>0)-(c<0);
		imprime(Tela);
		pegueenter();
		Tela.cartasnamesa=0;
		i=1;
	}
	for(;i<3;)
	{
		char c;
		if(situacao.aberto)
		{
			unsigned char escolha;
			do{
				imprime(Tela);
				escolha=situacao.aberto/2?jogada(Tela.comp,Tela.mesa[0],i,Tela.cartasnamesa,tipojogo,situacao,Tela.defman):entra(situacao);
			}
			while((escolha/16)!=4);
			if(escolha%16){
				meuputs("Aceito!");
				Tela.valorrod=situacao.valor;
				situacao.aberto=0;
				inicio.jog=!inicio.jog;
			}
			else{
				std.val=situacao.pedinte?0x15:0x3f;
				meuputs("Não aceitou...");
				imprime(Tela);
				pegueenter();
			}
		}
		else{
			unsigned char escolha;
			char tmp;
			char tr[4][8]={"Truco!","Seis!","NOVE!","DOZE!!!"};
			do{
				imprime(Tela);
				escolha=(inicio.jog)?jogada(Tela.comp,Tela.mesa[0],i,Tela.cartasnamesa,tipojogo,situacao,Tela.defman):entra(situacao);
			}
			while((((escolha/16)%8)!=1)&&(((escolha/16)%8)!=2));
			switch(escolha/16)
			{
				case 1:
				tmp=escolha%16;
				if(Tela.cartasnousu&(1<<tmp)){
					Tela.cartasnamesa++;
					Tela.mesa[0]=Tela.usuario[tmp];
					Tela.cartasnousu^=1<<tmp;
					Tela.cartasnamesa|=1;
					inicio.jog=!inicio.jog;
				}
				else meuputs("Essa carta já foi jogada!");
				break;
				case 9:
				Tela.mesa[1]=Tela.comp[2-i];
				Tela.compnumcartas--;
				Tela.cartasnamesa|=2;
				inicio.jog=!inicio.jog;
				break;
				case 2:
				tmp=escolha%16;
				tmp-=3;
				tmp+=!tmp;
				if(Tela.valorrod==tmp&&(situacao.pedinte||!situacao.pediu)){
					situacao.valor=escolha%16;
					situacao.pediu=1;
					situacao.pedinte=0;
					situacao.aberto=2;
					meuputs(tr[(situacao.valor/3)-1]);
					inicio.jog=!inicio.jog;
				}
				else meuputs("Indeferido");
				if(situacao.mescolha){
					meuputs("Truco com 11 pontos: Fim de Jogo!");
					Tela.pontoscomp=12;
					std.val=0x15;
				}
				break;
				case 10:
				situacao.valor=escolha%16;
				situacao.pediu=1;
				situacao.pedinte=1;
				situacao.aberto=1;
				inicio.jog=!inicio.jog;
				break;
			}
		}
		if(Tela.cartasnamesa==3)
		{
			c=compara(Tela.mesa[1],Tela.mesa[0]);
			c=(c>0)-(c<0);
			switch(i){
				case 0:
				std.rod.pri=c;
				if(c)inicio.jog=(c>0);
				break;
				case 1:
				std.rod.seg=c;
				inicio.jog=(c>0);
				break;
				case 2:
				std.rod.ter=c;
				break;
			}
			i++;
			imprime(Tela);
			pegueenter();
			Tela.cartasnamesa=0;
		}
		if(c=venceurodada(std.rod)){
			i=3;
			if(situacao.mescolha)
			{
				Tela.pontosusu+=(c<0)*2*(std.val!=0x3f);
				Tela.pontoscomp+=(c>0)*2*(std.val!=0x15);
			}
			Tela.pontosusu+=(c<0)*Tela.valorrod;
			Tela.pontoscomp+=(c>0)*Tela.valorrod;
		}
	}
}

int main (int argn, char * argv[])
{
	int le,g=0;
	if(argn==2)
		if(!strcmp("-g",argv[1]))
			g=1;
	tipojogo=escolhatipo(g,argv[0]);
	srand(time(NULL));
	for(le=0;le<3;le++)
		Tela.strings[le][0]='\0';
	Tela.compmostra=0;
	Tela.pontosusu=0;
	Tela.pontoscomp=0;
	Tela.mnova=0;
	Tela.compnumcartas=0;
	Tela.cartasnousu=0;
	Tela.cartasnamesa=0;
	Tela.valorrod=1;
	srand(time(NULL));
	inicio.rod=rand()%2;
	meuputs("Para jogar, digite A, B ou C e pressione Enter");
	meuputs("Para pedir truco e superiores, use T, 6, 9 e 12");
	meuputs("Pressione Enter para iniciar");
	imprime(Tela);
	pegueenter();
	Tela.mnova=tipojogo/2;
	do rodada();
	while((Tela.pontosusu<12)&&(Tela.pontoscomp<12));
	if(Tela.pontoscomp>12){
		Tela.pontoscomp=12;
		meuputs("Você PERDEU!");
	}
	else{
		Tela.pontosusu=12;
		meuputs("Você VENCEU!");
	}
	imprime(Tela);
	pegueenter();
	mudecor(0);
	return 0;
}