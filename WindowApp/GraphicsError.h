#pragma once
#include <source_location>

//首先是 CheckerToken 結構體, 它似乎被用作一種標記或機制。
//然後是 HrGrabber 結構體, 它包含兩個成員:
//-hr : 一個 unsigned int 類型的欄位
//- loc : 一個 std::source_location 類型的欄位, 用於記錄代碼的位置信息
//HrGrabber 還定義了一個構造函數, 在構造時記錄代碼位置。
//最後定義了一個右移操作符 >> , 它以 HrGrabber 和 CheckerToken 為操作數。
//整體來看, 這段代碼的作用似乎是 :
//	定義一種機制或標記 CheckerToken, 和一種帶代碼位置記錄功能的 HrGrabber 結構體。
//	當運行到 CheckerToken 標記時, 可以使用 HrGrabber 結構體記錄此時的代碼位置, 並執行某些操作(>> 操作符)。
//	這可以用於代碼的錯誤檢測、偵測和處理。在運行某段敏感或易出錯代碼前後放置 CheckerToken, 一旦出錯可以記錄位置並作出相應處理。

//This code defines a namespace called chil::app and defines two structures and an operator in it.
//First, the CheckerToken structure seems to be used as a mark or mechanism.
//Then the HrGrabber structure contains two members :
//-hr : An unsigned int field
//- loc : A std::source_location type field used to record the location information of the code
//HrGrabber also defines a constructor that records the code location when constructing.
//Finally, a right shift operator >> is defined with HrGrabber and CheckerToken as operands.
//Overall, the purpose of this code appears to be :
//Define a mechanism or mark CheckerToken, and a HrGrabber structure with code location recording function.
//When the CheckerToken mark is reached, the code location can be recorded using the HrGrabber structure, and some operations(>> operator) can be performed.
//This can be used for error detection, detection and handling of code.Place CheckerToken before and after a sensitive or error - prone code segment, and once an error occurs, the location can be recorded and corresponding handling can be performed.

namespace chil::app
{
	// CreateDeviceOrWhatever(...) >> chk;
	struct CheckerToken {} chk;
	struct HrGrabber
	{
		HrGrabber(unsigned int hr, std::source_location = std::source_location::current()) noexcept;
		unsigned int hr;
		std::source_location loc;
	};
	void operator>>(HrGrabber, CheckerToken);

}