# Command Queue (命令隊列)
> [d3d12-id3d12commandqueue](https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12commandqueue)

# 
```c++
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
```

命令隊列的作用是:
- 接收命令列表，並將它們遞交到GPU執行
- 控制命令列表的同步和排程

具體做了以下事情:
1. 定義了一個D3D12_COMMAND_QUEUE_DESC結構體，指定命令隊列的屬性:
   - Type: DIRECT，表示直接命令列表類型
   - Priority: NORMAL，優先級為正常
   - Flags: NONE，沒有旗標 
   - NodeMask: 0，用於多節點部署
2. 調用 ID3D12Device::CreateCommandQueue() 創建命令隊列
3. 把命令隊列指針存到ComPtr中

創建命令列表:
```cpp
ComPtr<ID3D12GraphicsCommandList> commandList; 
device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), NULL, IID_PPV_ARGS(&commandList))->Release();
```
遞交命令列表:
```cpp
commandQueue->ExecuteCommandLists(1, &commandList.Get());
```
同步命令列表:
```cpp
// 使用 fence 同步
ComPtr<ID3D12Fence> fence;
...

commandQueue->Signal(fence.Get(), ++fenceValue); 

fence->SetEventOnCompletion(fenceValue, fenceEvent);  
WaitForSingleObject(fenceEvent, INFINITE);

// 使用事件 同步
event->Set();
WaitForSingleObject(event, INFINITE);
```
其中:
- 使用 CreateCommandList() 創建命令列表
- 使用 ExecuteCommandLists() 將命令列表遞交到命令隊列,執行在 GPU 上
- 使用 Signal() 和 Wait() 方法來同步命令列表的執行,確保命令完成後才繼續執行
- 也可以使用事件(event object)來同步