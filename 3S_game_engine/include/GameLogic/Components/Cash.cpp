#include "Cash.h"
#include "GameLogic/Proctor.h"

namespace GameLogic
{
	Cash::Cash(ComponentType _type, Proctor* _proctor, Proctor* _playerOne, Proctor* _playerTwo)
		: Component(_type, _proctor), playerOne(_playerOne), playerTwo(_playerTwo)
	{
		if (_proctor != NULL)
		{
			proctor->addComponent(this);
		}
	}

	Cash::~Cash()
	{
		delete points;
	}

	void Cash::followPlayer()
	{
		// Distance between playerOne and a coin
		float xDistanceOne = playerOne->getPosition()[0] - proctor->getPosition()[0];
		float zDistanceOne = playerOne->getPosition()[2] - proctor->getPosition()[2];
		float distanceOne = sqrt(xDistanceOne * xDistanceOne + zDistanceOne * zDistanceOne);
		//std::cout << "PlayerOneDist: " << distanceOne << std::endl;
		// Distance between playerTwo and a coin
		float xDistanceTwo = playerTwo->getPosition()[0] - proctor->getPosition()[0];
		float zDistanceTwo = playerTwo->getPosition()[2] - proctor->getPosition()[2];
		float distanceTwo = sqrt(xDistanceTwo * xDistanceTwo + zDistanceTwo * zDistanceTwo);

		// Chose which player should be followed 
		if (distanceOne > distanceTwo)
		{
			xDistance = xDistanceTwo;
			zDistance = zDistanceTwo;
			distance = distanceTwo;
		}
		else
		{
			xDistance = xDistanceOne;
			zDistance = zDistanceOne;
			distance = distanceOne;
		}

		// Follow chosen player
		if (xDistance > minDistance)
		{
			proctor->setPosition(proctor->getPosition() += glm::vec3(speed, 0.0, 0.0));
		}

		if (xDistance < -minDistance)
		{
			proctor->setPosition(proctor->getPosition() - glm::vec3(speed, 0.0, 0.0));
		}

		if (zDistance > minDistance)
		{
			proctor->setPosition(proctor->getPosition() += glm::vec3(0.0, 0.0, speed));
		}

		if (zDistance < -minDistance)
		{
			proctor->setPosition(proctor->getPosition() -= glm::vec3(0.0, 0.0, speed));
		}

		if (distance <= minDistance + 0.1)
		{
			points->addPoints(100);
			proctor->getParentHierarchy()->removeObject(proctor);
		}
	}

	void Cash::setFollow(bool _isFollowing)
	{
		isFollowing = _isFollowing;
	}

	void Cash::update()
	{
		if(isFollowing)
			followPlayer();
	}
}