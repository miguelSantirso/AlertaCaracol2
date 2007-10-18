#include "Efecto_Agua.h"

#include <allegro.h>
#include "Body.h"

using namespace std;

Efecto_Agua::Efecto_Agua(void)
{
	Esquina_Superior_Izquierda.x = 0; 
	Esquina_Superior_Izquierda.y = 300;

	Esquina_Inferior_Derecha.x = 640;
	Esquina_Inferior_Derecha.y = 480;

	Amplitud		= 10;
	Frecuencia		= 1;
	Densidad		= 0.3;
	Numero_Puntos	= 80;

	t=0;

	Inicializa();
}

Efecto_Agua::Efecto_Agua(int X_1, int Y_1, int X_2, int Y_2, float frecuencia, float amplitud, float densidad, int numero_puntos)
{
	Esquina_Superior_Izquierda.x = X_1; 
	Esquina_Superior_Izquierda.y = Y_1;

	Esquina_Inferior_Derecha.x = X_2;
	Esquina_Inferior_Derecha.y = Y_2;

	Frecuencia		= frecuencia;
	Amplitud		= amplitud;
	Densidad		= densidad;
	Numero_Puntos	= numero_puntos;

	t = 0;

	Inicializa();
}

Efecto_Agua::~Efecto_Agua(void)
{
	delete[] Puntos;
}

void Efecto_Agua::Inicializa()
{
	Puntos = new int[Numero_Puntos*2+4];
	Separacion_Puntos = 640.0/Numero_Puntos+0.5;

	Numero_Ola = new float[Numero_Puntos];
	Numero_Ola_Anterior = new float[Numero_Puntos];
	for(int i=0; i<Numero_Puntos; i++)
	{
		Numero_Ola[i] = 0;
		Numero_Ola_Anterior[i] = 0;
	}
}

void Efecto_Agua::Dibuja(BITMAP * Buffer)
{
	float Opacidad = 0.6;
	const int val = (int)(Opacidad * 0xff);
	set_trans_blender(0xff, 0xff, 0xff, val);
	drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);
	polygon(Buffer, Numero_Puntos+2, Puntos, makeacol(0, 120, 255, 255*Opacidad));
	drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);

//	polygon(Buffer, Numero_Puntos+2, Puntos, makecol(42, 129, 220));
/*	for(int i=2; i<(Numero_Puntos)/6; i++)
		spline(Buffer, (Puntos+i*6), makecol(0, 0, 255));*/
}

void Efecto_Agua::Dibuja_Fondo(BITMAP * Buffer)
{
	float Opacidad = 1.0;
	const int val = (int)(Opacidad * 0xff);
	set_trans_blender(0xff, 0xff, 0xff, val);
	drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);
	polygon(Buffer, Numero_Puntos+2, Puntos, makeacol(146, 148, 210, 255*Opacidad));
	drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);

//	polygon(Buffer, Numero_Puntos+2, Puntos, makecol(42, 129, 220));
/*	for(int i=2; i<(Numero_Puntos)/6; i++)
		spline(Buffer, (Puntos+i*6), makecol(0, 0, 255));*/
}

void Efecto_Agua::Actualiza()
{
	t++;	// Incrementar el contador del tiempo

	// Actualizar el número de ola, que sirve para extender las olas por la superficie del agua
	for(int j=0; j<Numero_Puntos; j++)
	{
		if(j>0 && j<Numero_Puntos-1)
			Numero_Ola[j] = (Numero_Ola_Anterior[j-1]+Numero_Ola_Anterior[j+1])/2;
		else if(j==0) Numero_Ola[j] = Numero_Ola_Anterior[j+1];
		else Numero_Ola[j] = Numero_Ola_Anterior[j-1];

		if(Numero_Ola[j] > 0.5)
			Numero_Ola[j] -= Numero_Ola[j]/300;

		Numero_Ola_Anterior[j] = Numero_Ola[j];
	}

	int Aux = 0; // Se usará para calcular la altura media del agua
	// Actualizar la forma del spline lineal que se dibujará
	Puntos[0] = Esquina_Superior_Izquierda.x;		// X_1
	Puntos[1] = Esquina_Inferior_Derecha.y;			// Y_1
	for(int i=0; i<Numero_Puntos*2+2; i+=2)
	{
		double f=Densidad*Numero_Ola[i/2];
		Puntos[i+2] = (i*(Separacion_Puntos/2));
		Puntos[i+3] = Esquina_Superior_Izquierda.y+Amplitud*sin(fix(f*(i*Separacion_Puntos)+t));
		Aux += Puntos[i+3];
	}
	Puntos[Numero_Puntos*2+2] = Esquina_Inferior_Derecha.x;	// X_2
	Puntos[Numero_Puntos*2+3] = Esquina_Inferior_Derecha.y;	// Y_2

	Altura_Media = Aux/Numero_Puntos;

	// Comprobar si algún Cuerpo colisiona con el agua
	vector<Body *>::iterator i;
	vector<vector<Body *>::iterator> Objetos_Eliminar;
	for(i = Cuerpos.begin(); i != Cuerpos.end(); i++)
	{
		if((*i)->position.y > Obtener_Altura((*i)->position.x))
		{
			Objetos_Eliminar.push_back(i);
			Modifica_Amplitud((*i)->position.x, (*i)->velocity.y);
		}
	}

	for(int i=0; i<Objetos_Eliminar.size(); i++)
		Cuerpos.erase(Objetos_Eliminar[i]);
}

int Efecto_Agua::Numero_Cuerpos()
{
	return Cuerpos.size();
}

float Efecto_Agua::Obtener_Altura_Media()
{
	return Altura_Media;
}

float Efecto_Agua::Obtener_Normal(int X)
{
	int Punto = (X*Numero_Puntos)/(Numero_Puntos*Separacion_Puntos); // Obtener el punto a modificar a partir de la coordenada horizontal
	float Angulo = atan( float(Puntos[Punto*2+3] - Puntos[Punto*2-1]) / float(Puntos[Punto*2+2] - Puntos[Punto*2-2]) );
	return Angulo;
}

int Efecto_Agua::Obtener_Altura(int X)
{
	int Punto = (X*Numero_Puntos)/(Numero_Puntos*Separacion_Puntos); // Obtener el punto a modificar a partir de la coordenada horizontal
	return Puntos[Punto*2+3];
}

void Efecto_Agua::Modifica_Amplitud(int X, float Cantidad)
{
	int Punto = (X*Numero_Puntos)/(Numero_Puntos*Separacion_Puntos); // Obtener el punto a modificar a partir de la coordenada horizontal
	if(Punto < Numero_Puntos)
		Numero_Ola_Anterior[Punto] += Cantidad;
}

void Efecto_Agua::Nuevo_Cuerpo(Body * Cuerpo)
{
	if(Cuerpo != NULL)
		Cuerpos.push_back(Cuerpo);
}

void Efecto_Agua::Eliminar_Cuerpo(Body *Cuerpo)
{
	vector<Body *>::iterator i = Cuerpos.begin();
	for(; i != Cuerpos.end() && *i != Cuerpo; i++);

	if(i != Cuerpos.end() && Cuerpos.size() > 0 && *i == Cuerpo)
		Cuerpos.erase(i);
}