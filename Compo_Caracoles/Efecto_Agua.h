#pragma once

#include <vector>

struct BITMAP;
struct Body;

class Efecto_Agua
{
	struct Punto
	{
		int x;
		int y;
	};
public:
	Efecto_Agua(void);
	Efecto_Agua(int X_1, int Y_1, int X_2, int Y_2, float Frecuencia=1, float Amplitud=10, float Densidad=0.3, int Numero_Puntos=80);
	~Efecto_Agua(void);

	void Dibuja(BITMAP * Buffer);
	void Dibuja_Fondo(BITMAP * Buffer);
	void Actualiza();

	int Numero_Cuerpos();			// Retorna el número de cuerpos que se están teniendo en cuenta (tamaño de Cuerpos)
	float Obtener_Altura_Media();	// Retorna la altura media de la curva
	float Obtener_Normal(int X);	// Retorna la normal de la curva en el punto indicado
	int Obtener_Altura(int X);		// Retorna la altura de la curva en el punto indicado

	void Modifica_Amplitud(int i, float Cantidad=75);
	void Nuevo_Cuerpo(Body * Cuerpo);	// Añade un cuerpo que interactuará con el sistema de agua
	void Eliminar_Cuerpo(Body * Cuerpo);// Elimina el cuerpo indicado del vector de cuerpos

protected:
	void Inicializa();

	float * Numero_Ola;
	float * Numero_Ola_Anterior;

	int * Puntos;

	std::vector<Body *> Cuerpos;	// Cuerpos físicos que interactuarán con el agua

	int t;

	float Amplitud;
	float Densidad;
	float Frecuencia;
	int Numero_Puntos;
	float Separacion_Puntos;
	float Altura_Media;
	Punto Esquina_Superior_Izquierda;
	Punto Esquina_Inferior_Derecha;
};
