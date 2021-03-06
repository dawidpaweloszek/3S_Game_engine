#include "Model.h"

namespace Loader
{
	Model::Model(std::string _path, std::string _name, bool _noTex, glm::vec3 _position, glm::quat _rotation, glm::vec3 _scale)
		: position(_position), rotation(_rotation), scale(_scale), path(_path), noTex(_noTex), name(_name)
	{
		loadModel(path);
	}

	Model::Model(std::string _path, std::string _name, bool _noTex, bool _noAnim, glm::vec3 _position, glm::quat _rotation, glm::vec3 _scale)
		: position(_position), rotation(_rotation), scale(_scale), path(_path), noTex(_noTex), noAnim(_noAnim), name(_name)
	{
		loadModel(path);
	}

	Model::Model(bool _noTex, glm::vec3 _position, glm::quat _rotation, glm::vec3 _scale)
		: position(_position), rotation(_rotation), scale(_scale), noTex(_noTex)
	{
		loadModel(path);
	}

	void Model::init() {}

	void Model::render(Shader& _shader)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, this->position);
		model = glm::rotate(model, this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, this->scale);
		_shader.setUniform("model", model);
		_shader.setUniformFloat("material.shininess", 0.5f);

		for (Mesh mesh : meshes)
			mesh.render(_shader);
	}

	void Model::renderChild(Shader _shader)
	{
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, this->parentPosition);
		model = glm::rotate(model, this->parentRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, this->parentRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, this->parentRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, this->position);
		model = glm::rotate(model, this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, this->scale);
		_shader.setUniform("model", model);
		_shader.setUniformFloat("material.shininess", 0.5f);

		for (Mesh mesh : meshes)
			mesh.render(_shader);
	}

	void Model::loadModel(std::string _path)
	{
		//cleanup();
		path = _path;



		/* Read file via Assimp */
		Assimp::Importer importer;
		const aiScene* scene;
		if (!noAnim)
			scene = importer.ReadFile(
				_path,
				aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType |
				aiProcess_Triangulate |
				aiProcess_GenSmoothNormals |
				aiProcess_FlipUVs |
				aiProcess_LimitBoneWeights
			);
		else
			scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_PreTransformVertices);

		this->scene = scene;

		/* Check if scene is imported properly */
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr << "Failed to load model at " << _path << std::endl << "Reason: " << importer.GetErrorString() << std::endl;
			return;
		}

		directory = _path.substr(0, _path.find_last_of("/"));
		processNode(scene->mRootNode, scene);
	}

	void Model::processNode(aiNode* _node, const aiScene* _scene)
	{
		/* Process all the meshes */
		for (unsigned int i = 0; i < _node->mNumMeshes; i++)
		{
			aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, _scene));
		}

		/* Process all child nodes */
		for (unsigned int i = 0; i < _node->mNumChildren; i++)
		{
			processNode(_node->mChildren[i], _scene);
		}
	}

	Mesh Model::processMesh(aiMesh* _mesh, const aiScene* _scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		/* Process Vertices */
		for (unsigned int i = 0; i < _mesh->mNumVertices; i++)
		{
			Vertex vertex;

			glm::vec3 vector;
			vector.x = _mesh->mVertices[i].x;
			vector.y = _mesh->mVertices[i].y;
			vector.z = _mesh->mVertices[i].z;
			setVertexBoneDataToDefault(vertex);


			/* Position */
			vertex.position = AssimpGLMHelpers::GetGLMVec(_mesh->mVertices[i]);
			/* Normal vectors */
			if (_mesh->HasNormals())
			{
				vector.x = _mesh->mNormals[i].x;
				vector.y = _mesh->mNormals[i].y;
				vector.z = _mesh->mNormals[i].z;
				vertex.normal = vector;
			}
			vertex.normal = AssimpGLMHelpers::GetGLMVec(_mesh->mNormals[i]);
			/* Textures */
			if (_mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				vec.x = _mesh->mTextureCoords[0][i].x;
				vec.y = _mesh->mTextureCoords[0][i].y;
				vertex.texCoord = vec;
			}
			else
				vertex.texCoord = glm::vec2(0.0f, 0.0f);

			/* Push completed data */
			vertices.push_back(vertex);
		}

		/* Process Indices */
		for (unsigned int i = 0; i < _mesh->mNumFaces; i++)
		{
			aiFace face = _mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		aiMaterial* material = _scene->mMaterials[_mesh->mMaterialIndex];

		/* Diffuse color */
		aiColor4D diff(1.0f);
		aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diff);
		/* Specular color*/
		aiColor4D spec(1.0f);
		aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &spec);

		extractBoneWeightForVertices(vertices, _mesh, _scene);

		/* Return properly collected data */
		return Mesh(vertices, indices, diff, spec);

		///* Process Material */
		//if (_mesh->mMaterialIndex >= 0)
		//{
		//	

		//	if (noTex)
		//	{

		//	}
		//	else
		//	{
		//		/* Diffuse maps */
		//		std::vector<Texture> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE);
		//		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		//		/* Specular maps */
		//		std::vector<Texture> specularMaps = loadTextures(material, aiTextureType_SPECULAR);
		//		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		//		
		//		/* Return properly collected data */
		//		return Mesh(vertices, indices, textures);
		//	}
		//}
	}

	void Model::setVertexBoneDataToDefault(Vertex& _vertex)
	{
		if (noAnim)
			return;

		for (int i = 0; i < MAX_BONE_WEIGHTS; i++)
		{
			_vertex.boneIDs[i] = -1;
			_vertex.weights[i] = 0.0f;
		}
	}

	void Model::setVertexBoneData(Vertex& _vertex, int _boneID, float _weight)
	{
		if (noAnim)
			return;

		for (int i = 0; i < MAX_BONE_WEIGHTS; i++)
		{
			if (_vertex.boneIDs[i] < 0)
			{
				_vertex.weights[i] = _weight;
				_vertex.boneIDs[i] = _boneID;
				break;
			}
		}
	}

	void Model::extractBoneWeightForVertices(std::vector<Vertex>& _vertices, aiMesh* _mesh, const aiScene* _scene)
	{
		if (noAnim)
			return;

		auto& _boneInfoMap = boneInfoMap;
		int& _boneCount = boneCount;

		for (int boneIndex = 0; boneIndex < _mesh->mNumBones; boneIndex++)
		{
			int boneID = -1;
			std::string boneName = _mesh->mBones[boneIndex]->mName.C_Str();

			if (_boneInfoMap.find(boneName) == _boneInfoMap.end())
			{
				BoneInfo newBoneInfo;
				newBoneInfo.id = _boneCount;
				newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(_mesh->mBones[boneIndex]->mOffsetMatrix);
				_boneInfoMap[boneName] = newBoneInfo;
				boneID = _boneCount;
				_boneCount++;
			}
			else
			{
				boneID = _boneInfoMap[boneName].id;
			}

			assert(boneID != -1);
			auto weights = _mesh->mBones[boneIndex]->mWeights;
			int numWeights = _mesh->mBones[boneIndex]->mNumWeights;

			for (int weightIndex = 0; weightIndex < numWeights; weightIndex++)
			{
				int vertexID = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				assert(vertexID <= _vertices.size());
				setVertexBoneData(_vertices[vertexID], boneID, weight);
			}
		}
	}

	std::vector<Texture> Model::loadTextures(aiMaterial* _material, aiTextureType _type)
	{
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < _material->GetTextureCount(_type); i++)
		{
			aiString str;
			_material->GetTexture(_type, i, &str);

			/* Prevent duplicate loading */
			bool skip = false;

			for (unsigned int j = 0; j < texturesLoaded.size(); j++)
			{
				if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(texturesLoaded[j]);
					skip = true;
					break;
				}
			}

			if (!skip)
			{
				/* Not loaded yet */
				Texture tex(this->directory, str.C_Str(), _type);
				tex.load(false); // we don't want to flip it, flipUV's is doing it for us
				textures.push_back(tex);
				texturesLoaded.push_back(tex);
			}
		}

		return textures;
	}

	void Model::cleanup()
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;

		for (Mesh mesh : meshes)
			mesh.cleanup();
	}

	std::vector<Mesh> Model::getMeshes() const
	{
		return meshes;
	}

	glm::mat4 Model::getModelMatrix() const
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, this->position);
		model = glm::rotate(model, this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, this->scale);
		return model;
	}
}