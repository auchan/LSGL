#pragma once
#include <vector>
#include "../Core/Transform.h"
#include "../Math/Math.h"

namespace lsgl
{
	class SceneComponent
	{
	public:
		SceneComponent();
		~SceneComponent();
		Vector3 getLocalPosition();
		Vector3 getWorldPosition();
		Matrix4x4 getLocalToWorldTransformMatrix();
		Matrix4x4 getWorldToLocalTransformMatrix();

		Vector3 getXDirection();
		Vector3 getYDirection();
		Vector3 getZDirection();


		void setLocalPosition(const Vector3& position);
		void setWorldPosition(const Vector3& position);
		void setLocalRotation(const Vector3& rotation);
		void setLocalScale(const Vector3& scale);

		void translate(const Vector3& translation);
		void rotateX(float angle);
		void rotateY(float angle);

		void attachTo(SceneComponent& parent);
		void detach();

		void addChild(SceneComponent& child);
		void removeChild(const SceneComponent& child);

		std::vector<SceneComponent *> getChildren() const;

	protected:
		Transform localTransform;
		SceneComponent *parent;
		std::vector<SceneComponent *> children;

		void setTransformDirty();
		bool isTransformDirty;

	private:
		Matrix4x4 localToWorldTransformCache;
		Matrix4x4 worldToLocalTransformCache;
	};
}
