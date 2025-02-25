#include "App.h"
#include "GraphicsError.h"
#include <Core/src/log/Log.h> 
#include <Core/src/utl/String.h> 

#include <initguid.h> 
#include <d3d12.h> 
#include <dxgi1_6.h> 
#include <d3dcompiler.h> 
#include <DirectXMath.h> 
#pragma warning(push)
#pragma warning(disable : 26495)
#include "d3dx12.h" 
#pragma warning(pop)
#include <DirectXTex.h>
#include <wrl.h>

#include <cmath> 
#include <numbers> 
#include <ranges>

// IID_PPV_ARGS: __uuidof(**(&devices)), IID_PPV_ARGS_Helper(&devices)

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
	namespace vi = rn::views;

	int Run(win::IWindow& window)
	{
		// Constants 
		constexpr UINT width = 1280;
		constexpr UINT height = 720;
		constexpr UINT bufferCount = 2;

		// init COM
		CoInitializeEx(nullptr, COINIT_MULTITHREADED) >> chk;

		// Enable the software debug layer for d3d12
		{
			ComPtr<ID3D12Debug1> debugController;
			D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)) >> chk;
			debugController->EnableDebugLayer();
			debugController->SetEnableGPUBasedValidation(true);
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

		// depth buffer
		ComPtr<ID3D12Resource> depthBuffer;
		{
			const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
			const CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
				DXGI_FORMAT_D32_FLOAT,
				width, height,
				1, 0, 1, 0,
				D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
			const D3D12_CLEAR_VALUE clearValue = {
				.Format = DXGI_FORMAT_D32_FLOAT,
				.DepthStencil = { 1.0f, 0 },
			};
			devices->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&clearValue,
				IID_PPV_ARGS(&depthBuffer)) >> chk;
		}

		// dev descriptor heap
		ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
		{
			const D3D12_DESCRIPTOR_HEAP_DESC desc = {
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
				.NumDescriptors = 1
			};
			devices->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&dsvDescriptorHeap)) >> chk;
		}

		// dsv and handle 
		const CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{
			dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
		};
		devices->CreateDepthStencilView(depthBuffer.Get(), nullptr, dsvHandle);

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
			XMFLOAT2 tc;
		};

		// create Vertex buffer
		ComPtr<ID3D12Resource> vertexBuffer;
		UINT nVertices;
		{
			// the content data 
			const Vertex vertexData[] = {
				{ {-1.0f, -1.0f, -1.0f}, { 0.f, 0.f } }, // 0 
				{ {-1.0f,  1.0f, -1.0f}, { 0.f, 1.f } }, // 1 
				{ {1.0f,  1.0f, -1.0f}, { 1.f, 1.f } }, // 2 
				{ {1.0f, -1.0f, -1.0f}, { 1.f, 0.f } }, // 3 
				{ {-1.0f, -1.0f,  1.0f}, { 0.f, 1.f } }, // 4 
				{ {-1.0f,  1.0f,  1.0f}, { 0.f, 0.f } }, // 5 
				{ {1.0f,  1.0f,  1.0f}, { 1.f, 0.f } }, // 6 
				{ {1.0f, -1.0f,  1.0f}, { 1.f, 1.f } }  // 7
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
			// transition vertex buffer to vertex buffer state 
			{
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
					vertexBuffer.Get(),
					D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
				commandList->ResourceBarrier(1, &barrier);
			}
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

		// Create index buffer
		ComPtr<ID3D12Resource> indexBuffer;
		UINT nIndices;
		{
			// the content data
			const WORD indexData[] = {
				0, 1, 2, 0, 2, 3,
				4, 6, 5, 4, 7, 6,
				4, 5, 1, 4, 1, 0,
				3, 2, 6, 3, 6, 7,
				1, 5, 6, 1, 6, 2,
				4, 0, 3, 4, 3, 7
			};

			// set the vertex count
			nIndices = (UINT)std::size(indexData);
			{
				const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
				const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(indexData));
				devices->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resourceDesc,
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&indexBuffer)
				) >> chk;
			}
			// create commited resource for cpu upload of index data
			ComPtr<ID3D12Resource> indexUploadBuffer;
			{
				const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
				const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(indexData));
				devices->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resourceDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&indexUploadBuffer)
				) >> chk;
			}
			// copy array of index data to upload buffer 
			{
				WORD* mappedIndexData = nullptr;
				indexUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndexData)) >> chk;
				rn::copy(indexData, mappedIndexData);
				indexUploadBuffer->Unmap(0, nullptr); // Upload OK
			}
			// reset allocator and list
			commandAllocator->Reset() >> chk;
			commandList->Reset(commandAllocator.Get(), nullptr) >> chk;
			// copy upload buffer to index buffer
			commandList->CopyResource(indexBuffer.Get(), indexUploadBuffer.Get());
			// transition index buffer to index buffer state 
			{
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
					indexBuffer.Get(),
					D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
				commandList->ResourceBarrier(1, &barrier);
			}
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

		// Create the index buffer view
		const D3D12_INDEX_BUFFER_VIEW indexBufferView{
			.BufferLocation = indexBuffer->GetGPUVirtualAddress(),
			.SizeInBytes = nIndices * sizeof(WORD),
			.Format = DXGI_FORMAT_R16_UINT
		};

		// Create the cube texture 
		ComPtr<ID3D12Resource> cubeFaceTexture;
		{
			// load image data from disk 
			DirectX::ScratchImage image;
			DirectX::LoadFromWICFile(L"cube_face.jpeg", DirectX::WIC_FLAGS_NONE, nullptr, image) >> chk;

			// generate mip chain 
			DirectX::ScratchImage mipChain;
			DirectX::GenerateMipMaps(*image.GetImages(), DirectX::TEX_FILTER_BOX, 0, mipChain) >> chk;

			// create texture resource
			{
				const auto& chainBase = *mipChain.GetImages();
				const D3D12_RESOURCE_DESC texDesc{
					.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
					.Width = (UINT)chainBase.width,
					.Height = (UINT)chainBase.height,
					.DepthOrArraySize = 1,
					.MipLevels = (UINT16)mipChain.GetImageCount(),
					.Format = chainBase.format,
					.SampleDesc = {.Count = 1 },
					.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
					.Flags = D3D12_RESOURCE_FLAG_NONE,
				};
				const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
				devices->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&texDesc,
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&cubeFaceTexture)
				) >> chk;
			}

			// collect subresource data 
			const auto subresourceData = vi::iota(0, (int)mipChain.GetImageCount()) |
				vi::transform([&](int i) {
				const auto img = mipChain.GetImage(i, 0, 0);
				return D3D12_SUBRESOURCE_DATA{
					.pData = img->pixels,
					.RowPitch = (LONG_PTR)img->rowPitch,
					.SlicePitch = (LONG_PTR)img->slicePitch,
				};
					}) |
				rn::to<std::vector>();

			// create the intermediate upload buffer 
			ComPtr<ID3D12Resource> uploadBuffer;
			{
				const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
				const auto uploadBufferSize = GetRequiredIntermediateSize(
					cubeFaceTexture.Get(), 0, (UINT)subresourceData.size()
				);
				const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
				devices->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resourceDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&uploadBuffer)
				) >> chk;
			}

			// reset command list and allocator   
			commandAllocator->Reset() >> chk;
			commandList->Reset(commandAllocator.Get(), nullptr) >> chk;
			// write commands to copy data to upload texture (copying each subresource) 
			UpdateSubresources(
				commandList.Get(),
				cubeFaceTexture.Get(),
				uploadBuffer.Get(),
				0, 0,
				(UINT)subresourceData.size(),
				subresourceData.data()
			);
			// write command to transition texture to texture state  
			{
				const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
					cubeFaceTexture.Get(),
					D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				commandList->ResourceBarrier(1, &barrier);
			}
			// close command list   
			commandList->Close() >> chk;
			// submit command list to queue as array with single element  
			ID3D12CommandList* const commandLists[] = { commandList.Get() };
			commandQueue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);
			// insert fence to detect when upload is complete  
			commandQueue->Signal(fence.Get(), ++fenceValue) >> chk;
			fence->SetEventOnCompletion(fenceValue, fenceEvent) >> chk;
			if (WaitForSingleObject(fenceEvent, INFINITE) == WAIT_FAILED) {
				GetLastError() >> chk;
			}
		}

		// descriptor heap for the shader resource view 
		ComPtr<ID3D12DescriptorHeap> srvHeap;
		{
			const D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{
				.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				.NumDescriptors = 1,
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
			};
			devices->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)) >> chk;
		}

		// create handle to the srv heap and to the only view in the heap 
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(srvHeap->GetCPUDescriptorHandleForHeapStart());

		// create the descriptor in the heap 
		{
			const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{
				.Format = cubeFaceTexture->GetDesc().Format,
				.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Texture2D{.MipLevels = cubeFaceTexture->GetDesc().MipLevels },
			};
			devices->CreateShaderResourceView(cubeFaceTexture.Get(), &srvDesc, srvHandle);
		}

		// create root signature
		ComPtr<ID3D12RootSignature> rootSignature;
		{
			// 2023/07/02: define root signature with a matrix of 16 32-bit floats used by the vertex shader (rotation matrix)
			// 2023/07/02 ss: define root signature with a matrix of 16 32-bit floats used by the vertex shader (mvp matrix) 
			CD3DX12_ROOT_PARAMETER rootParameters[2]{};
			rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
			{
				CD3DX12_DESCRIPTOR_RANGE descRange{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0 };
				rootParameters[1].InitAsDescriptorTable(1, &descRange); 
			}
			// Allow input layout and vertex shader and deny unnecessary access to certain pipeline stages.
			const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
			// define static sampler 
			const CD3DX12_STATIC_SAMPLER_DESC staticSampler{ 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR };
			// define empty root signature
			// 2023/07/02: define root signature with transformation matrix
			// rootSignatureFlags added
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(
				(UINT)std::size(rootParameters), rootParameters,
				1, &staticSampler,
				rootSignatureFlags);
			// serizlize root signature
			ComPtr<ID3DBlob> signatureBlob;
			ComPtr<ID3DBlob> errorBlob;
			if (const auto hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
				&signatureBlob, &errorBlob); FAILED(hr))
			{
				if (errorBlob)
				{
					auto errorBufferPtr = static_cast<const char*>(errorBlob->GetBufferPointer());
					chilog.error(utl::ToWide(errorBufferPtr)).no_trace();
				}
				hr >> chk;
			}
			// Create Root Signature
			devices->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
				signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)) >> chk;
		}

		// create pipeline state object
		ComPtr<ID3D12PipelineState> pipelineState;
		{
			struct PipelineStateStream
			{
				CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
				CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
				CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
				CD3DX12_PIPELINE_STATE_STREAM_VS VS;
				CD3DX12_PIPELINE_STATE_STREAM_PS PS;
				CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
				CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
			} pipelineStateStream;

			// define the Vertex input layout
			const D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			// Load the vertex shader
			ComPtr<ID3D10Blob> vertexShaderBlob;
			D3DReadFileToBlob(L"VertexShader.cso", &vertexShaderBlob) >> chk;

			// Load the pixel shader
			ComPtr<ID3D10Blob> pixelShaderBlob;
			D3DReadFileToBlob(L"PixelShader.cso", &pixelShaderBlob) >> chk;

			// filling pso structure
			pipelineStateStream.RootSignature = rootSignature.Get();
			pipelineStateStream.InputLayout = { inputLayout, (UINT)std::size(inputLayout) };
			pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
			pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
			pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
			pipelineStateStream.RTVFormats = {
				.RTFormats{ DXGI_FORMAT_R8G8B8A8_UNORM },
				.NumRenderTargets = 1,
			};

			// building the position state object
			const D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
				sizeof(pipelineStateStream),&pipelineStateStream
			};
			devices->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)) >> chk;
		}

		// define scissor rect 
		const CD3DX12_RECT scissorRect{ 0, 0, LONG_MAX, LONG_MAX };

		// define viewport 
		const CD3DX12_VIEWPORT viewport{ 0.0f, 0.0f, float(width), float(height) };

		// set view projection martix
		XMMATRIX viewProjection;
		{
			// setup view (camera) matrix
			const auto eyePosition = XMVectorSet(0, 0, -6, 1);
			const auto focusPoint = XMVectorSet(0, 0, 0, 1);
			const auto upDirection = XMVectorSet(0, 1, 0, 0);
			const auto view = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
			// setup perspective projection matrix
			const auto aspectRatio = float(width) / float(height);
			const auto projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(65.f), aspectRatio, 0.1f, 200.0f);
			viewProjection = view * projection;
		}

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
			// get rtv handle for the buffer used in this frame
			const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv{
					rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
					(INT)curBackBufferIndex, rtvDescriptorSize };
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

				// clear rtv
				commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
			}
			// clear the depth buffer 
			commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			// set pipeline state
			commandList->SetPipelineState(pipelineState.Get());
			commandList->SetGraphicsRootSignature(rootSignature.Get());
			// configure IA
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			commandList->IASetIndexBuffer(&indexBufferView); // 2023/07/03 
			// configure RS
			commandList->RSSetScissorRects(1, &scissorRect);
			commandList->RSSetViewports(1, &viewport);
			// bind the heap containing the texture descriptor 
			commandList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
			// bind the descriptor table containing the texture descriptor 
			commandList->SetGraphicsRootDescriptorTable(1, srvHeap->GetGPUDescriptorHandleForHeapStart());
			// bind render target and dsv handle (depth)
			commandList->OMSetRenderTargets(1, &rtv, TRUE, &dsvHandle);
			// bind the transformation matrix
			// XMMatrixRotationZ
			// XMMatrixRotationY
			// XMMatrixRotationX
			// draw cube #1 
			{
				// bind the transformation matrix 
				const auto mvp = XMMatrixTranspose(
					XMMatrixRotationX(1.0f * t + 1.f) *
					XMMatrixRotationY(1.2f * t + 2.f) *
					XMMatrixRotationZ(1.1f * t + 0.f) *
					viewProjection
				);
				commandList->SetGraphicsRoot32BitConstants(0, sizeof(mvp) / 4, &mvp, 0);
				// draw the geometry  
				commandList->DrawIndexedInstanced(nIndices, 1, 0, 0, 0);
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
			t += step;
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