#include "Objeto_Nave.h"

#include <allegro.h>
#include <string>
#include <map>

#include "Body.h"
#include "Joint.h"
#include "World.h"
#include "Objeto_Paracaidas.h"
#include "Objeto_Disparo.h"
#include "Efecto_Agua.h"

using namespace std;

// Obtener variables globales
extern vector<Joint *> joints;
extern vector<Body *> bodies;
extern vector<Objeto_Disparo *> Disparos;
extern vector<Objeto_Fisico *> Objetos_Fisicos;
extern World world;
extern Efecto_Agua * Puntero_Agua;

extern int Puntuacion;
extern int Agua_Restante;
extern SAMPLE *Cargando;
extern map<string, int> Teclas;

// Variables globales de la clase (esto no es nada correcto)
int Dosificador_Disparo = 0;
int Dosificador_Carga = 0;
int Dosificador_Propulsion = 0;

bool Recarga_Activa = false;

// Constructor por defecto de la clase; siempre se usar� �ste.
Objeto_Nave::Objeto_Nave(void)
{
	// Inicializamos el objeto
	Inicializa("graficos\\nave.pcx");

	// Modificamos los par�metros f�sicos del objeto
	Puntero_Box->mass=1500.0f;
	Puntero_Box->friction = 0.2;

	// Creamos el paraca�das
	Puntero_Paracaidas1 = new Objeto_Paracaidas();
	Objetos_Fisicos.push_back(Puntero_Paracaidas1);

	// Unimos el paraca�das a la nave
	Crea_Cuerda(20);
	Crea_Cuerda(-20);
}

// Destructor; no hace falta hacer nada porque llama impl�citamente al del padre
Objeto_Nave::~Objeto_Nave(void)
{
}

// Funci�n auxiliar que crea una uni�n entre el paraca�das y la nave
void Objeto_Nave::Crea_Cuerda(int Despl_Horizontal)
{
	Body *b1=Puntero_Box;
	Body *b2=Puntero_Paracaidas1->Puntero_Box;

	int SemiDistancia=-1*(b1->position.y-b2->position.y)/2;

    Joint *j = new Joint();
	j->Set(b1, b2, b1->position + Vec2(Despl_Horizontal, SemiDistancia));
	world.Add(j);
	joints.push_back(j);
    
	j = new Joint();
	j->Set(b1, b2, b1->position + Vec2(Despl_Horizontal, SemiDistancia));
	world.Add(j);
	joints.push_back(j);

}

// Funci�n que indica al objeto que debe subir (se est� propulsando hacia arriba)
void Objeto_Nave::Sube(void)
{
	// Si queda agua
	if(Agua_Restante > 1)
	{
		// Indicamos al paraca�das que suba
		Puntero_Paracaidas1->Sube();

		// Hace que se gaste menos agua
		if(Dosificador_Propulsion>1)
		{
			Agua_Restante-=3;
			Dosificador_Propulsion=0;
		}
		else
			Dosificador_Propulsion++;
	}
}

// Funci�n que indica al objeto que debe disparar hacia un lado
void Objeto_Nave::Disparo_Lateral(bool Derecha)
{
	// Si queda algo de agua
	if(Agua_Restante > 1)
	{
		// Creamos una fuerza lateral
		Puntero_Box->AddForce((Derecha ? -1 : 1)*300.0 * Vec2(1, 0));

		// Hace que los disparos salgan algo separados
		if(Dosificador_Disparo>0)
		{
			Agua_Restante-=1;
			Dosificador_Disparo=0;

			// Crear el disparo
			Objeto_Disparo * Disparo = new Objeto_Disparo(
				Puntero_Box->position.x+(Derecha ? Grafico_Objeto->w/2 : -Grafico_Objeto->w/2), 
				Puntero_Box->position.y, Vec2((Derecha ? 1 : -1), 0));

			Disparos.push_back(Disparo);
		}
		else
			Dosificador_Disparo++;
	}
}

// Actualiza la l�gica de la nave
void Objeto_Nave::Actualiza()
{
	
	if(!key[Teclas["left"] >> 8] && !key[Teclas["right"] >> 8])
		Dosificador_Disparo=2;

	// Chapuza que tuve que hacer porque a veces se descontrolaba la nave (es lo malo que tiene trabajar con f�sicas)
	// En esos casos reinicializamos todas las variables f�sicas y deber�a recuperarse el control de la nave
	if(abs(Puntero_Box->velocity.x)>40.0f)
	{
		world.arbiters.clear();
		Puntero_Box->velocity.x=0;
		Puntero_Paracaidas1->Puntero_Box->force.x=0;
		Puntero_Box->velocity.y=0;
		Puntero_Paracaidas1->Puntero_Box->force.y=0;
		Puntero_Box->rotation = 0;
		Puntero_Paracaidas1->Puntero_Box->rotation = 0;
		Puntero_Paracaidas1->Puntero_Box->position = Puntero_Box->position;
		Puntero_Paracaidas1->Puntero_Box->position.y -= 25;
	}

	// Hacer que la nave flote en el agua
	if(Puntero_Box->position.y>Puntero_Agua->Obtener_Altura(Puntero_Box->position.x))
	{
		Body * PBC = Puntero_Paracaidas1->Puntero_Box;	// Puntero a la caja de colisiones del paraca�das
		float Rotacion_Agua = Puntero_Agua->Obtener_Normal(Puntero_Box->position.x);

		Puntero_Box->position.y=Puntero_Agua->Obtener_Altura(Puntero_Box->position.x);
		Puntero_Box->rotation = Rotacion_Agua;
		PBC->position = Puntero_Box->position - 25*Vec2(sin(-Rotacion_Agua), cos(-Rotacion_Agua));
		PBC->rotation = Rotacion_Agua;
	}

	// Controlar cuando la nave est� recargando
	if(Puntero_Box->position.y>Puntero_Agua->Obtener_Altura_Media()-15)
	{
		if(Agua_Restante < NIVEL_MAXIMO_AGUA)
		{
			// Reproducir sonido
			if(!Recarga_Activa)
			{
				play_sample(Cargando, 255, 0, 500, 1);
				Recarga_Activa=true; // Indica que ya se ha lanzado el sonido
			}

			// Para que cargue m�s lento
			if(Dosificador_Carga>3)
			{
				Agua_Restante+=2;
				Dosificador_Carga=0;
			}
			else
				Dosificador_Carga++;
		}
	}
	else
	{
		if(Recarga_Activa) // Si est� sonando la recarga
		{
			stop_sample(Cargando); // La paramos
			Recarga_Activa = false;
		}
	}

	// Actualizamos la l�gica del paracaidas
	Puntero_Paracaidas1->Actualiza();
}