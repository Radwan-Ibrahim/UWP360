#pragma once
using namespace Windows::Foundation::Numerics;

namespace DirectX360Image
{
	// Position/rotation (degrees)/scale of an object in world space.
	public ref class Transform sealed
	{
	public:
		Transform();
		property float3 Translation;
		property float3 Rotation;
		property float3 Scale;

	internal:
		// Composes Scale * RotX * RotY * RotZ * Translation.
		DirectX::XMMATRIX GetWorldMatrix();
	};
}
