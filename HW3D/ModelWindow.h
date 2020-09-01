#pragma once
#include "Graphics.h"
#include "DynamicConstant.h"
#include <unordered_map>
#include <optional>

class Node;

class ModelWindow
{
public:
	void Show( Graphics& gfx, const char* windowName, const Node& root ) noexcept;
	void ApplyParameters() noexcept(!IS_DEBUG);
private:
	DirectX::XMMATRIX GetTransform() const noexcept(!IS_DEBUG);
	const Dcb::Buffer& GetMaterial() const noexcept(!IS_DEBUG);
	bool Transform() const noexcept(!IS_DEBUG);
	void ResetTransform() noexcept(!IS_DEBUG);
	bool Material() const noexcept(!IS_DEBUG);
	void ResetMaterial() noexcept(!IS_DEBUG);
	bool TransformMaterial() const noexcept(!IS_DEBUG);
private:
	Node* pSelectedNode;
	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	struct NodeData
	{
		TransformParameters transformParams;
		bool transformParamsBool;
		std::optional<Dcb::Buffer> materialCBuf;
		bool materialCBufBool;
	};
	std::unordered_map<int, NodeData> transforms;
};