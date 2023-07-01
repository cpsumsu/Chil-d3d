#include "App.h"
#include "GraphicsError.h"


#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h> // This header is used by DXGI. For more informati see 
#include "d3dx12.h"
// https://learn.microsoft.com/en-us/windows/win32/api/_direct3ddxgi/

#include <cmath>
#include <numbers>

// IID_PPV_ARGS: __uuidof(**(&devices)), IID_PPV_ARGS_Helper(&devices)

#include <DirectXMath.h> 
#include <ranges>

//IID_PPV_ARGS ��һ����, �����ں����Ľӿګ@ȡָᘵ��Z����
//�� COM �����, �҂�������Ҫ��ӿڵķ������f�ӿ� ID(IID)�ͽӿ�ָ�׃���ĵ�ַ(&args), �ԫ@ȡԓ�ӿڵ�ָᘡ��e������:
//cpp
//IUnknown* ptr;
//// ...
//HRESULT hr = obj->QueryInterface(IID_IUnknown, (void**)&ptr);
//�@�e�҂����f IID_IUnknown ��& ptr �ԫ@ȡ IUnknown �ӿڵ�ָᘡ�
//IID_PPV_ARGS ����Ժ���������Z�����҂����Ԍ��� :
//cpp
//IUnknown* ptr;
//// ...
//HRESULT hr = obj->QueryInterface(IID_PPV_ARGS(&ptr));

//�@�δ��a������һЩ Direct3D 12 �� Windows ���Еr C++ ģ���(WRL)���^�ļ���
//�����x�� chil::app �������g��Run������Run��������һ�� win::IWindow & ��������������
//��ԓ������, ������һ��ָ�� ID3D12Device2 �� ComPtr �ǻ�ָᘡ�Ȼ�����{�� D3D12CreateDevice �����턓�� Direct3D 12 �O��, ���f nullptr ����������� D3D_FEATURE_LEVEL_12_2 ������͹��ܼ��e��
//D3D12CreateDevice�ĽY�������� chk ��ӛ��ʹ�� >> �\������@���� D3D12CreateDevice ���ص� HRESULT �M���e�`�z�顣����l���e�`, ��������һ�� std::runtime_error, ���а������P�e�`��Ԕ����Ϣ��
//���ā��f, �@�δ��a:
//-����һ��ָ�� ID3D12Device2 Direct3D 12 �O��� ComPtr ����ָ�
//- ���� D3D12CreateDevice �����턓��ԓ�O��
//- ʹ�� >> ���\����� D3D12CreateDevice �ĽY���M���e�`�z��
//- ����l���e�`, ����һ�� std::runtime_error �K�����e�`Ԕ����Ϣ


/*This code imports some Direct3D 12 and Windows Runtime C++ Template Library(WRL) headers.
It defines a chil::app namespace and a Run function within it.The Run function takes a win::IWindow & reference as an argument.
In the function, it declares a ComPtr smart pointer to ID3D12Device2.It then calls the D3D12CreateDevice function to create a Direct3D 12 device, passing nullptr for the adapter and D3D_FEATURE_LEVEL_12_2 for the minimum feature level.
The result of D3D12CreateDevice is streamed into the chk token using the >> operator. This performs error checking on the HRESULT returned by D3D12CreateDevice.If an error occurs, it will throw a std::runtime_error with details about the error.
In summary, this code:
-Declares a smart pointer ComPtr to a ID3D12Device2 Direct3D 12 device
- Calls D3D12CreateDevice to create the device
- Uses the >> streaming operator to perform error checking on the result of D3D12CreateDevice
- If an error occurs, throws a std::runtime_error with error details
So the purpose of this code is to create a Direct3D 12 device, with error handling and error details provided on failur*/



namespace chil::app
{
	using Microsoft::WRL::ComPtr;
	using namespace DirectX;
	namespace rn = std::ranges;

	int Run(win::IWindow& window)
	{
		// Constants 
		constexpr UINT width = 1280;
		constexpr UINT height = 720;
		constexpr UINT bufferCount = 2;

		// Enable the software debug layer for d3d12
		{
			ComPtr<ID3D12Debug> debugController;
			D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)) >> chk;
			debugController->EnableDebugLayer();
		}

		// Factory dxgi D3D12
		ComPtr<IDXGIFactory4> dxgiFactory;
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgiFactory)) >> chk;
		
		// Devices D3D12
		ComPtr<ID3D12Device2> devices;
		D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&devices)) >> chk;
		
		// Command Queue (�������)
		// https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12commandqueue
		ComPtr<ID3D12CommandQueue> commandQueue;
		{
			const D3D12_COMMAND_QUEUE_DESC desc = {
				.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
				.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
				.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
				.NodeMask = 0
			};
			devices->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)) >> chk;
		};

		// swap chain ��������̓�M�����n�^
		ComPtr<IDXGISwapChain4> swapChain;
		{
			const DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
				.Width = width,
				.Height = height,
				.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
				.Stereo = FALSE,
				.SampleDesc = {
					.Count = 1,
					.Quality = 0
				},
				.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
				.BufferCount = bufferCount,
				.Scaling = DXGI_SCALING_STRETCH,
				.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
				.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
				.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING,
			};
			ComPtr<IDXGISwapChain1> swapChain1;
			dxgiFactory->CreateSwapChainForHwnd(
				commandQueue.Get(),
				window.GetHandle(),
				&swapChainDesc,
				nullptr,
				nullptr,
				&swapChain1
			) >> chk;
			// �����˂�1 GG
			swapChain1.As(&swapChain) >> chk;
		}

		// �����@�������� rtv
		// rtv descriptor heap
		ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
		{
			const D3D12_DESCRIPTOR_HEAP_DESC desc = {
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
				.NumDescriptors = bufferCount,
			};
			devices->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvDescriptorHeap)) >> chk;
		}

		// rtvDescriptorSize
		// The descriptor heap for the render-target view.
		const auto rtvDescriptorSize = devices->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV
		);

		// rtv descriptor and buffer references
		ComPtr<ID3D12Resource> backBuffers[bufferCount];
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
				rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			for (int i = 0; i < bufferCount; ++i)
			{
				swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i])) >> chk;
				devices->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvHandle);
				rtvHandle.Offset(rtvDescriptorSize);
			}
		}

		// Command allocator
		// https://learn.microsoft.com/en-us/windows/win32/direct3d12/recording-command-lists-and-bundles
		// ������������S���ó������������б����ăȴ档�����������ͨ�^�{��CreateCommandAllocator�����ġ����������б�r���������������б���ͣ���D3D12_COMMAND_LIST_TYPEָ��������c���ڄ����������б�������ƥ�䡣�o���ķ�����һ��ֻ���cһ����ǰӛ䛵������б����P������һ���������������춄������┵����GraphicsCommandList����
		// Ҫ�������������������ăȴ棬���ó����{��ID3D12CommandAllocator::Reset���@���S�������������������������p����׌Ӵ�С�������@����֮ǰ�����ó����횴_�� GPU ���و����c�������P���κ������б���t���{�Ì�ʧ������Ոע�⣬�� API �������ɾ��̵ģ���˲��܏Ķ�������ͬ�r��ͬһ�����������{�á�
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		devices->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator)) >> chk;

		// Command list
		// D3D12_COMMAND_LIST_TYPE_DIRECT �c D3D12_COMMAND_LIST_TYPE_DIRECT ������ͬ: 
		ComPtr<ID3D12GraphicsCommandList> commandList;
		devices->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator.Get(),nullptr,
			IID_PPV_ARGS(&commandList)) >> chk;
		
		// initially close the command list so it can be reset at top of draw loop
		commandList->Close() >> chk;

		// fence (��Ҫ�� DXD12 Command Resources ������)
		// https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12fence
		// Represents a fence, an object used for synchronization of the CPU and one or more GPUs.
		ComPtr<ID3D12Fence> fence;
		uint16_t fenceValue = 0;
		devices->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)) >> chk;

		// fence signalling event
		HANDLE fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		if (!fenceEvent) // Error
		{
			GetLastError() >> chk;
			throw std::runtime_error("Failed to create fence event");
		}

		// data structure for vertex data
		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT3 color;
		};

		// create Vertex buffer
		ComPtr<ID3D12Resource> vertexBuffer;
		UINT nVertices;
		{
			// the content data 
			const Vertex vertexData[] = {
				{ {  0.00f,  0.50f, 0.0f }, { 1.0f, 0.0f, 0.0f } }, // top 
				{ {  0.43f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, // right 
				{ { -0.43f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f } }, // left 
			};
			
			// set the vertex count
			nVertices = (UINT)std::size(vertexData);
			{
				const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
				const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexData));

				// Committed resource == Create a heap in the same time
				devices->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resourceDesc,
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr, IID_PPV_ARGS(&vertexBuffer)
				) >> chk;
			}
			// create commited resource cpu upload for vertex data
			ComPtr<ID3D12Resource> vertexUploadBuffer;
			{
				const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
				const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexData));

				// Committed resource == Create a heap in the same time
				devices->CreateCommittedResource(
					&heapProps, 
					D3D12_HEAP_FLAG_NONE,
					&resourceDesc, 
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr, IID_PPV_ARGS(&vertexUploadBuffer)
				) >> chk;
			}
			// Copy array of vertex data to upload buffer
			{
				Vertex* mappedVertexData = nullptr;
				vertexUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertexData)) >> chk;
				rn::copy(vertexData, mappedVertexData);
				vertexUploadBuffer->Unmap(0, nullptr); // Upload OK
			}

			// D3D12 ERROR: ID3D12Resource2::<final-release>: CORRUPTION: 
			// An ID3D12Resource object (0x000001F0BC3926A0:'Unnamed Object') is referenced by GPU operations in-flight on Command Queue 
			// (0x000001F0BC070930:'Unnamed ID3D12CommandQueue Object').  It is not safe to final-release objects that may have GPU operations pending.  
			// This can result in application instability. [ EXECUTION ERROR #921: OBJECT_DELETED_WHILE_STILL_IN_USE]
			
			// reset allocator and list
			commandAllocator->Reset() >> chk;
			commandList->Reset(commandAllocator.Get(), nullptr) >> chk;
			// copy upload buffer to vertex buffer
			commandList->CopyResource(vertexBuffer.Get(), vertexUploadBuffer.Get());
			// close commandList
			commandList->Close() >> chk;
			// submit command list to queue as arry with single element
			ID3D12CommandList* const commandLists[] = { commandList.Get() };
			commandQueue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);
			
			// ����� CopyResouce ���P�] CommandList �r�g��һ�ӣ���Ҫ�O�� fence �ȴ�
			// inset fence to detect when upload is complete
			commandQueue->Signal(fence.Get(), ++fenceValue) >> chk;
			fence->SetEventOnCompletion(fenceValue, fenceEvent) >> chk;
			if (WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED)
			{
				GetLastError() >> chk;
			}
		}

		// CPU ҕ������ view
		// Create the Vertex buffer view (CPU)
		const D3D12_VERTEX_BUFFER_VIEW vertexBufferView{
			.BufferLocation = vertexBuffer->GetGPUVirtualAddress(),
			.SizeInBytes = nVertices * sizeof(Vertex),
			.StrideInBytes = sizeof(Vertex)
		};

		// render loop
		UINT curBackBufferIndex;
		float t = 0.f;
		constexpr float step = 0.01f;
		while (!window.IsClosing())
		{
			// advance buffer
			curBackBufferIndex = swapChain->GetCurrentBackBufferIndex();

			auto& backBuffer = backBuffers[curBackBufferIndex];
			commandAllocator->Reset() >> chk;
			commandList->Reset(commandAllocator.Get(),nullptr) >> chk;
			// clear the render target
			{
				// transition buffer resource to render target state
				// CPU cashe data BARRIER::Transition
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
					backBuffer.Get(),
					D3D12_RESOURCE_STATE_PRESENT, 
					D3D12_RESOURCE_STATE_RENDER_TARGET
				);
				commandList->ResourceBarrier(1, &barrier);
				// clear buffer
				// Cycle color
				const FLOAT clearColor[] = {
					sin(2.f * t + 1.f) / 2.f + .5f,
					sin(3.f * t + 2.f) / 2.f + .5f,
					sin(5.f * t + 3.f) / 2.f + .5f,
					1.0f 
				};



				const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv{
					rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
					(INT)curBackBufferIndex, rtvDescriptorSize };
				commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
			}
			// prepare buffer for presentation
			{
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
					backBuffer.Get(),
					D3D12_RESOURCE_STATE_RENDER_TARGET,
					D3D12_RESOURCE_STATE_PRESENT
				);
				commandList->ResourceBarrier(1, &barrier);
			}
			// submit command list
			{
				// close command list
				commandList->Close() >> chk;
				// submit command list to queue
				ID3D12CommandList* const commandLists[] = { commandList.Get() };
				commandQueue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);
			}
			// insert fence to mark command list completion
			commandQueue->Signal(fence.Get(), ++fenceValue) >> chk;
			// present framce
			swapChain->Present(1, 0) >> chk;
			// wait for command list / allocator to become free
			fence->SetEventOnCompletion(fenceValue, fenceEvent) >> chk;
			if (::WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED) {
				GetLastError() >> chk;
			}
			if ((t += step) >= 2.f * std::numbers::pi_v<float>) {
				t = 0.f;
			}
		}

		// wait for queue to become completely empty (2 seconds max) 
		commandQueue->Signal(fence.Get(), ++fenceValue) >> chk;
		fence->SetEventOnCompletion(fenceValue, fenceEvent) >> chk;
		if (WaitForSingleObject(fenceEvent, 2000) == WAIT_FAILED)
		{
			GetLastError() >> chk;
		}

		return 0;
	}
}