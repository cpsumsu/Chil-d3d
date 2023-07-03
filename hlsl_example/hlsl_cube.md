# hlsl_cube

```c++
struct CubeColors
{
	float4 colors[6];
};
ConstantBuffer<CubeColors> cubeColors : register(b0);

float4 main(uint primitiveID : SV_PrimitiveID) : SV_TARGET
{
	return cubeColors.colors[primitiveID >> 1];
}
```

這段HLSL代碼是用來為立方體的6個面上色的。利用SV_PrimitiveID根據三角形的順序， 索引相應的陣列位置， 獲取正確的顏色。
1. struct CubeColors用來定義一個結構， 裡面有6個float4顏色。
2. ConstantBuffer<CubeColors> cubeColors : register(b0); 則是定義一個存著CubeColors結構內容的常數緩衝區。
3. float4 main(uint primitiveID : SV_PrimitiveID) : SV_TARGET 是主程式， 它利用葫蘆ID(primitiveID)來索引cubeColors.colors陣列， 獲取相應的顏色。
4. 因為立方體有6個面，每個面有2個三角形，primitiveID從0開始加總，0~1是第一個面，2~3是第二個面， 故primitiveID >> 1等於0 - 5， 就是cubeColors.colors[x]的索引。
