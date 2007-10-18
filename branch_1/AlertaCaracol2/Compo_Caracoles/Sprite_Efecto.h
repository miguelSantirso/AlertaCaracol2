#pragma once

struct BITMAP;

class Sprite_Efecto
{
public:
	Sprite_Efecto(void);
	Sprite_Efecto(BITMAP *Sprite, int x, int y, int D=1);
	~Sprite_Efecto(void);

	void Dibuja(BITMAP *);

	bool Destruir;

protected:
	BITMAP *Sprite;
	int Pos_X;
	int Pos_Y;
	int Decremento;

	int Alfa;
};
