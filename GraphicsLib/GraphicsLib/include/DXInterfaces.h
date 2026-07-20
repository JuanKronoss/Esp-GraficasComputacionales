#pragma once

#include "Helpers.h"

#define DX_VERSION_11_0 NOT_IN_USE
#define DX_VERSION_11_1 NOT_IN_USE
#define DX_VERSION_11_2 NOT_IN_USE
#define DX_VERSION_11_3 NOT_IN_USE
#define DX_VERSION_11_4 IN_USE

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#if USING(DX_VERSION_11_0)
#  include <d3d11.h>

using D3DDevice = ID3D11Device;
using D3DDeviceContext = ID3D11DeviceContext;
using SwapChain = IDXGISwapChain;
using DXGIFactory = IDXGIFactory1;
using DXGIAdapter = IDXGIAdapter;
using DXGIDevice = IDXGIDevice1;
using RenderTargetView = ID3D11RenderTargetView;
using RasterizerState = ID3D11RasterizerState;
using BlendState = ID3D11BlendState;

#elif USING(DX_VERSION_11_1)
#  include <d3d11_1.h>

using D3DDevice = ID3D11Device1;
using D3DDeviceContext = ID3D11DeviceContext1;
using SwapChain = IDXGISwapChain1;
using DXGIFactory = IDXGIFactory2;
using DXGIAdapter = IDXGIAdapter1;
using DXGIDevice = IDXGIDevice2;
using RenderTargetView = ID3D11RenderTargetView;
using RasterizerState = ID3D11RasterizerState1;
using BlendState = ID3D11BlendState1;

#elif USING(DX_VERSION_11_2)
#  include <d3d11_2.h>

using D3DDevice = ID3D11Device2;
using D3DDeviceContext = ID3D11DeviceContext2;
using SwapChain = IDXGISwapChain2;
using DXGIFactory = IDXGIFactory3;
using DXGIAdapter = IDXGIAdapter1;
using DXGIDevice = IDXGIDevice3;
using RenderTargetView = ID3D11RenderTargetView;
using RasterizerState = ID3D11RasterizerState1;
using BlendState = ID3D11BlendState1;

#elif USING(DX_VERSION_11_3)
#  include <d3d11_3.h>
#  include <dxgi1_4.h>

using D3DDevice = ID3D11Device3;
using D3DDeviceContext = ID3D11DeviceContext3;
using SwapChain = IDXGISwapChain3;
using DXGIFactory = IDXGIFactory4;
using DXGIAdapter = IDXGIAdapter1;
using DXGIDevice = IDXGIDevice3;
using RenderTargetView = ID3D11RenderTargetView1;
using RasterizerState = ID3D11RasterizerState2;
using BlendState = ID3D11BlendState1;

#elif USING(DX_VERSION_11_4)
#  include <d3d11_4.h>
#  include <dxgi1_5.h>

using D3DDevice = ID3D11Device4;
using D3DDeviceContext = ID3D11DeviceContext4;
using SwapChain = IDXGISwapChain4;
using DXGIFactory = IDXGIFactory5;
using DXGIAdapter = IDXGIAdapter1;
using DXGIDevice = IDXGIDevice3;
using RenderTargetView = ID3D11RenderTargetView1;
using RasterizerState = ID3D11RasterizerState2;
using BlendState = ID3D11BlendState1;

#else
#  error "No DirectX version defined. Please define a DirectX version to use."
#endif

using D3DResource = ID3D11Resource;
using D3DTexture1D = ID3D11Texture1D;
using D3DTexture2D = ID3D11Texture2D;
using D3DTexture3D = ID3D11Texture3D;

using ShaderResourceView = ID3D11ShaderResourceView;
using DepthStencilView = ID3D11DepthStencilView;
using UnorderedAccessView = ID3D11UnorderedAccessView;

inline void ThrowIfFailed(HRESULT hr, const char* msg = "DirectX call failed")
{
  if (FAILED(hr))
  {

    throw std::exception(msg);
  }
}

template<typename RetT, typename OgT>
[[nodiscard]]RetT* GetAs(OgT* pObj)
{
  RetT* pRet = nullptr;
  ThrowIfFailed(pObj->QueryInterface(__uuidof(RetT), reinterpret_cast<void**>(&pRet)));
  return pRet;
}
