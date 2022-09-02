#include <Windows.h>
#include <DirectXMath.h>

#include <chrono>

#include "gfx.h"
#include "buffer.h"
#include "constant_buffer.h"
#include "shader.h"
#include "texture.h"
#include "window.h"

#pragma comment(lib, "d3d11.lib")

class Object
{
public:
	Object(float offs) : m_offs(offs) {}

	DirectX::XMMATRIX GetTransformMatrix(float time) const
	{
		return DirectX::XMMatrixRotationX(time + m_offs) * 
			DirectX::XMMatrixRotationZ(time - m_offs) * 
			DirectX::XMMatrixTranslation(m_offs, abs(sin(time * 4 + m_offs)) - 0.5f, 6.0f);
	}

private:
	float m_offs;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	constexpr int Width = 1280;
	constexpr int Height = 720;
	constexpr float AspectRatio = static_cast<float>(Height) / static_cast<float>(Width);

	Window window(Width, Height, L"Diotetric Assimilator");
	GfxContext gfx(window);

	const std::vector<Vertex> vertices =
	{
		// Top point
		{ {  0.0f,  1.0f,  0.0f }, { 0.5f, 0 } },

		// Bottom face
		{ { -1.0f, -1.0f,  1.0f }, { 0, 1 } },
		{ {  1.0f, -1.0f,  1.0f }, { 1, 1 } },
		{ {  1.0f, -1.0f, -1.0f }, { 1, 0 } },
		{ { -1.0f, -1.0f, -1.0f }, { 0, 0 } },

		// North side bottom
		{ { -1.0f, -1.0f,  1.0f }, { 1, 1 } },
		{ {  1.0f, -1.0f,  1.0f }, { 0, 1 } },

		// East side bottom
		{ {  1.0f, -1.0f,  1.0f }, { 1, 1 } },
		{ {  1.0f, -1.0f, -1.0f }, { 0, 1 } },

		// South side bottom
		{ {  1.0f, -1.0f, -1.0f }, { 1, 1 } },
		{ { -1.0f, -1.0f, -1.0f }, { 0, 1 } },

		// West side bottom
		{ { -1.0f, -1.0f, -1.0f }, { 1, 1 } },
		{ { -1.0f, -1.0f,  1.0f }, { 0, 1 } },
	};

	const std::vector<unsigned short> indices =
	{
		4, 3, 2,
		4, 2, 1,
		0, 5, 6,
		0, 7, 8,
		0, 9, 10,
		0, 11, 12,
	};

	VertexBuffer vertexBuffer(gfx, vertices);
	vertexBuffer.Bind();

	IndexBuffer indexBuffer(gfx, indices);
	indexBuffer.Bind();

	std::vector<Object> objects;
	objects.emplace_back(-3.0f);
	objects.emplace_back(0.0f);
	objects.emplace_back(3.0f);

	DirectX::XMMATRIX perspMatrix = DirectX::XMMatrixPerspectiveLH(1.0f, AspectRatio, 0.5f, 10.0f);

	struct VertexConstBufferData
	{
		DirectX::XMMATRIX transform;
	};

	const VertexConstBufferData cb =
	{
		{
			DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0.0f, 0.0f, 4.0f) * perspMatrix)
		}
	};

	VertexConstantBuffer vertexConstBuffer(gfx, &cb, sizeof(cb));
	vertexConstBuffer.Bind();

	VertexShader vertexShader(gfx, L"VertexShader.cso");
	PixelShader pixelShader(gfx, L"PixelShader.cso");

	gfx.SetupInputLayout(vertexShader);

	Texture texture(gfx, L"brickwall_albedo.tiff");
	texture.Bind(gfx);

	auto lastTime = std::chrono::high_resolution_clock::now();
	while (!window.ShouldClose())
	{
		window.Update();

		auto now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float>(now - lastTime).count();

		vertexShader.Bind();
		pixelShader.Bind();

		const float colour[] = { 0.1f, 0.1f, 0.2f, 1.0f };
		gfx.ClearColor(colour);

		for (const auto& obj : objects)
		{
			const VertexConstBufferData newConstantBufferValues =
			{
				{
					DirectX::XMMatrixTranspose(obj.GetTransformMatrix(time) * perspMatrix)
				}
			};
			vertexConstBuffer.Update(&newConstantBufferValues, sizeof(newConstantBufferValues));
			gfx.Draw(indexBuffer);
		}

		gfx.Present();
	}
}
