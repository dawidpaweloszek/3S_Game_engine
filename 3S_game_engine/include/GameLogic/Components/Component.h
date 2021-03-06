#pragma once
#ifndef COMPONENT_H
#define COMPONENT_H

namespace GameLogic
{
	enum ComponentType
	{
		C_MESH = 1,
		C_COLLIDER = 2,
		C_MOVEMENT = 3,
		C_INTERACTABLE = 4,
		C_TREASURE = 5,
		C_CASH = 6,
		C_ANIMA = 7,
		C_ENEMY = 8
	};

	class Proctor;

	class Component
	{
	public:
		Proctor* proctor;
		ComponentType type;

		/* Constructors / Destructors */
		Component();
		Component(ComponentType _type, Proctor* _proctor);
		virtual ~Component();

		virtual void update() {}
		virtual void cleanup() {}

		bool isActive();
		void setActive(bool _value);

	protected:
		bool active = true;
		unsigned int uuid = 0;
	};
}

#endif // !COMPONENT_H