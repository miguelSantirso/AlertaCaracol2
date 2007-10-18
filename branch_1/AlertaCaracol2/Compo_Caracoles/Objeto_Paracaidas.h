#pragma once
#include "objeto_fisico.h"

class Objeto_Paracaidas :
	public Objeto_Fisico
{
friend class Objeto_Nave;
public:
	Objeto_Paracaidas(void);
	~Objeto_Paracaidas(void);

	void Actualiza(); // Método que actuliza ciertos comportamientos del objeto
	void Sube();
};
