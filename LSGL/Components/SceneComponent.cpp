#include "SceneComponent.h"

namespace lsgl
{
	SceneComponent::SceneComponent()
		:isTransformDirty(false)
	{
		localToWorldTransformCache = Matrix4x4::identity();
		worldToLocalTransformCache = Matrix4x4::identity();
	}

	SceneComponent::~SceneComponent()
	{
		detach();
	}

	Vector3 SceneComponent::getLocalPosition()
	{
		return localTransform.position;
	}

	Vector3 SceneComponent::getWorldPosition()
	{
		if (parent == nullptr)
		{
			return localTransform.position;
		}

		return parent->getLocalToWorldTransformMatrix() * Vector4(localTransform.position, 1);
	}

	Matrix4x4 SceneComponent::getLocalToWorldTransformMatrix()
	{
		if (isTransformDirty)
		{
			if (parent)
			{
				localToWorldTransformCache = parent->getLocalToWorldTransformMatrix();
			}
			else
			{
				localToWorldTransformCache = Matrix4x4::identity();
			}

			localToWorldTransformCache = localToWorldTransformCache * Matrix4x4::translate(localTransform.position) * Matrix4x4::rotateEuler(localTransform.rotation) * Matrix4x4::scale(localTransform.scale);
		}
		return localToWorldTransformCache;
	}

	Matrix4x4 SceneComponent::getWorldToLocalTransformMatrix()
	{
		if (isTransformDirty)
		{
			if (parent)
			{
				worldToLocalTransformCache = parent->getWorldToLocalTransformMatrix();
			}
			else
			{
				worldToLocalTransformCache = Matrix4x4::identity();
			}

			worldToLocalTransformCache = worldToLocalTransformCache * Matrix4x4::scale(localTransform.scale.inverse()) * Matrix4x4::rotateEuler(localTransform.rotation).transpose() * Matrix4x4::translate(localTransform.position * -1);
		}
		return worldToLocalTransformCache;
	}

	Vector3 SceneComponent::getXDirection()
	{
		Vector3 direction0 = getLocalToWorldTransformMatrix() * Vector3(0, 0, 0);
		Vector3 direction1 = getLocalToWorldTransformMatrix() * Vector3(1, 0, 0);
		return (direction1 - direction0).normalize();
	}

	Vector3 SceneComponent::getYDirection()
	{
		Vector3 direction0 = getLocalToWorldTransformMatrix() * Vector3(0, 0, 0);
		Vector3 direction1 = getLocalToWorldTransformMatrix() * Vector3(0, 1, 0);
		return (direction1 - direction0).normalize();
	}

	Vector3 SceneComponent::getZDirection()
	{
		Vector3 direction0 = getLocalToWorldTransformMatrix() * Vector3(0, 0, 0);
		Vector3 direction1 = getLocalToWorldTransformMatrix() * Vector3(0, 0, 1);
		return (direction1 - direction0).normalize();
	}

	void SceneComponent::setLocalPosition(const Vector3 & position)
	{
		localTransform.position = position;
		setTransformDirty();
	}

	void SceneComponent::setWorldPosition(const Vector3 & position)
	{
	}

	void SceneComponent::setLocalRotation(const Vector3 & rotation)
	{
		localTransform.rotation = rotation;
		setTransformDirty();
	}

	void SceneComponent::setLocalScale(const Vector3 & scale)
	{
		localTransform.scale = scale;
		setTransformDirty();
	}

	void SceneComponent::translate(const Vector3 & translation)
	{
		setLocalPosition(localTransform.position + translation);
	}

	void SceneComponent::rotateX(float angle)
	{
		setLocalRotation(localTransform.rotation + Vector3(angle, 0, 0));
	}

	void SceneComponent::rotateY(float angle)
	{
		setLocalRotation(localTransform.rotation + Vector3(0, angle, 0));
	}

	void SceneComponent::attachTo(SceneComponent & parent)
	{
		this->parent = &parent;
		setTransformDirty();
	}

	void SceneComponent::detach()
	{
		if (this->parent)
		{
			this->parent->removeChild(*this);
		}
		this->parent = nullptr;
	}

	void SceneComponent::addChild(SceneComponent & child)
	{
		std::vector<SceneComponent *>::iterator target = std::find(children.begin(), children.end(), &child);
		if (target != children.end())
		{
			return;
		}
		children.push_back(&child);
	}

	void SceneComponent::removeChild(const SceneComponent & child)
	{
		std::vector<SceneComponent *>::iterator target = std::find(children.begin(), children.end(), &child);
		if (target != children.end())
		{
			children.erase(target);
		}
	}

	std::vector<SceneComponent*> SceneComponent::getChildren() const
	{
		return children;
	}

	void SceneComponent::setTransformDirty()
	{
		isTransformDirty = true;
		for (auto child : children)
		{
			child->setTransformDirty();
		}
	}
}