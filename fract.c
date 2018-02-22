/* 
 * fract.c
 *
 * Simple visualizador de conjuntos de Julia y caos en opengl 
 * todas las funciones aritmeticas fueron implementadas.
 * los colores estan dados por la tablita que esta ahi que genere con otro
 * programa ue se llama genbox.c que te genera 2 conjuntos invertibles (funcion biyectiva)
 * usando una entropia para poder hacer difusion y confusion de datos , pero en este caso solamente me tome
 * uno de los conjuntos.. pero esto fue removido ya que se ve mas bonito con pocos colores... 
 * si quieres usarlo entonces en glColor3b() metele argumentos en funcion con el arreglo colorbox
 * esto simplemente es 
 * z = z^2 + c   donde z y c estan en C
 * puedes subirle el grado .. solo lee el codigo , hay una parte donde comento esto..
 * Espero lo disfruten , como yo disfruto haciendolo
 * para compilar.. es
 * $ gcc fract.c -Wall -pedantic -s -O3 -fomit-frame-pointer -funroll-loops -lGL -lGLU -lglut -o fract
 * la Licencia de esto es BSD y el Copyright de Eduardo Ruiz Duarte
 * $ ./fract
 * TODO:
 * Interfaz GTK para generarlos mas facilmente y escoger la gamma personalizada , asi como los tipos de fractales
 * rduarte@ciencias.unam.mx
 */
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

typedef struct cdouble
{
  double real, imag;
} cdouble_t;

typedef struct base_t
{
  double inicio;
  double final;
} base;
int rs, gs, bs;			/* Escalas a multiplicar por colores (seran aleatorias en teoria) */
base deltax, deltay;
/* generado con genbox.c */
unsigned char colorbox[256] = {
  150, 44, 234, 191, 33, 181, 83, 59, 75, 125, 112, 132, 41, 36, 221, 183,
  188, 186, 216, 22, 15, 105, 48, 208, 151, 204, 172, 203, 153, 16, 100, 242,
  144, 39, 84, 34, 90, 40, 10, 79, 140, 143, 189, 86, 67, 223, 106, 163,
  76, 102, 218, 246, 77, 146, 117, 30, 147, 17, 244, 19, 180, 142, 131, 248,
  160, 212, 63, 241, 25, 1, 167, 209, 139, 225, 177, 62, 137, 56, 27, 217,
  193, 35, 138, 65, 247, 219, 54, 166, 129, 64, 205, 202, 7, 187, 168, 252,
  238, 170, 107, 240, 158, 213, 169, 228, 159, 227, 51, 134, 141, 94, 128, 9,
  171, 123, 57, 190, 152, 232, 85, 71, 233, 108, 237, 182, 120, 197, 31, 2,
  60, 157, 68, 236, 91, 80, 149, 104, 161, 155, 92, 194, 28, 58, 4, 55,
  43, 115, 164, 121, 38, 81, 199, 211, 178, 101, 251, 110, 11, 253, 93, 72,
  254, 148, 49, 32, 119, 114, 113, 130, 175, 13, 239, 133, 179, 196, 88, 250,
  165, 162, 109, 215, 74, 245, 89, 235, 176, 82, 184, 116, 118, 5, 45, 50,
  73, 198, 26, 14, 3, 37, 8, 222, 214, 98, 12, 46, 229, 156, 18, 255,
  122, 226, 243, 231, 192, 124, 201, 97, 24, 21, 20, 111, 145, 174, 66, 207,
  95, 99, 70, 210, 230, 135, 78, 47, 200, 127, 224, 220, 206, 52, 0, 96,
  69, 136, 42, 185, 29, 195, 126, 87, 173, 154, 23, 103, 249, 6, 53, 61,
};
int ejex = 768;			/* Adecualo al size de tu screen pero a lo ancho , de preferencia que sea cuadrado */
int ejey = 768;
cdouble_t cons;
void
crea_plano ()
{
  gluOrtho2D (0, ejex, 0, ejey);
}
int cs=0;
void
mouse (int button, int state, int x, int y)
{
  /*
   * Es necesario que ejex y ejey globales 
   * sean los valores maximos de gluOrtho2D para
   * que cuando le des click sea realmente donde quieres
   * hacer el zoom.
   */
cs++;
  double muevex, muevey;
  if(cs%2 == 0)
   return;
  muevex = (deltax.final - deltax.inicio);
  muevey = (deltay.final - deltay.inicio);

  y = ejex - y;

  if (state == GLUT_DOWN)
    {
      switch (button)
	{
	case GLUT_LEFT_BUTTON:	/* disminuye 10 */
	  deltax.final = x * muevex / ejey + muevex / 10 + deltax.inicio;
	  deltax.inicio = x * muevex / ejey - muevex / 10 + deltax.inicio;
	  deltay.final = y * muevey / ejex + muevey / 10 + deltay.inicio;
	  deltay.inicio = y * muevey / ejex - muevey / 10 + deltay.inicio;

	  break;
	case GLUT_RIGHT_BUTTON:	/* lo aumenta 5 */
	  deltax.final = x * muevex / ejey + muevex * 5 + deltax.inicio;
	  deltax.inicio = x * muevex / ejey - muevex * 5 + deltax.inicio;
	  deltay.final = y * muevey / ejex + muevey * 5 + deltay.inicio;
	  deltay.inicio = y * muevey / ejex - muevey * 5 + deltay.inicio;
	  break;
	}
    }

  printf ("deltax.final = %f\tdeltax.inicio=%f\n", deltax.final,
	  deltax.inicio);
  printf ("deltay.final = %f\tdeltay.inicio=%f\n", deltay.final,
	  deltay.inicio);
  printf ("x = %d\ty = %d\n", x, y);
  glutPostRedisplay();
}



void
display (void)
{
  cdouble_t z;
  double i, j, temp, rgb_principal;
  int kolor = 0;
  glColor3f (0.0, 0.0, 0.0);
  glClear (GL_COLOR_BUFFER_BIT);
  glColor3f (1.0, 1.0, 1.0);
  glBegin (GL_POINTS);
  for (i = 0; i <= ejex; i += 1)
    {
      for (j = 0; j < ejey; j += 1)
	{
	  z.real = i * (deltax.final - deltax.inicio) / ejey + deltax.inicio;
	  z.imag = j * (deltay.final - deltay.inicio) / ejex + deltay.inicio;
/* Colores con mas resolucion , haciendo el delta del vector (r,g,b) (en este caso es r=g=b con datos aleatorios)  mas chico */

	  while ((kolor++ < 255) && ((hypot (z.real, z.imag)) <= 4.0))

	    {
	      temp = (z.real * z.real - z.imag * z.imag) + cons.real;
	      z.imag = (2 * z.real * z.imag) + cons.imag;
	      z.real = temp;

	    }
	  rgb_principal = kolor / 255.0;
	  glColor3d (rgb_principal * (rs % 16), rgb_principal * (gs % 16),
		     rgb_principal * (bs % 16));
	  glVertex2d (i, j);
	  kolor = 0;
	}
    }

  glEnd ();
  glutSwapBuffers();
}

void
crea_escalas_colores ()
{
  int fd;
  unsigned char buf[3];
  if ((fd = open ("/dev/urandom", O_RDONLY, 600)) < 0)
    {
      perror ("No pude abrir urandom open");
      exit (-1);
    }
  read (fd, &buf, sizeof (buf));
  rs = buf[0];
  gs = buf[1];
  bs = buf[2];
  close (fd);
  return;
}
void reshape(int x, int y) {
}

int
main (int argc, char **argv)
{
  deltax.inicio = -1.8;
  deltax.final = 1.8;
  deltay.inicio = -1.8;
  deltay.final = 1.8;


  if (argc < 3)
    exit (printf
	  ("Necesitas dar un numero complejo como argumento\nej: si quieres c=i entonces arg1 = 0 y arg2 = 1\n entonces:\n# %s -0.123 0.745 -r\nel -r al final es para asignar rgb aleatorio, no pongas -r para obtener la configuracion favorita del autor\npara mas valores de fractales de julia checar http://mathworld.wolfram.com/JuliaSet.html\nrduarte@ciencias.unam.mx\n",
	   argv[0]));
  cons.real = strtod (argv[1], NULL);
  cons.imag = strtod (argv[2], NULL);
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA);
  glutInitWindowPosition (0, 0);
  glutInitWindowSize (ejex, ejey);
  glutCreateWindow ("Julia");
  rs = 9;
  gs = 5;
  bs = 15;
  crea_plano ();
  if (argv[3] != NULL)
    if (strcmp (argv[3], "-r") == 0)
      {
	printf ("Asignando colores aleatorios\n");
	crea_escalas_colores ();
	printf ("r=%d\tg=%d\tb=%d\n", rs % 16, gs % 16, bs % 16);
      }
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
  glutMouseFunc (mouse);
  glutMainLoop ();
  return 0;
}
