#include <stdlib.h>

struct sit {
	unsigned pediu :1;
	unsigned pedinte :1;
	unsigned valor :4;
	unsigned aberto :2;
	unsigned mescolha :2;
};

typedef union {
	unsigned valor :9;
	struct {
		unsigned n :2; 
		unsigned f :4; /*4,5,6,7,q,j,k,1,2,3*/
		unsigned m :3;
		/*detalhes: naipe, face, manilha */
	}d;
} Carta;

typedef struct rodada { //enum {naojogou=-2,perdeu,cangou,venceu};
	signed pri :2;
	signed seg :2;
	signed ter :2;
}Rodada;
struct rodada stdrod;

Carta impossiveis[7];
unsigned int imp=0;

struct chanceroda{
	float ven;
	float per;
};

struct baralho{
	Carta c[40];
	unsigned tam :6;
	unsigned pos :6;
};

struct baralho embaralhar(int n,char bmax);

float rand1()
{
	return (float)(((unsigned int)rand()%65536)/65536.0);
}

float randgauss(unsigned int n,float c)
{
	float ret=0,a=2-4*c;
	int i;
	if(a>1){
		for(i=0;i<30;i++)
			a-=(a*a-c)/(2*a);
		ret=randgauss(n,a);
		ret*=ret;
		return ret;
	}
	if(a<-1){
		for(i=0;i<30;i++)
			a-=(a*(2-a)-c)/(2-2*a);
		ret=randgauss(n,a);
		ret*=2-ret;
		return ret;
	}
	for(i=0;i<n;i++)
		ret+=rand1();
	ret/=(float)i;
	ret*=a*ret-a+1;
	return ret;
}

char inicializarodada(struct rodada *r)
{
	r->pri=r->seg=r->ter=-2;
}

char impossivel(Carta c, Carta* v,unsigned int n)
{
	int i;
	for(i=0;i<n;i++)
		if(c.valor==v[i].valor)return 1;
	return 0;
}

char venceurodada(Rodada r);

char estrategiavence(Carta *usu,Carta *comp){
	char c;
	int r;
	c=compara(usu[2],comp[0]);
	r=(c>0)-(c<0);
	c=compara(usu[1],comp[1]);
	r+=(c>0)-(c<0);
	if(r) return c;
	c=compara(usu[0],comp[2]);
	return (c>0)-(c<0);
}

float chance(Carta *c,Carta *u,char usu,char tipojogo,Carta *im,unsigned int ni)
{
	int i=0,j;
	float ret=0;
	Carta us[3];
	if(usu==3){
		if(estrategiavence(c,u)>0) ret++;
		i++;
		//wprintf(L"%d,%d,%d: %f\n",compara(c[2],u[0]),compara(c[1],u[1]),compara(c[0],u[2]),ret/i);
	}
	else for(j=0;j<3;j++){
		if(usu==j){
			int k;
			struct baralho b;
			tipojogo-=tipojogo/2;
			b=embaralhar(0,tipojogo);
			for(k=0;k<b.tam;k++){
				u[j].valor=b.c[k].valor;
				if(impossivel(u[j],im,ni))continue;
				ret+=chance(c,u,usu+1,tipojogo,im,ni);
				i++;
			}
		}
	}
	return (float)ret/i;
}

void trocacarta(Carta *a,Carta *b)
{
	Carta tmp=*a;
	*a=*b;
	*b=tmp;
}

void permutar(Carta *c,int n)
{
	n%=6;
	if(n%2)trocacarta(c,c+1);
	trocacarta(c+2,c+2-n/2);
}

unsigned short jogada(Carta comp[3],Carta mesa,int rodada,int cartasnamesa,char tipojogo,struct sit s,Carta defman)
{
	static Carta usuario[3];
	int i;
	static int n=20;
	rodada%=3;
	cartasnamesa%=2;
	if (rodada==0){
		for(i=0;i<2;i++)
			impossiveis[i].valor=comp[i].valor;
		imp=3;
		if(tipojogo==2){
			impossiveis[3].valor=defman.valor;
			imp=4;
		}
	}
	{
		float perm[6],ja,g;
		int maxp,escolhida=0;
		if(rodada+cartasnamesa>0){
			usuario[rodada+cartasnamesa-1].valor=mesa.valor;
			if(impossiveis[imp-1].valor!=mesa.valor){
				impossiveis[imp].valor=mesa.valor;
				imp++;
			}
		}
		maxp=6-(4*rodada*rodada)%11;//(rodada==0)?6:((rodada==1)?2:0);
		for(i=0;i<maxp;i++)
		{
			int j;
			Carta c[3];
			for(j=0;j<3;j++)
				c[j]=comp[j];
			permutar(c,i);
			perm[i]=chance(c,usuario,rodada+cartasnamesa%2,tipojogo,impossiveis,imp);
		}
		for(i=0;i<maxp;i++)
			if(perm[i]>perm[escolhida])escolhida=i;
		permutar(comp,escolhida);
		maxp=s.valor;
		maxp/=3;
		maxp+=1;
		maxp-=s.aberto;
		maxp*=3;
		if(s.aberto){
			if(s.mescolha&&perm[escolhida]>randgauss(100,0.5))
				return 0x41;
			if(perm[escolhida]>randgauss(10,1-1/(float)maxp))
				return 0x41;
			return 0x40;
		}
		if(perm[escolhida]>randgauss(n,1-1/(float)maxp)){
			if(!s.pediu&&!s.mescolha)
				return 0xa0+maxp;
		}
		return 0x90;
	}
}