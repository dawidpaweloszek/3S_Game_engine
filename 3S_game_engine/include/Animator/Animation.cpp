#include "Animation.h"

Animation::Animation(const std::string& _animationPath, Loader::Model* _model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	auto animation = scene->mAnimations[0];
	duration = animation->mDuration;
	ticksPerSecond = animation->mTicksPerSecond;
	aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	globalTransformation = globalTransformation.Inverse();
	readHierarchyData(rootNode, scene->mRootNode);
	setupBones(animation, *_model);
}

Bone* Animation::findBone(const std::string& _name)
{
	auto iter = std::find_if(bones.begin(), bones.end(),
		[&](const Bone& Bone)
		{
			return Bone.getBoneName() == _name;
		}
	);

	if (iter == bones.end())	return nullptr;
	else return &(*iter);
}

void Animation::readHierarchyData(AssimpNodeData& _dest, const aiNode* _src)
{
	assert(_src);

	_dest.name = _src->mName.data;
	_dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(_src->mTransformation);
	_dest.childrenCount = _src->mNumChildren;

	for (int i = 0; i < _src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		readHierarchyData(newData, _src->mChildren[i]);
		_dest.children.push_back(newData);
	}
}

void Animation::setupBones(const aiAnimation* _animation, Loader::Model& _model)
{
	int size = _animation->mNumChannels;
	std::cout << size << std::endl;

	auto& boneInfoMap = _model.getBoneInfoMap();
	int& boneCount = _model.getBoneCount();

	for (int i = 0; i < size; i++)
	{
		auto channel = _animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}

		bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
	}

	this->boneInfoMap = boneInfoMap;
}
