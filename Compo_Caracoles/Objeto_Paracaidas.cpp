#include <map>
#include <string>

#include "Objeto_Paracaidas.h"

#include "Body.h"
#include "World.h"

#include "allegro.h"

extern std::map<std::string, int> Teclas;

Objeto_Paracaidas::Objeto_Paracaidas(void)
{
	Inicializa("graficos\\paracaidas.pcx");
	Puntero_Box->mass=1000.0f;
	Puntero_Box->position.y-=25;
}

Objeto_Paracaidas::~Objeto_Paracaidas(void)
{
}

void Objeto_Paracaidas::Actualiza()
{
	if(Puntero_Box->velocity.y>0 && !key[Teclas["up"] >> 8]) // Si el objeto está cayendo
	{
		Puntero_Box->velocity.y-=Puntero_Box->velocity.y/4; // El paracaídas funciona y la velocidad se decrementa
		Puntero_Box->angularVelocity-=Puntero_Box->angularVelocity/2; // El paracaídas funciona y la velocidad se decrementa
	}
}

void Objeto_Paracaidas::Sube()
{
	Puntero_Box->AddForce(-3000.0 * Vec2(0, 1));
}