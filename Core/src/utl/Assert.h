#pragma once  
#include <string>  
#include <sstream>  
#include <optional>  
#include "StackTrace.h"  
#include <format>  
#include "Macro.h"  

namespace chil::utl
{
	class Assertion
	{
	public:
		Assertion(std::wstring expression, const wchar_t* file, const wchar_t* function, int line);
		~Assertion();
		Assertion& msg(const std::wstring& message);
		template<typename T>
		Assertion& watch(T&& val, const wchar_t* name)
		{
			stream_ << L"   " << name << L" => " << std::forward<T>(val) << L"\n";
			return *this;
		}
	private:
		const wchar_t* file_;
		const wchar_t* function_;
		int line_ = -1;
		std::wostringstream stream_;
	};
}

#ifndef ZC_CHILASS_ACTIVE  
#ifdef NDEBUG  
#define ZC_CHILASS_ACTIVE false  
#else  
#define ZC_CHILASS_ACTIVE true  
#endif  
#endif  

#define chilass(expr) (!ZC_CHILASS_ACTIVE || bool(expr)) ? void(0) : (void)utl::Assertion{ ZC_WSTR(expr), __FILEW__, __FUNCTIONW__, __LINE__ }  

#define ass_watch(expr) watch((expr), ZC_WSTR(expr))