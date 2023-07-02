# Command allocator
> [recording-command-lists-and-bundles](https://learn.microsoft.com/en-us/windows/win32/direct3d12/recording-command-lists-and-bundles)

# 
```c++
// Command allocator
		// https://learn.microsoft.com/en-us/windows/win32/direct3d12/recording-command-lists-and-bundles
		// 命令分配器允許應用程序管理為命令列表分配的內存。命令分配器是通過調用CreateCommandAllocator創建的。創建命令列表時，分配器的命令列表類型（由D3D12_COMMAND_LIST_TYPE指定）必須與正在創建的命令列表的類型相匹配。給定的分配器一次只能與一個當前記錄的命令列表相關聯，儘管一個命令分配器可用於創建任意數量的GraphicsCommandList對象。
		// 要回收由命令分配器分配的內存，應用程序調用ID3D12CommandAllocator::Reset。這允許分配器重新用於新命令，但不會減少其底層大小。但在這樣做之前，應用程序必須確保 GPU 不再執行與分配器關聯的任何命令列表；否則，調用將失敗。另請注意，此 API 不是自由線程的，因此不能從多個線程同時在同一個分配器上調用。
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		devices->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator)) >> chk;
```

![](https://cdn.discordapp.com/attachments/894788659356794880/1125055237003169844/gpu-workitems.png)

命令配置器的作用是:
- 管理命令列表所分配的記憶體
- 允許重複使用，但不會減少其大小
  
具體步驟:
1. 調用 ID3D12Device::CreateCommandAllocator() 創建命令配置器
2. 指定命令列表類型為DIRECT
3. 把命令配置器指針存到ComPtr中


使用命令配置器，我們主要有兩個操作:
- 創建命令列表時，需要指定配置器
- 調用 Reset() 方法重置配置器，允許其重新使用

重置配置器之前，必須確保GPU無法訪問相關命令列表，否則將失敗。

所以，通過命令配置器，我們可以管理命令列表所分配的內存，並重複使用配置器，但不影響其大小。