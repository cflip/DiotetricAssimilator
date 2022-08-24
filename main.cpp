#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wincodec.h>

#include <array>
#include <cmath>
#include <chrono>

#include "texture.h"
#include "window.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "gdiplus.lib")

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	constexpr int Width = 1280;
	constexpr int Height = 720;

	Window window(Width, Height, L"Diotetric Assimilator");

	/*** GRAPHICS DEVICE INITIALIZATION ***/

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

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

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, nullptr, 0, D3D11_SDK_VERSION, &sc, &swapChain, &device, nullptr, &context);

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTarget;

	Microsoft::WRL::ComPtr<ID3D11Resource> backBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer);
	device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTarget);

	/*** GRAPHICS SETUP part 2 ***/

	struct Vertex
	{
		struct
		{
			float x, y, z;
		} pos;
		struct
		{
			float u, v;
		} tex;
	};

	const Vertex vertices[] =
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

	const unsigned short indices[] =
	{
		4, 3, 2,
		4, 2, 1,
		0, 5, 6,
		0, 7, 8,
		0, 9, 10,
		0, 11, 12,
	};

	/** Depth Stencil **/

	D3D11_DEPTH_STENCIL_DESC dsd;
	ZeroMemory(&dsd, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthState;
	device->CreateDepthStencilState(&dsd, &depthState);
	context->OMSetDepthStencilState(depthState.Get(), 1);

	D3D11_TEXTURE2D_DESC dtd;

	ZeroMemory(&dtd, sizeof(D3D11_TEXTURE2D_DESC));
	dtd.Width = Width;
	dtd.Height = Height;
	dtd.MipLevels = 1;
	dtd.ArraySize = 1;
	dtd.Format = DXGI_FORMAT_D32_FLOAT;
	dtd.SampleDesc.Count = 1;
	dtd.SampleDesc.Quality = 0;
	dtd.Usage = D3D11_USAGE_DEFAULT;
	dtd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
	device->CreateTexture2D(&dtd, nullptr, &depthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC stencilViewDesc;

	ZeroMemory(&stencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	stencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	stencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	stencilViewDesc.Texture2D.MipSlice = 0;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	device->CreateDepthStencilView(depthStencil.Get(), &stencilViewDesc, &depthStencilView);
	context->OMSetRenderTargets(1, renderTarget.GetAddressOf(), depthStencilView.Get());

	/** Vertex Buffer **/
	D3D11_BUFFER_DESC vbd;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.ByteWidth = sizeof(vertices);
	vbd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA vsd;
	vsd.pSysMem = vertices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	device->CreateBuffer(&vbd, &vsd, &vertexBuffer);

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	/** Index Buffer **/
	D3D11_BUFFER_DESC ibd;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA isd;
	isd.pSysMem = indices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	device->CreateBuffer(&ibd, &isd, &indexBuffer);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	/** Constant Buffer **/

	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};

	constexpr float AspectRatio = 9.0f / 16.0f;
	const ConstantBuffer cb =
	{
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixTranslation(0.0f, 0.0f, 4.0f) *
				DirectX::XMMatrixPerspectiveLH(1.0f, AspectRatio, 0.5f, 10.0f)
			)
		}
	};

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA csd;
	csd.pSysMem = &cb;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	device->CreateBuffer(&cbd, &csd, &constantBuffer);
	context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	/** Pixel Shader **/
	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	D3DReadFileToBlob(L"PixelShader.cso", &blob);
	device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader);

	/** Vertex Shader **/
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	D3DReadFileToBlob(L"VertexShader.cso", &blob);
	device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader);

	/** Input Layout **/
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	const D3D11_INPUT_ELEMENT_DESC inputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 3 * sizeof(float), D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	HRESULT hr = device->CreateInputLayout(inputElements, ARRAYSIZE(inputElements), blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);

	/** Texture **/
	GfxContext gfx = { device.Get(), context.Get() };
	Texture texture(gfx, L"brickwall_albedo.tiff");
	texture.Bind(gfx);

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	device->CreateSamplerState(&samplerDesc, &sampler);
	context->PSSetSamplers(0, 1, sampler.GetAddressOf());

	/** Context Setup **/
	context->IASetInputLayout(inputLayout.Get());
	context->OMSetRenderTargets(1, renderTarget.GetAddressOf(), nullptr);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/** Viewport **/
	D3D11_VIEWPORT vp;
	vp.Width = (float)Width;
	vp.Height = (float)Height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	context->RSSetViewports(1, &vp);

	/*** MAIN RENDER LOOP AND STUFF ***/

	auto lastTime = std::chrono::high_resolution_clock::now();
	while (!window.ShouldClose())
	{
		window.Update();

		auto now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float>(now - lastTime).count();
		const ConstantBuffer newConstantBufferValues =
		{
			{
				DirectX::XMMatrixTranspose(
					DirectX::XMMatrixRotationX(time) *
					DirectX::XMMatrixRotationZ(time) *
					DirectX::XMMatrixTranslation(0.0f, abs(sin(time * 4)) - 0.5f, 4.0f) *
					DirectX::XMMatrixPerspectiveLH(1.0f, AspectRatio, 0.5f, 10.0f)
				)
			}
		};

		D3D11_MAPPED_SUBRESOURCE msr;
		context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		CopyMemory(msr.pData, &newConstantBufferValues, sizeof(newConstantBufferValues));
		context->Unmap(constantBuffer.Get(), 0);

		context->VSSetShader(vertexShader.Get(), nullptr, 0);
		context->PSSetShader(pixelShader.Get(), nullptr, 0);

		const float colour[] = { 0.1f, 0.1f, 0.2f, 1.0f };
		context->ClearRenderTargetView(renderTarget.Get(), colour);
		context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		context->DrawIndexed(ARRAYSIZE(indices), 0, 0);

		swapChain->Present(1, 0);
	}
}
