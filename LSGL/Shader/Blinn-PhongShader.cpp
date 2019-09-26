#include "Blinn-PhongShader.h"

namespace lsgl
{
	Vector4 BlinnPhongShader::vertexShading()
	{
		DescriptorPtr descriptor = renderer->getDescriptor("_Base");
		if (!descriptor)
		{
			return Vector4();
		}

		UniformBufferBase* baseUniformBuffer = reinterpret_cast<UniformBufferBase*>(descriptor->data);

		vertex->worldPosition = Vector3(baseUniformBuffer->modelMatrix * vertex->position);
		Matrix4x4 &mvpMatrix = baseUniformBuffer->mvpMatrix;
		return mvpMatrix * vertex->position;
	}

	Vector4 BlinnPhongShader::fragmentShading()
	{
		//light
		static Vector3 lightPos = Vector3(0, 300, 200);
		static float lightPower = 100000;
		Vector3 lightDir = (lightPos - fragment->worldPosition);
		LSFloat distance = lightDir.lengthPow();
		lightDir = lightDir.normalize();
		static Vector3 lightColor = Vector3(1, 1, 1) * lightPower / distance;

		//material
		Vector3 matDiffuse = texture("_DiffuseTex", fragment->uv);
		Vector3 matSpecular = texture("_SpecularTex", fragment->uv);
		static float shininess = 16;

		//view
		DescriptorPtr descriptor = renderer->getDescriptor("_App");
		if (!descriptor)
		{
			return Vector4();
		}

		UniformBufferApp* appUniformBuffer = reinterpret_cast<UniformBufferApp*>(descriptor->data);
		Vector3 &viewPos = appUniformBuffer->viewPos;

		Vector3 normal = fragment->normal.normalize();
		LSFloat diff = std::fmax(normal.dot(lightDir), 0);
		Vector3 diffuse = lightColor * diff * matDiffuse;

		Vector3 viewDir = (viewPos - fragment->worldPosition).normalize();
		Vector3 halfDir = (lightDir + viewDir).normalize();
		double specAngle = std::fmax(halfDir.dot(normal), 0.0);
		double spec = pow(specAngle, shininess);
		Vector3 specular = lightColor * spec * matSpecular;

		return Vector4(diffuse + specular, 1.0f);
	}
}
