#include "Material.h"

Material Material::emerald = {
	glm::vec3(0.0215f, 0.1745f, 0.0215f),
	glm::vec3(0.07568f, 0.61424f, 0.07568f),
	glm::vec3(0.633f, 0.727811f, 0.633f),
	0.6f
};

Material Material::gold = {

};

Material Material::mix(Material _m1, Material _m2, float _mix)
{
	return {
		/* Set lighting values based on proportion */
		_m1.ambient * _mix + _m2.ambient * (1 - _mix),
		_m1.diffuse * _mix + _m2.diffuse * (1 - _mix),
		_m1.specular * _mix + _m2.specular * (1 - _mix),
		_m1.shininess * _mix + _m2.shininess * (1 - _mix)
	};
}
