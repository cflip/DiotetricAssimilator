#include "gfx.h"

#include "buffer.h"
#include "shader.h"

GfxContext::GfxContext(const Window& window)
{
	DXGI_SWAP_CHAIN_DESC sc;

	ZeroMemory(&sc, sizeof(DXGI_SWAP_CHAIN_DESC));
	sc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sc.SampleDesc.Count = 1;
	sc.SampleDesc.Quality = 0;
	sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc.BufferCount = 1;
	sc.OutputWindow = window.GetWindowHandle();
	sc.Windowed = TRUE;
	sc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, nullptr, 0, D3D11_SDK_VERSION, &sc, &m_swapChain, &m_device, nullptr, &m_context);

	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(window.GetWidth());
	vp.Height = static_cast<float>(window.GetHeight());
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_context->RSSetViewports(1, &vp);

	m_swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &m_backBuffer);
	m_device->CreateRenderTargetView(m_backBuffer.Get(), nullptr, &m_renderTarget);
	m_context->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), nullptr);

	SetupDepthBuffer(window.GetWidth(), window.GetHeight());
	SetupTextureSampler();

	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void GfxContext::ClearColor(const float color[4])
{
	m_context->ClearRenderTargetView(m_renderTarget.Get(), color);
	m_context->ClearDepthStencilView(m_depthView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void GfxContext::Draw(const IndexBuffer& ib)
{
	m_context->DrawIndexed(ib.Count(), 0, 0);
}

void GfxContext::Present()
{
	m_swapChain->Present(1, 0);
}

void GfxContext::SetupInputLayout(const VertexShader& vertexShader)
{
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	const D3D11_INPUT_ELEMENT_DESC inputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 3 * sizeof(float), D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	ID3DBlob* blob = vertexShader.GetSourceBlob();
	m_device->CreateInputLayout(inputElements, ARRAYSIZE(inputElements), blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);
	m_context->IASetInputLayout(inputLayout.Get());
}

void GfxContext::SetupDepthBuffer(unsigned width, unsigned height)
{
	D3D11_DEPTH_STENCIL_DESC dsd;
	ZeroMemory(&dsd, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthState;
	m_device->CreateDepthStencilState(&dsd, &depthState);
	m_context->OMSetDepthStencilState(depthState.Get(), 1);

	D3D11_TEXTURE2D_DESC dtd;

	ZeroMemory(&dtd, sizeof(D3D11_TEXTURE2D_DESC));
	dtd.Width = width;
	dtd.Height = height;
	dtd.MipLevels = 1;
	dtd.ArraySize = 1;
	dtd.Format = DXGI_FORMAT_D32_FLOAT;
	dtd.SampleDesc.Count = 1;
	dtd.SampleDesc.Quality = 0;
	dtd.Usage = D3D11_USAGE_DEFAULT;
	dtd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
	m_device->CreateTexture2D(&dtd, nullptr, &depthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC stencilViewDesc;

	ZeroMemory(&stencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	stencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	stencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	stencilViewDesc.Texture2D.MipSlice = 0;

	m_device->CreateDepthStencilView(depthStencil.Get(), &stencilViewDesc, &m_depthView);
	m_context->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), m_depthView.Get());
}

void GfxContext::SetupTextureSampler()
{
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	m_device->CreateSamplerState(&samplerDesc, &sampler);
	m_context->PSSetSamplers(0, 1, sampler.GetAddressOf());
}