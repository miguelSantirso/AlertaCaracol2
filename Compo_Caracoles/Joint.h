/*
* Copyright (c) 2006 Erin Catto http://www.gphysics.com
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies.
* Erin Catto makes no representations about the suitability
* of this software for any purpose.
* It is provided "as is" without express or implied warranty.
*/

/*
* Adapted to Allegro and slightly changed by Johan Peitz
* http://www.freelunchdesign.com
*/

#ifndef JOINT_H
#define JOINT_H

#include "MathUtils.h"

struct Body;

struct Joint
{
	Joint() : body1(0), body2(0), accumulatedImpulse(0.0f, 0.0f), relaxation(1.0f) {}

	void Set(Body* body1, Body* body2, const Vec2& anchor);

	void PreStep(float inv_dt);
	void ApplyImpulse();

	Mat22 M;
	Body* body1;
	Body* body2;
	Vec2 localAnchor1, localAnchor2;
	Vec2 r1, r2;
	Vec2 bias;
	Vec2 accumulatedImpulse;
	float relaxation;
};

#endif
