#pragma once 

#include "ImGui/ImGuiBaseLayer.h"
#include "ImGui/ImGuiContextInstance.h"
#include "ECWorld/SceneWorld.h"
#include "ECWorld/TransformComponent.h"

#include <imgui/imgui.h>

namespace ImGuiUtils
{

template<typename T>
bool ImGuiProperty(const char* pName, T& value, const T& minValue = {}, const T& maxValue = {})
{
	bool dirty = false;
	bool isUniform = true;

	if constexpr (std::is_same<T, std::string>())
	{
		ImGui::Columns(2);
		ImGui::TextUnformatted(pName);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		ImGui::TextUnformatted(value.c_str());

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		ImGui::Columns(1);
	}
	else if constexpr (std::is_same<T, bool>())
	{
		//CD_INFO("This is checkbox1");
		if (ImGui::Checkbox(pName, &value))
		{
			CD_INFO("This is checkbox2");
			CD_INFO(value);
			dirty = true;
		}
	}
	else if constexpr (std::is_same<T, float>())
	{
		ImGui::Columns(2);
		ImGui::TextUnformatted(pName);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string labelName = std::format("##{}", pName);
		float delta = maxValue - minValue;
		float speed = cd::Math::IsEqualToZero(delta) ? 1.0f : delta * 0.05f;
		if (ImGui::DragFloat(labelName.c_str(), &value, speed, minValue, maxValue))
		{
			dirty = true;
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		ImGui::Columns(1);
	}

	else if constexpr (std::is_same<T, cd::Vec2f>() || std::is_same<T, cd::Vec3f>() || std::is_same<T, cd::Vec4f>())
	{
		ImGui::Columns(2);
		ImGui::TextUnformatted(pName);
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		std::string labelName = std::format("##{}", pName);
		float delta = maxValue.x() - minValue.x();
		float speed = cd::Math::IsEqualToZero(delta) ? 1.0f : delta * 0.05f;
		if constexpr (std::is_same<T, cd::Vec2f>())
		{
			if (ImGui::DragFloat2(labelName.c_str(), value.Begin(), speed, minValue.x(), maxValue.x()))
			{
				dirty = true;
			}
		}
		else if constexpr (std::is_same<T, cd::Vec3f>())
		{
			if (ImGui::DragFloat3(labelName.c_str(), value.Begin(), speed, minValue.x(), maxValue.x()))
			{
				dirty = true;
			}
		}
		else if constexpr (std::is_same<T, cd::Vec4f>())
		{
			if (ImGui::DragFloat4(labelName.c_str(), value.Begin(), speed, minValue.x(), maxValue.x()))
			{
				dirty = true;
			}
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		ImGui::Columns(1);
	}
	else if constexpr (std::is_same<T, cd::Transform>())
	{
		if (ImGuiProperty<cd::Vec3f>("Translation", value.GetTranslation()))
		{
			dirty = true;
		}

		cd::Vec3f eularAngles = value.GetRotation().ToEulerAngles();
		if (ImGuiProperty<cd::Vec3f>("Rotation", eularAngles, cd::Vec3f::Zero(), cd::Vec3f(360.0f)))
		{
			float pitch = std::min(eularAngles.x(), 89.9f);
			pitch = std::max(pitch, -89.9f);

			value.SetRotation(cd::Quaternion::FromPitchYawRoll(pitch, eularAngles.y(), eularAngles.z()));
			dirty = true;
		}

		cd::Vec3f originScale = value.GetScale();
		cd::Vec3f scale = originScale;
		ImGui::TextUnformatted("Scale");
		ImGui::SameLine();
		bool UniformScaleEnabled = engine::TransformComponent::DoUseUniformScale();
		ImGui::Checkbox("Uniform", &UniformScaleEnabled);
		engine::TransformComponent::SetUseUniformScale(UniformScaleEnabled);

		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		if (ImGui::DragFloat3("##Scale", scale.Begin(), 0.1f, 0.001f, 999.0f))
		{
			if (!cd::Math::IsEqualTo(scale.x(), originScale.x()))
			{
				if (UniformScaleEnabled)
				{
					float ratio = scale.x() / originScale.x();
					cd::Vec3f _scale = value.GetScale();
					_scale *= ratio;
					value.SetScale(_scale);

					dirty = true;
				}
				else
				{
					value.SetScale(scale);
					dirty = true;
				}
			}

			if (!cd::Math::IsEqualTo(scale.y(), originScale.y()))
			{
				if (UniformScaleEnabled)
				{
					float ratio = scale.y() / originScale.y();
					cd::Vec3f _scale = value.GetScale();
					_scale *= ratio;
					value.SetScale(_scale);
					dirty = true;
				}
				else
				{
					value.SetScale(scale);
					dirty = true;
				}
			}

			if (!cd::Math::IsEqualTo(scale.z(), originScale.z()))
			{
				if (UniformScaleEnabled)
				{
					float ratio = scale.z() / originScale.z();
					cd::Vec3f _scale = value.GetScale();
					_scale *= ratio;
					value.SetScale(_scale);
					dirty = true;
				}
				else
				{
					value.SetScale(scale);
					dirty = true;
				}
			}
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		ImGui::Columns(1);
	}
	else
	{
		static_assert("Unsupported data type for imgui property.");
	}

	return dirty;
}

}