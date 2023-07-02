# fence

> [nn-d3d12-id3d12fence](https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12fence)

```c++
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
```

Fence的作用是用於同步CPU和GPU，確保命令列表的執行順序。

具體步驟:
1. 調用`ID3D12Device::CreateFence()`方法創建一個fence
2. 指定旗標為NONE，表示一個普通的fence
3. 把fence指針存到ComPtr中
4. 創建一個fence event，用於等待fence完成

這樣我們就有了一個fence物件和相應的事件。

使用fence，主要有以下步驟:

1. 在遞交命令列表前，將fence值(fenceValue)加一
2. 調用`ID3D12CommandQueue::Signal()`方法訊號fence
3. 調用`ID3D12Fence::SetEventOnCompletion()`方法，當fence完成時觸發事件
4. 調用`WaitForSingleObject()`方法等待事件，確保命令列表完成
   
這樣一來，我們就可以確保命令列表在GPU上完成執行，CPU才繼續執行。