#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "window.h"

struct Vertex
{
	struct { float x, y, z;	} pos;
	struct { float u, v; } tex;
};

class IndexBuffer;
class VertexShader;

class GfxContext
{
public:
	GfxContext(const Window&);

	ID3D11Device* GetDevice() const { return m_device.Get(); }
	ID3D11DeviceContext* GetContext() const { return m_context.Get(); }

	void ClearColor(const float color[4]);
	void Draw(const IndexBuffer&);
	void Present();

	// TODO: The user of this class shouldn't need to manually set up things, 
	//       the input layout could probably be its own class eventually.
	void SetupInputLayout(const VertexShader&);

private:
	void SetupDepthBuffer(unsigned width, unsigned height);
	void SetupTextureSampler();

	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthView;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTarget;
	Microsoft::WRL::ComPtr<ID3D11Resource> m_backBuffer;


};