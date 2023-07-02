# PSO

> [The D3D12 Pipeline State Object](https://logins.github.io/graphics/2020/04/12/DX12PipelineStateObject.html)

![](pic/螢幕擷取畫面%202023-07-01%20112950.png)

# Root signature

> [example-root-signatures](https://learn.microsoft.com/en-us/windows/win32/direct3d12/example-root-signatures)

![](pic/image.png)
![](https://cdn.discordapp.com/attachments/894788659356794880/1125040421609951262/image.png)
![](https://cdn.discordapp.com/attachments/894788659356794880/1125041062587674665/image.png)

# 2023/07/01
```c++
// create root signature 
		ComPtr<ID3D12RootSignature> rootSignature;
		{
			// define empty root signature 
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
			// serialize root signature 
			ComPtr<ID3DBlob> signatureBlob;
			ComPtr<ID3DBlob> errorBlob;
			if (const auto hr = D3D12SerializeRootSignature(
				&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
				&signatureBlob, &errorBlob); FAILED(hr)) {
				if (errorBlob) {
					auto errorBufferPtr = static_cast<const char*>(errorBlob->GetBufferPointer());
					chilog.error(utl::ToWide(errorBufferPtr)).no_trace();
				}
				hr >> chk;
			}
			// Create the root signature. 
			device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
				signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)) >> chk;
		}

		// creating pipeline state object 
		ComPtr<ID3D12PipelineState> pipelineState;
		{
			// static declaration of pso stream structure 
			struct PipelineStateStream
			{
				CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
				CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
				CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
				CD3DX12_PIPELINE_STATE_STREAM_VS VS;
				CD3DX12_PIPELINE_STATE_STREAM_PS PS;
				CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
			} pipelineStateStream;

			// define the Vertex input layout 
			const D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			// Load the vertex shader. 
			ComPtr<ID3DBlob> vertexShaderBlob;
			D3DReadFileToBlob(L"VertexShader.cso", &vertexShaderBlob) >> chk;

			// Load the pixel shader. 
			ComPtr<ID3DBlob> pixelShaderBlob;
			D3DReadFileToBlob(L"PixelShader.cso", &pixelShaderBlob) >> chk;

			// filling pso structure 
			pipelineStateStream.RootSignature = rootSignature.Get();
			pipelineStateStream.InputLayout = { inputLayout, (UINT)std::size(inputLayout) };
			pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
			pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
			pipelineStateStream.RTVFormats = {
				.RTFormats{ DXGI_FORMAT_R8G8B8A8_UNORM },
				.NumRenderTargets = 1,
			};

			// building the pipeline state object 
			const D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
				sizeof(PipelineStateStream), &pipelineStateStream
			};
			device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)) >> chk;
		}
```
1. 創建一個根符號
> 先定義一個空的根符號描述符,只允許輸入組裝器輸入佈局。然後將其序列化為一個blob,最後創建根符號。
2. 創建管線狀態物件

首先定義頂點輸入佈局和載入頂點/像素著色器。

然後定義一個CD3DX12_PIPELINE_STATE_STREAM_DESC結構體,填充相關狀態:
- 根符號
- 輸入佈局
- 參數網格
- 頂點/像素著色器
- 渲染目標格式

# 2023/07/02

```cpp
ComPtr<ID3D12RootSignature> rootSignature;
		{
			// 2023/07/02: define root signature with a matrix of 16 32-bit floats used by the vertex shader (rotation matrix)
			// 2023/07/02 ss: define root signature with a matrix of 16 32-bit floats used by the vertex shader (mvp matrix) 
			CD3DX12_ROOT_PARAMETER rootParameters[1]{};
			rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
			// Allow input layout and vertex shader and deny unnecessary access to certain pipeline stages.
			const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

			// define empty root signature
			// 2023/07/02: define root signature with transformation matrix
			// rootSignatureFlags added
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init((UINT)std::size(rootParameters), rootParameters,
				0, nullptr, rootSignatureFlags); 
```

根符號簽章用於指定頂點著色器可以訪問的資料。 在這裡,它定義了頂點著色器可以訪問一個16個32位浮點數的矩陣。 這很可能是模型變換矩陣(model、view、projection matrix)。

具體步驟是:
1. 定義根參數(root parameters),這裡只定義了一個作為常量的矩陣
```cpp
rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
```
2. 設定根符號標誌(root signature flags),允許頂點著色器訪問,禁止其他階段訪問
3. 初始化根符號描述(CD3DX12_ROOT_SIGNATURE_DESC),指定根參數和根符號標誌
4. 創建根符號(ID3D12Device::CreateRootSignature)

這樣一來，頂點著色器便可以通過根參數訪問矩陣變量，用於顯示幾何圖形。

# Pipeline State Object
![Alt text](pic/螢幕擷取畫面%202023-07-01%20112950.png)
PSO(Pipeline State Object)是Direct3D 12中重要的概念。它封装了渲染管线中的多个状态，包括:
- 顶点着色器和像素着色器
- 栅格化状态(多样本抗锯齿、缓存模式等)
- 深度和采样状态
- 输入布局和输出布局
- 根签名 
  - (描述每個可編程階段可用的資源集（例如紋理）)

简而言之，PSO代表了一个完整的渲染管线状态。开发者可以创建多个PSO，对应不同的渲染效果。这允许高效地切换渲染状态。
通过D3D12 API，开发者可以:
- 创建PSO(ID3D12Device::CreateGraphicsPipelineState())
- 设置PSO(ID3D12GraphicsCommandList::SetPipelineState())
- 删除PSO(ID3D12Device::DestroyPipelineState())

# 參考代碼

```C++
// 顶点着色器和像素着色器
auto vsBlob = ...; 
auto psBlob = ...;

// 输入布局
D3D12_INPUT_ELEMENT_DESC inputLayout[] = { ... };

// 创建管线状态对象描述
D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
psoDesc.VS = { ... };   // 顶点着色器
psoDesc.PS = { ... };   // 像素着色器
psoDesc.InputLayout = { ... }; // 输入布局
psoDesc.RasterizerState = ...; // 栅格状态
...

// 创建管线状态对象  
ID3D12PipelineState* pso;
dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));

// 在命令列表中使用管线状态对象
cmdList->SetPipelineState(pso);

...

// 最后释放管线状态对象
pso->Release();
```

```c++
struct PipelineStateStream
{
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
    CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
    CD3DX12_PIPELINE_STATE_STREAM_VS VS;
    CD3DX12_PIPELINE_STATE_STREAM_PS PS;
    CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
} pipelineStateStream;
```