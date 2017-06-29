#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define COR3f(r, g, b) glColor3f((float) r / 255, (float) g / 255, (float) b / 255);
#define CORCC(r, g, b, t) glClearColor((float) r, (float) g, (float) b, (float) t);
#define QUADRADO(x0, y0, h, l) glBegin(GL_QUADS); glVertex2f(x0, y0); glVertex2f(x0, y0 + h); glVertex2f(x0 + l, y0 + h); glVertex2f(x0 + l, y0); glEnd();
#define TRIANGULO(x0, y0, h, l) glBegin(GL_TRIANGLES); glVertex2f(x0, y0); glVertex2f(x0 + (l / 2), y0 + h); glVertex2f(x0 + l, y0); glEnd();
#define CIRCULO_PREENCHIDO(x0, y0, seg, raio) glBegin(GL_POLYGON); for(int i=0; i < seg; i++) glVertex2d(x0 + raio * sin(M_PI * 2 * i / seg), y0 + raio * cos(M_PI * 2 * i / seg)); glEnd();
#define OVAL_PREENCHIDO(x0, y0, seg, raio, des) glBegin(GL_POLYGON); for(int i=0; i < seg; i++) glVertex2d(x0 + (raio + des) * sin(M_PI * 2 * i / seg), y0 + raio * cos(M_PI * 2 * i / seg)); glEnd(); // Valores positivos desloca na horizontal, negativos na vertical
#define LINHA(w, x1, y1, x2, y2) glLineWidth(w); glBegin(GL_LINES); glVertex2f(x1, y1); glVertex2f(x2, y2); glEnd();
#define DISTANCIA(xa, xb, ya, yb) sqrt( pow(xa-xb,2) + pow(ya-yb,2) );

#define altura 600
#define largura 800

// DE = direita para esquerda / ED = esquerda para direita

// FUNCOES --------------------------------------------------------------------------------------------
// Ambiente e movimento
void keyboard(unsigned char tecla, int x, int y);
void tela(GLsizei w, GLsizei h);
void anima(int valor);
void construtor();
void fundo();
void sobreposicao();
// Escrita
void escrita(int x, int y, char *string);
// Submarino
void submarinoDE();
void submarinoED();
void submarino_barra(int x, int y);
void mini_submarinoDE(int x, int y, int n);
void mini_submarinoED(int x, int y, int n);
// Missil
void missilDE(int x, int y);
void missilED(int x, int y);
// Barra oxigenio
void barra_oxigenio();
// Mergulhador
void mergulhador_barra(int x, int y);
void mergulhadorDE(int x, int y, int n);
void mergulhadorED(int x, int y, int n);
// Tubarao
void tubaraoDE(int x, int y, int n);
void tubaraoED(int x, int y, int n);
// Colisao
float testa_colisao_submarino(int x, int y);
float testa_colisao_missil(int x, int y);
// Placar
void placar();
// Reset
void reset();
// Gerador
void gerador();
void reset_gera();

// VARIAVEIS GLOBAIS ----------------------------------------------------------------------------------
// Velocidade do movimento
int mov;
int vel = 150;
int passo = 5;
// Game Over
bool gameOver = false;
// Submarino ------
// Posicao
int xsub = 400;
int ysub = 300;
int xmsubDE = 0;
int xmsubED = largura;
// Passo
int passomsub = 7;
// Controle criacao
bool msub1de = false;
bool msub2de = false;
bool msub3de = false;
bool msub4de = false;
bool msub1ed = false;
bool msub2ed = false;
bool msub3ed = false;
bool msub4ed = false;
// Direcao
bool direita = true;
// Missil ------
// Movimento
int passomissil = 80;
int movmissil;
// Direcao
bool misdireita = true;
bool ok_mudanca = true;
// Controle criacao
bool atira = false;
// Posicao inicial
int xini;
int yini;
// Posicao colisao
int xmis;
// Barra de oxigenio ------
// Largura da barra
int lbarraOxi = 350;
// Mergulhador ------
// Movimento
int passomerg = 7;
// Resgatados
int resgatados = 0;
bool dimresgatados = true;
// Posicao
int xmergDE = 0;
int xmergED = largura;
// Controle criacao
bool merg1de = false;
bool merg2de = false;
bool merg3de = false;
bool merg4de = false;
bool merg1ed = false;
bool merg2ed = false;
bool merg3ed = false;
bool merg4ed = false;
// Tubarao ------
// Movimento
int passotub = 7;
// Posicao
int xtubDE = 0;
int xtubED = largura;
// Controle criacao
bool tub1de = false;
bool tub2de = false;
bool tub3de = false;
bool tub4de = false;
bool tub1ed = false;
bool tub2ed = false;
bool tub3ed = false;
bool tub4ed = false;
// Placar ------
int pontuacao = 0;
int vidas = 3;
// Troca de estados ------
int estado_sub = 0;
// Gerador ------
bool gera = true;
int count_gera = 0;

// FUNCOES PRINCIPAIS ---------------------------------------------------------------------------------
int main(int argc, char** argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(largura, altura);
	glutCreateWindow("SeaQuest");
	glutReshapeFunc(tela);
	glutDisplayFunc(construtor);

	glutTimerFunc(vel, anima, passo);
	glutMainLoop();
	return 0;
}

void tela(GLsizei w, GLsizei h) 
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// cria janela(esq, direita, em baixo, em cima)
	gluOrtho2D(0, largura, 0, altura);
	glMatrixMode(GL_MODELVIEW);
}

// MOVIMENTO -------------------------------------------------------------------------------------------
void anima(int valor)
{
	mov += valor;

	//randon = rand()%5;
	//printf("%d\n", randon);

	glutKeyboardFunc(&keyboard);
	glutPostRedisplay();
	glutTimerFunc(vel, anima, passo);
}

void keyboard(unsigned char tecla, int x, int y)
{
	int movsub = 8;
    
	if(tecla == 'e')
	{
		xini = xsub;
		yini = ysub;
		atira = true;
	} 

	if(tecla == 'w') 
    {
        if(ysub < 390) ysub += movsub;
    }
	else if(tecla == 's') 
    {
        if(ysub > 140) ysub -= movsub;
    }
	else if(tecla == 'd') 
	{
		direita = true;
		if(xsub < largura - 80) xsub += movsub;
	}
	else if(tecla == 'a') 
	{
		direita = false;
		if(xsub > 80)xsub -= movsub;
	}
}

// CONSTRUTORES ------------------------------------------------------------------------------------------
void construtor() 
{
	fundo();
	placar();
    if(!gameOver)
    {
		barra_oxigenio();

		gerador();

		int aux = -135;
		for(int i = 0; i < resgatados; i++)
		{
			mergulhador_barra((largura/2) + aux, 20);
			aux += 50;
		}

		int aux2 = 0;
		for(int i = 0; i < vidas; i++)
		{
			submarino_barra((largura/2) - 50 + aux2, 500);
			aux2 -= 80;
		}

		if(ok_mudanca) misdireita = direita;
		if(atira)
		{
			if(misdireita) missilDE(xini, yini);
			else missilED(xini, yini);
		}

		if(merg1de) mergulhadorDE(xmergDE, 155, 1);
		if(merg2de) mergulhadorDE(xmergDE, 220, 2);
		if(merg3de) mergulhadorDE(xmergDE, 285, 3);
		if(merg4de) mergulhadorDE(xmergDE, 350, 4);
		if(merg1ed) mergulhadorED(xmergED, 155, 1);
		if(merg2ed) mergulhadorED(xmergED, 220, 2);
		if(merg3ed) mergulhadorED(xmergED, 285, 3);
		if(merg4ed) mergulhadorED(xmergED, 350, 4);

		if(tub1de) tubaraoDE(xtubDE, 155, 1);
		if(tub2de) tubaraoDE(xtubDE, 220, 2);
		if(tub3de) tubaraoDE(xtubDE, 285, 3);
		if(tub4de) tubaraoDE(xtubDE, 350, 4);
		if(tub1ed) tubaraoED(xtubED, 155, 1);
		if(tub2ed) tubaraoED(xtubED, 220, 2);
		if(tub3ed) tubaraoED(xtubED, 285, 3);
		if(tub4ed) tubaraoED(xtubED, 350, 4);

		if(msub1de) mini_submarinoDE(xmsubDE, 155, 1);
		if(msub2de) mini_submarinoDE(xmsubDE, 220, 2);
		if(msub3de) mini_submarinoDE(xmsubDE, 285, 3);
		if(msub4de) mini_submarinoDE(xmsubDE, 350, 4);
		if(msub1ed) mini_submarinoED(xmsubED, 155, 1);
		if(msub2ed) mini_submarinoED(xmsubED, 220, 2);
		if(msub3ed) mini_submarinoED(xmsubED, 285, 3);
		if(msub4ed) mini_submarinoED(xmsubED, 350, 4);

		if(direita) submarinoDE();
		else submarinoED();
    }

	sobreposicao();
		
	glutSwapBuffers();
	//glFlush();
}

void fundo() 
{
	//Fundo mar
	COR3f(4, 24, 141);
	QUADRADO(0, 0, altura, largura);
	//Verde fundo
	COR3f(3, 97, 7);
	QUADRADO(0, 0, 120, largura);
	//Azul ceu
	COR3f(69, 75, 255);
	QUADRADO(0, (altura-180), 180, largura);
	//Fundo cinza
	COR3f(140, 138, 140);
	QUADRADO(0, 0, 80, largura);
	//Superficie
	COR3f(119, 142, 235);
	QUADRADO(0, 380, 40, largura);
	//Horizonte
	COR3f(225, 157, 235);
	QUADRADO(0, 420, 40, largura);
    //Linhas limite fundo mar
	COR3f(94, 64, 17);
	LINHA(3, 0, 120, largura, 120);
}

void sobreposicao()
{
	COR3f(119, 142, 235); // Azul claro
	QUADRADO(0, 380, 15, largura);
	COR3f(4, 24, 141); // Azul escuro
	QUADRADO(0, 390, 5, largura);
	COR3f(0, 0, 0); // Preto
	LINHA(3, 0, 380, largura, 380);
}

// ESCRITA -----------------------------------------------------------------------------------------------
void escrita(int x, int y, char *string)
{
	int len, i;
	glRasterPos2f(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++) {
		string[i] = toupper(string[i]);
		//glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
	glEnd();
}

// SUBMARINO ---------------------------------------------------------------------------------------------
void submarinoDE()
{
	int raio = 10;

	COR3f(217, 220, 66);
	OVAL_PREENCHIDO(xsub, ysub, 100, raio, 10);
	QUADRADO(xsub, (ysub-raio), 20, 30);
	OVAL_PREENCHIDO((xsub+35), ysub, 100, raio, 25);
	QUADRADO((xsub+25), ysub, 25, 20);
	TRIANGULO((xsub+15), ysub, 25, 20);

	LINHA(2.5 ,(xsub-32), ysub, (xsub-10), ysub);

	if(estado_sub > 5) estado_sub = 0;

	if(estado_sub == 0 || estado_sub == 1) 
	{
		LINHA(2.5 ,(xsub-32), (ysub+10), (xsub-10), (ysub+3));
		LINHA(2.5 ,(xsub-32), (ysub-10), (xsub-10), (ysub-3));
		estado_sub += 1;
	}
	else if(estado_sub == 2 || estado_sub == 3)
	{
		LINHA(2.5 ,(xsub-32), (ysub+5), (xsub-10), (ysub+3));
		LINHA(2.5 ,(xsub-32), (ysub-5), (xsub-10), (ysub-3));
		estado_sub += 1;
	}
	else if(estado_sub == 4 || estado_sub == 5) estado_sub += 1;

	COR3f(30, 49, 161);
	OVAL_PREENCHIDO((xsub+50), (ysub+4), 100, (raio-8), 4);
}

void submarinoED()
{
	int raio = 10;

	COR3f(217, 220, 66);
	OVAL_PREENCHIDO(xsub, ysub, 100, raio, 10);
	QUADRADO((xsub-30), (ysub-raio), 20, 30);
	OVAL_PREENCHIDO((xsub-35), ysub, 100, raio, 25);
	QUADRADO((xsub-35), ysub, 25, 20);
	TRIANGULO((xsub-25), ysub, 25, 20);

	LINHA(2.5 ,(xsub+32), ysub, (xsub+10), ysub);

	if(estado_sub > 5) estado_sub = 0;

	if(estado_sub == 0 || estado_sub == 1) 
	{
		LINHA(2.5 ,(xsub+32), (ysub+10), (xsub+10), (ysub+3));
		LINHA(2.5 ,(xsub+32), (ysub-10), (xsub+10), (ysub-3));
		estado_sub += 1;
	}
	else if(estado_sub == 2 || estado_sub == 3)
	{
		LINHA(2.5 ,(xsub+32), (ysub+5), (xsub+10), (ysub+3));
		LINHA(2.5 ,(xsub+32), (ysub-5), (xsub+10), (ysub-3));
		estado_sub += 1;
	}
	else if(estado_sub == 4 || estado_sub == 5) estado_sub += 1;

	COR3f(30, 49, 161);
	OVAL_PREENCHIDO((xsub-50), (ysub+4), 100, (raio-8), 4);
}

void submarino_barra(int x, int y)
{
	int raio = 10;

	COR3f(217, 220, 66);
	OVAL_PREENCHIDO(x, y, 100, raio, 10);
	OVAL_PREENCHIDO((x+15), y, 100, raio, 25);
	QUADRADO((x+15), y, 20, 15);
	TRIANGULO((x+8), y, 20, 15);

	//rabo
	LINHA(2.5 ,(x-28), y, (x-10), y);
	LINHA(2.5 ,(x-28), (y+7), (x-10), (y+3));
	LINHA(2.5 ,(x-28), (y-7), (x-10), (y-3));

	//COR3f(30, 49, 161);
	//OVAL_PREENCHIDO((x+30), (y+4), 100, (raio-8), 4);
}

void mini_submarinoDE(int x, int y, int n)
{
	int raio = 10;

	COR3f(243, 243, 243);
	OVAL_PREENCHIDO(x, y, 100, raio, 10);
	OVAL_PREENCHIDO((x+15), y, 100, raio, 25);
	QUADRADO((x+15), y, 20, 15);
	TRIANGULO((x+8), y, 20, 15);

	//rabo
	LINHA(2.5 ,(x-28), y, (x-10), y);
	//LINHA(2.5 ,(x-28), (y+7), (x-10), (y+3));
	//LINHA(2.5 ,(x-28), (y-7), (x-10), (y-3));

	if(estado_sub > 5) estado_sub = 0;

	if(estado_sub == 0 || estado_sub == 1) 
	{
		LINHA(2.5 ,(x-28), (y+7), (x-10), (y+3));
		LINHA(2.5 ,(x-28), (y-7), (x-10), (y-3));
	}
	else if(estado_sub == 2 || estado_sub == 3)
	{
		LINHA(2.5 ,(x-28), (y+4), (x-10), (y+3));
		LINHA(2.5 ,(x-28), (y-4), (x-10), (y-3));
	}

	COR3f(30, 49, 161);
	OVAL_PREENCHIDO((x+30), (y+4), 100, (raio-8), 4);

	if(xmsubDE < largura) xmsubDE += passomsub;
	else 
	{
		count_gera += 1;
		xmsubDE = 0;
		if(n == 1) 
		{
			msub1de = false;
		}
		else if(n == 2) 
		{
			msub2de = false;
		}
		else if(n == 3) 
		{
			msub3de = false;
		}
		else if(n == 4) 
		{
			msub4de = false;
		}
	}

	if(yini <= (y+raio) && yini >= (y-raio))
	{
		float dist = testa_colisao_missil(x,y);
		if(dist < 40)
		{
			count_gera += 1;
			pontuacao += 20;
			ok_mudanca = true;
			atira = false;
			movmissil = 0;
			if(n == 1) msub1de = false;
			else if(n == 2) msub2de = false;
			else if(n == 3) msub3de = false;
			else if(n == 4) msub4de = false;
		}
	}

	float distsub = testa_colisao_submarino(x,y);
	if(distsub < 40)
	{
		vidas -= 1;
		reset();
	}
}

void mini_submarinoED(int x, int y, int n)
{
	int raio = 10;

	COR3f(243, 243, 243);
	OVAL_PREENCHIDO(x, y, 100, raio, 10);
	OVAL_PREENCHIDO((x-15), y, 100, raio, 25);
	QUADRADO((x-30), y, 20, 15);
	TRIANGULO((x-22), y, 20, 15);

	//rabo
	LINHA(2.5 ,(x+28), y, (x+10), y);
	//LINHA(2.5 ,(x+28), (y+7), (x+10), (y+3));
	//LINHA(2.5 ,(x+28), (y-7), (x+10), (y-3));

	if(estado_sub > 5) estado_sub = 0;

	if(estado_sub == 0 || estado_sub == 1) 
	{
		LINHA(2.5 ,(x+28), (y+7), (x+10), (y+3));
		LINHA(2.5 ,(x+28), (y-7), (x+10), (y-3));
	}
	else if(estado_sub == 2 || estado_sub == 3)
	{
		LINHA(2.5 ,(x+28), (y+4), (x+10), (y+3));
		LINHA(2.5 ,(x+28), (y-4), (x+10), (y-3));
	}

	COR3f(30, 49, 161);
	OVAL_PREENCHIDO((x-30), (y+4), 100, (raio-8), 4);

	if(xmsubED > 0) xmsubED -= passomsub;
	else 
	{
		count_gera += 1;
		xmsubED = largura;
		if(n == 1) 
		{
			msub1ed = false;
		}
		else if(n == 2) 
		{
			msub2ed = false;
		}
		else if(n == 3) 
		{
			msub3ed = false;
		}
		else if(n == 4) 
		{
			msub4ed = false;
		}
	}

	if(yini <= (y+raio) && yini >= (y-raio))
	{
		float dist = testa_colisao_missil(x,y);
		if(dist < 40)
		{
			count_gera += 1;
			pontuacao += 20;
			ok_mudanca = true;
			atira = false;
			movmissil = 0;
			if(n == 1) msub1ed = false;
			else if(n == 2) msub2ed = false;
			else if(n == 3) msub3ed = false;
			else if(n == 4) msub4ed = false;
		}
	}

	float distsub = testa_colisao_submarino(x,y);
	if(distsub < 40)
	{
		vidas -= 1;
		reset();
	}
}

// MISSIL -----------------------------------------------------------------------------------------------
void missilDE(int x, int y)
{
	ok_mudanca = false;

	xmis = x + movmissil;

	COR3f(217, 220, 66);

	LINHA(2.5, xmis, y, (xmis + 20), y);

	if(xmis < largura)
	{
		movmissil += passomissil;
	}
	else
	{
		atira = false;
		movmissil = 0;
		ok_mudanca = true;
	}
}

void missilED(int x, int y)
{
	ok_mudanca = false;

	xmis = x - movmissil;

	COR3f(217, 220, 66);

	LINHA(2.5, xmis, y, (xmis + 20), y);

	if(xmis > 0)
	{
		movmissil += passomissil;
	}
	else
	{
		atira = false;
		movmissil = 0;
		ok_mudanca = true;
	}
}

// BARRA OXIGENIO ----------------------------------------------------------------------------------------
void barra_oxigenio()
{
    int hbarra = 45;
    int lbarra = 350;
    float xquad = ((largura/2)-(lbarra/2));

    char texto[10];
	sprintf(texto, "OXIGENIO");
	COR3f(0, 0, 0);
	escrita(xquad - 175, hbarra + 2, texto);

    // Barra inferior
    COR3f(170, 0, 0);
    QUADRADO(xquad, hbarra, 20, lbarra);
    // Barra superior
    COR3f(243, 243, 243);
    QUADRADO(xquad, hbarra, 20, lbarraOxi);

	if(lbarraOxi <= 0)
	{
		vidas -= 1;
		reset();
	}

    if(ysub > 390) 
    {
		if(dimresgatados)
		{
			if(resgatados == 0)
			{
				vidas -= 1;
				reset();
			}
			else if(resgatados == 6)
			{
				vidas = -1;
				reset();
			}
			else 
			{
				resgatados -= 1;
				dimresgatados = false;
			}
		}

        if(lbarraOxi <= lbarra) lbarraOxi += 10;
        if(lbarraOxi > lbarra) lbarraOxi = lbarra;
    }
    else 
	{
		lbarraOxi -= 2;
		dimresgatados = true;
	}
}

// MERGULHADOR ------------------------------------------------------------------------------------------
void mergulhador_barra(int x, int y)
{
    COR3f(0, 0, 160);
    //Pernas
    LINHA(4, x, y, x - 20, y - 5);
    LINHA(5, x, y, x - 15, y - 15);
    //Tronco
    LINHA(5, x, y, x + 15, y + 10);
    //Cabeca
    CIRCULO_PREENCHIDO(x + 17, y + 13, 100, 5);
    //Braco esticado
    LINHA(3, x + 5, y + 5, x + 5 + 23, y + 6);
    //Braco dobrado
    QUADRADO(x + 5, y + 5, 5, 15);
}

void mergulhadorDE(int x, int y, int n)
{
    COR3f(211, 215, 207);
    //Pernas
    LINHA(4, x, y, x - 20, y - 5);
    LINHA(5, x, y, x - 15, y - 15);
    //Tronco
    LINHA(5, x, y, x + 15, y + 10);
    //Cabeca
    CIRCULO_PREENCHIDO(x + 17, y + 13, 100, 5);
    //Braco esticado
    LINHA(3, x + 5, y + 5, x + 5 + 23, y + 6);
    //Braco dobrado
    QUADRADO(x + 5, y + 5, 5, 15);

    if(x < largura) xmergDE += passomerg;
    else 
	{
		count_gera += 1;
		xmergDE = 0;
		if(n == 1) 
		{
			merg1de = false;
		}
		else if(n == 2) 
		{
			merg2de = false;
		}
		else if(n == 3) 
		{
			merg3de = false;
		}
		else if(n == 4) 
		{
			merg4de = false;
		}
	}
    
    float dist = testa_colisao_submarino(x,y);
	if(dist < 80)
	{
		count_gera += 1;
		if(resgatados < 6) resgatados += 1;
		if(n == 1) merg1de = false;
		else if(n == 2) merg2de = false;
		else if(n == 3) merg3de = false;
		else if(n == 4) merg4de = false;
	}
}

void mergulhadorED(int x, int y, int n)
{
    COR3f(211, 215, 207);
    //Pernas
    LINHA(4, x, y, x + 20, y - 5);
    LINHA(5, x, y, x + 15, y - 15);
    //Tronco
    LINHA(5, x, y, x - 15, y + 10);
    //Cabeca
    CIRCULO_PREENCHIDO(x - 17, y + 13, 100, 5);
    //Braco esticado
    LINHA(3, x - 5, y + 5, x - 5 - 23, y + 6);
    //Braco dobrado
    QUADRADO(x - 5 - 15, y + 5, 5, 15);

    if(x > 0) xmergED -= passomerg;
    else 
	{
		count_gera += 1;
		xmergED = largura;
		if(n == 1) 
		{
			merg1ed = false;
		}
		else if(n == 2) 
		{
			merg2ed = false;
		}
		else if(n == 3) 
		{
			merg3ed = false;
		}
		else if(n == 4) 
		{
			merg4ed = false;
		}
	}
    
    float dist = testa_colisao_submarino(x,y);
	if(dist < 80)
	{
		count_gera += 1;
		if(resgatados < 6) resgatados += 1;
		if(n == 1) merg1ed = false;
		else if(n == 2) merg2ed = false;
		else if(n == 3) merg3ed = false;
		else if(n == 4) merg4ed = false;
	}
}

// TUBARAO ---------------------------------------------------------------------------------------------
void tubaraoDE(int x, int y, int n)
{
	int raio = 10;

	COR3f(246, 110, 231);
	OVAL_PREENCHIDO(x, y, 100, raio, 15);
	TRIANGULO(x, y + 5, 15, 15);
	CIRCULO_PREENCHIDO(x - 20, y, 100, raio);

	COR3f(4, 24, 141);
	OVAL_PREENCHIDO(x + 20, y - 4, 100, (raio-8), 4);
	OVAL_PREENCHIDO(x - 27, y, 100, (raio-5), 4);

	if(xtubDE < largura) xtubDE += passotub;
	else 
	{
		count_gera += 1;
		xtubDE = 0;
		if(n == 1) 
		{
			tub1de = false;
		}
		else if(n == 2) 
		{
			tub2de = false;
		}
		else if(n == 3) 
		{
			tub3de = false;
		}
		else if(n == 4) 
		{
			tub4de = false;
		}
	}

	if(yini <= (y+raio) && yini >= (y-raio))
	{
		float dist = testa_colisao_missil(x,y);
		if(dist < 40)
		{
			count_gera += 1;
			pontuacao += 20;
			ok_mudanca = true;
			atira = false;
			movmissil = 0;
			if(n == 1) tub1de = false;
			else if(n == 2) tub2de = false;
			else if(n == 3) tub3de = false;
			else if(n == 4) tub4de = false;
		}
	}

	float distsub = testa_colisao_submarino(x,y);
	if(distsub < 40)
	{
		vidas -= 1;
		reset();
	}
}

void tubaraoED(int x, int y, int n)
{
	int raio = 10;

	COR3f(246, 110, 231);
	OVAL_PREENCHIDO(x, y, 100, raio, 15);
	TRIANGULO(x - 5, y + 5, 15, 15);
	CIRCULO_PREENCHIDO(x + 20, y, 100, raio);

	COR3f(4, 24, 141);
	OVAL_PREENCHIDO(x - 20, y - 4, 100, (raio-8), 4);
	OVAL_PREENCHIDO(x + 27, y, 100, (raio-5), 4);

	if(xtubED > 0) xtubED -= passotub;
	else 
	{
		count_gera += 1;
		xtubED = largura;
		if(n == 1) 
		{
			tub1ed = false;
		}
		else if(n == 2) 
		{
			tub2ed = false;
		}
		else if(n == 3) 
		{
			tub3ed = false;
		}
		else if(n == 4) 
		{
			tub4ed = false;
		}
	}

	if(yini <= (y+raio) && yini >= (y-raio))
	{
		float dist = testa_colisao_missil(x,y);
		if(dist < 40)
		{
			count_gera += 1;
			pontuacao += 20;
			ok_mudanca = true;
			atira = false;
			movmissil = 0;
			if(n == 1) tub1ed = false;
			else if(n == 2) tub2ed = false;
			else if(n == 3) tub3ed = false;
			else if(n == 4) tub4ed = false;
		}
	}

	float distsub = testa_colisao_submarino(x,y);
	if(distsub < 40)
	{
		vidas -= 1;
		reset();
	}
}

// COLISAO ---------------------------------------------------------------------------------------------
float testa_colisao_submarino(int x, int y)
{
    float cat1;
	float cat2;
    float dist;

	if(xsub > x) cat1 = xsub-x;	
	else cat1 = x-xsub;

	if(ysub > y) cat2 = ysub-y;
	else cat2 = y-ysub;

	dist = sqrt( pow(cat1,2) + pow(cat2,2) );

    return dist;
}

float testa_colisao_missil(int x, int y)
{
    float cat1;
	float cat2;
    float dist;

	if(xmis > x) cat1 = xmis-x;	
	else cat1 = x-xmis;

	if(yini > y) cat2 = yini-y;
	else cat2 = y-yini;

	dist = sqrt( pow(cat1,2) + pow(cat2,2) );

    return dist;
}

// PLACAR -----------------------------------------------------------------------------------------------
void placar()
{
	char texto[10];
	sprintf(texto, "%d", pontuacao);
	COR3f(217, 220, 66);
	escrita(largura/2 + 40, altura-40, texto);
}

// RESET ------------------------------------------------------------------------------------------------
void reset()
{
	if(vidas < 0) gameOver = true;
	else
	{
		lbarraOxi = 350;
		// Submarino ------
		// Posicao
		xsub = 400;
		ysub = 300;
		// Direcao
		bool direita = true;
		// Missil ------
		// Movimento
		movmissil = 0;
		// Direcao
		misdireita = true;
		ok_mudanca = true;
		// Controle criacao
		atira = false;
		// Mergulhador ------
		// Resgatados
		if(resgatados > 0) resgatados -= 1;
		dimresgatados = true;
		// Posicao
		int xmergDE = 0;
		int xmergED = largura;
		// Controle criacao
		bool merg1de = false;
		bool merg2de = false;
		bool merg3de = false;
		bool merg4de = false;
		bool merg1ed = false;
		bool merg2ed = false;
		bool merg3ed = false;
		bool merg4ed = false;
		// Tubarao ------
		// Posicao
		int xtubDE = 0;
		int xtubED = largura;
		// Controle criacao
		bool tub1de = false;
		bool tub2de = false;
		bool tub3de = false;
		bool tub4de = false;
		bool tub1ed = false;
		bool tub2ed = false;
		bool tub3ed = false;
		bool tub4ed = false;
		// Controle criacao
		bool msub1de = false;
		bool msub2de = false;
		bool msub3de = false;
		bool msub4de = false;
		bool msub1ed = false;
		bool msub2ed = false;
		bool msub3ed = false;
		bool msub4ed = false;
		int xmsubDE = 0;
		int xmsubED = largura;
	}
}

// GERADOR ---------------------------------------------------------------------------------------------
void gerador()
{
	if(count_gera >= 4) 
	{
		reset_gera();
		gera = true;
	}
	if(gera)
	{
		count_gera = 0;
		gera = false;
		int rand1 = rand()%4;
		if(rand1 == 0)
		{
			int rand10 = rand()%2;
			if(rand10 == 0) merg1de = true;
			else merg1ed = true;
		}
		else if(rand1 == 1)
		{
			int rand11 = rand()%2;
			if(rand11 == 0) tub1de = true;
			else tub1ed = true;
		}
		else if(rand1 == 2)
		{
			int rand12 = rand()%2;
			if(rand12 == 0) msub1de = true;
			else msub1ed = true;
		}
		else count_gera += 1;

		int rand2 = rand()%4;
		if(rand2 == 0)
		{
			int rand20 = rand()%2;
			if(rand20 == 0) merg2de = true;
			else merg2ed = true;
		}
		else if(rand2 == 1)
		{
			int rand21 = rand()%2;
			if(rand21 == 0) tub2de = true;
			else tub2ed = true;
		}
		else if(rand2 == 2)
		{
			int rand22 = rand()%2;
			if(rand22 == 0) msub2de = true;
			else msub2ed = true;
		}
		else count_gera += 1;

		int rand3 = rand()%4;
		if(rand3 == 0)
		{
			int rand30 = rand()%2;
			if(rand30 == 0) merg3de = true;
			else merg3ed = true;
		}
		else if(rand3 == 1)
		{
			int rand31 = rand()%2;
			if(rand31 == 0) tub3de = true;
			else tub3ed = true;
		}
		else if(rand3 == 2)
		{
			int rand32 = rand()%2;
			if(rand32 == 0) msub3de = true;
			else msub3ed = true;
		}
		else count_gera += 1;

		int rand4 = rand()%4;
		if(rand4 == 0)
		{
			int rand40 = rand()%2;
			if(rand40 == 0) merg4de = true;
			else merg4ed = true;
		}
		else if(rand4 == 1)
		{
			int rand41 = rand()%2;
			if(rand41 == 0) tub4de = true;
			else tub4ed = true;
		}
		else if(rand4 == 2)
		{
			int rand42 = rand()%2;
			if(rand42 == 0) msub4de = true;
			else msub4ed = true;
		}
		else count_gera += 1;

		printf("%d\n", count_gera);
	}
}

void reset_gera()
{
	// Controle criacao
	msub1de = false;
	msub2de = false;
	msub3de = false;
	msub4de = false;
	msub1ed = false;
	msub2ed = false;
	msub3ed = false;
	msub4ed = false;
	// Posicao
	xmsubDE = 0;
	xmsubED = largura;
	// Controle criacao
	merg1de = false;
	merg2de = false;
	merg3de = false;
	merg4de = false;
	merg1ed = false;
	merg2ed = false;
	merg3ed = false;
	merg4ed = false;
	// Posicao
	xmergDE = 0;
	xmergED = largura;
	// Controle criacao
	tub1de = false;
	tub2de = false;
	tub3de = false;
	tub4de = false;
	tub1ed = false;
	tub2ed = false;
	tub3ed = false;
	tub4ed = false;
	// Posicao
	xtubDE = 0;
	xtubED = largura;
}