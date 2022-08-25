#include "texture.h"

#include <assert.h>
#include <wincodec.h>
#include <wrl/client.h>

#include "debug.h"

static IWICImagingFactory* GetFactory()
{
	static IWICImagingFactory* factory = nullptr;

	if (!factory)
		GFX_ASSERT(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), reinterpret_cast<LPVOID*>(&factory)));

	return factory;
}

static UINT BitsPerPixelForFormat(WICPixelFormatGUID format)
{
	Microsoft::WRL::ComPtr<IWICComponentInfo> cinfo;
	GFX_ASSERT(GetFactory()->CreateComponentInfo(format, &cinfo));

	WICComponentType type;
	GFX_ASSERT(cinfo->GetComponentType(&type));
	assert(type == WICPixelFormat);

	Microsoft::WRL::ComPtr<IWICPixelFormatInfo> pfInfo;
	GFX_ASSERT(cinfo->QueryInterface(__uuidof(IWICPixelFormatInfo), &pfInfo));

	UINT bpp;
	GFX_ASSERT(pfInfo->GetBitsPerPixel(&bpp));
	return bpp;
}

Texture::Texture(const GfxContext& gfx, LPCWSTR filename)
{
	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
	GFX_ASSERT(GetFactory()->CreateDecoderFromFilename(L"brickwall_albedo.tiff", nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder));
	GFX_ASSERT(decoder->GetFrame(0, &frame));

	WICPixelFormatGUID wicFormat;
	GFX_ASSERT(frame->GetPixelFormat(&wicFormat));
	// TODO: Support more pixel formats, and convert from WIC format to DXGI format
	assert(wicFormat == GUID_WICPixelFormat32bppBGRA);

	UINT width, height;
	UINT bpp = BitsPerPixelForFormat(wicFormat);
	GFX_ASSERT(frame->GetSize(&width, &height));

	UINT imageStride = (width * bpp + 7) / 8;
	UINT imageSize = imageStride * height;

	BYTE* buffer = new BYTE[imageSize];
	GFX_ASSERT(frame->CopyPixels(nullptr, imageStride, imageSize, buffer));

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = buffer;
	data.SysMemPitch = imageStride;
	data.SysMemSlicePitch = imageSize;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	GFX_ASSERT(gfx.GetDevice()->CreateTexture2D(&desc, &data, &texture));

	D3D11_SHADER_RESOURCE_VIEW_DESC tvd;
	ZeroMemory(&tvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	tvd.Format = desc.Format;
	tvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	tvd.Texture2D.MostDetailedMip = 0;
	tvd.Texture2D.MipLevels = 1;

	GFX_ASSERT(gfx.GetDevice()->CreateShaderResourceView(texture.Get(), &tvd, &m_textureView));

	delete[] buffer;
}

void Texture::Bind(const GfxContext& gfx)
{
	gfx.GetContext()->PSSetShaderResources(0, 1, m_textureView.GetAddressOf());
}
