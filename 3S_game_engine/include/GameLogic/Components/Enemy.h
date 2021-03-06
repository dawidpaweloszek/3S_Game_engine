#pragma once
#include <string>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include "GameLogic/Components/Component.h"
#include "../../IrrKlang/irrKlang.h"



namespace GameLogic
{
	enum EnemyState
	{
		STOIC_STATE = 1,
		CHASING_STATE = 2,
		ATTACK_STATE = 3,
		DEAD_STATE = 5
	};

	class Enemy : public Component
	{
	public:
		//irrklang::ISoundEngine* enemyEngine;
		/* Constructors/Destructors */
		Enemy(Proctor* _proctor, Proctor* _playerOneRef, Proctor* _playerTwoRef, int _islandID);
		Enemy(Proctor* _proctor, Proctor* _playerOneRef, Proctor* _playerTwoRef, int _islandID, std::string _enemyType);
		Enemy(Proctor* _proctor, Proctor* _playerOneRef, Proctor* _playerTwoRef, int _islandID, float _maxHealth, float _damage, float _sightRadius, float _attackRadius, float _damageRadius, float _maxVelocity, float _acceleration, float _maxGravity, float _gravityAcceleration);
		~Enemy();

		/* Public methods */
		void update() override;
		void takeDamage(Proctor* playerRef, float damage);

		/* Getters */
		int getIslandID();
		
		/* Setters */
		void setCurrentGravity(float _currentGravity);

	private:
		/* Private fields */
		float maxHealth;
		float currentHealth;
		float sightRadius;
		float damage;
		float attackRadius;
		float damageRadius;
		float maxVelocity;
		float currentVelocity;
		float acceleration;
		float maxGravity;
		float currentGravity;
		float gravityAcceleration;
		int islandID;
		float cTimeInAir = 0;
		irrklang::ISoundEngine* enemyEngine = irrklang::createIrrKlangDevice();
		std::string enemyType;
		
		EnemyState currentState;
		
		/* References to our players */
		Proctor* playerOneRef;
		Proctor* playerTwoRef;
		Proctor* currentlyChasedPlayer;

		glm::vec3 wanderDirection;
		float newWanderDirectionTimer;
		float attackTimer;
		
		/* Private methods */
		void stoicBehaviour();
		void chasingBehaviour();
		void attackBehaviour();
		void dyingBehaviour();
		float distanceToPlayer(Proctor* player);
	};
}