#pragma once
#include "objeto_fisico.h"

#include "MathUtils.h"

// Clase que representa los disparos de agua de la nave
class Objeto_Disparo :
	public Objeto_Fisico
{
public:
	// Constructores y destructor
	Objeto_Disparo(void);
	Objeto_Disparo(int X, int Y, Vec2 Direccion);
	~Objeto_Disparo(void);

	void Actualiza();	// Actualiza la lógica del juego

	bool Destruir;	// Indica que el objeto debe ser destruido tan pronto como sea posible
};
