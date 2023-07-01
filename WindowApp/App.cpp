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

//IID_PPV_ARGS 是一個宏, 它用于簡化從接口獲取指針的語法。
//在 COM 编程中, 我們經常需要向接口的方法傳遞接口 ID(IID)和接口指針變量的地址(&args), 以獲取該接口的指針。舉個例子:
//cpp
//IUnknown* ptr;
//// ...
//HRESULT hr = obj->QueryInterface(IID_IUnknown, (void**)&ptr);
//這裡我們傳遞 IID_IUnknown 和& ptr 以獲取 IUnknown 接口的指針。
//IID_PPV_ARGS 宏可以簡化上面的語法。我們可以寫成 :
//cpp
//IUnknown* ptr;
//// ...
//HRESULT hr = obj->QueryInterface(IID_PPV_ARGS(&ptr));

//這段代碼導入了一些 Direct3D 12 和 Windows 執行時 C++ 模板庫(WRL)的頭文件。
//它定義了 chil::app 命名空間和Run函數。Run函數接收一個 win::IWindow & 參考作為引數。
//在該函數中, 它聲明了一個指向 ID3D12Device2 的 ComPtr 智慧指針。然後它調用 D3D12CreateDevice 函數來創建 Direct3D 12 設備, 傳遞 nullptr 作為配接器和 D3D_FEATURE_LEVEL_12_2 作為最低功能級別。
//D3D12CreateDevice的結果被流入 chk 標記中使用 >> 運算符。這將對 D3D12CreateDevice 返回的 HRESULT 進行錯誤檢查。如果發生錯誤, 它將拋出一個 std::runtime_error, 其中包含有關錯誤的詳細信息。
//總的來說, 這段代碼:
//-聲明了一個指向 ID3D12Device2 Direct3D 12 設備的 ComPtr 智能指針
//- 调用 D3D12CreateDevice 函數來創建該設備
//- 使用 >> 流運算符對 D3D12CreateDevice 的結果進行錯誤檢查
//- 如果發生錯誤, 拋出一個 std::runtime_error 並帶有錯誤詳細信息


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
		
		// Command Queue (命令隊列)
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

		// swap chain 交换链：虛擬幀緩衝區
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
			// 打少了個1 GG
			swapChain1.As(&swapChain) >> chk;
		}

		// 媽的這三個老六 rtv
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
		// 命令分配器允許應用程序管理為命令列表分配的內存。命令分配器是通過調用CreateCommandAllocator創建的。創建命令列表時，分配器的命令列表類型（由D3D12_COMMAND_LIST_TYPE指定）必須與正在創建的命令列表的類型相匹配。給定的分配器一次只能與一個當前記錄的命令列表相關聯，儘管一個命令分配器可用於創建任意數量的GraphicsCommandList對象。
		// 要回收由命令分配器分配的內存，應用程序調用ID3D12CommandAllocator::Reset。這允許分配器重新用於新命令，但不會減少其底層大小。但在這樣做之前，應用程序必須確保 GPU 不再執行與分配器關聯的任何命令列表；否則，調用將失敗。另請注意，此 API 不是自由線程的，因此不能從多個線程同時在同一個分配器上調用。
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		devices->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator)) >> chk;

		// Command list
		// D3D12_COMMAND_LIST_TYPE_DIRECT 與 D3D12_COMMAND_LIST_TYPE_DIRECT 保持相同: 
		ComPtr<ID3D12GraphicsCommandList> commandList;
		devices->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator.Get(),nullptr,
			IID_PPV_ARGS(&commandList)) >> chk;
		
		// initially close the command list so it can be reset at top of draw loop
		commandList->Close() >> chk;

		// fence (主要為 DXD12 Command Resources 再重用)
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
			
			// ？因為 CopyResouce 和關閉 CommandList 時間不一樣，需要設置 fence 等待
			// inset fence to detect when upload is complete
			commandQueue->Signal(fence.Get(), ++fenceValue) >> chk;
			fence->SetEventOnCompletion(fenceValue, fenceEvent) >> chk;
			if (WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED)
			{
				GetLastError() >> chk;
			}
		}

		// CPU 視角命名 view
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