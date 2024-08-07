﻿/*
 *   Copyright 2022 Edmond Chow
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
#include <string>
#include <iostream>
#include <dynamics.h>
#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L) && (_MSC_VER >= 1800))
#define INLINE_VAR inline
#else
#define INLINE_VAR
#endif
 /* Compiler optimizing the assembly, the generated object code goes a bit difference. */
constexpr int __stdcall add(int x, int y)
{
	return x + y;
};
struct offset
{
public:
	std::uint64_t o{};
};
struct base
{
public:
	int x;
	constexpr base(int x)
		: x{ x }
	{};
	constexpr int __thiscall sub(int y)
	{
		return this->x -= y;
	};
};
struct box : public offset, public base
{
public:
	constexpr box(int x)
		: base{ x }
	{};
};
#ifndef _WIN64
INLINE_VAR constexpr std::uint8_t caller[]{
	0x6a, 0x04, 0x6a, 0x03, 0xb8, 0x00, 0x00, 0x00, 0x00, 0xff, 0xd0, 0xc3
};
INLINE_VAR constexpr std::size_t caller_sz = std::extent_v<decltype(caller)>;
/*
	0:  6a 04                   push   0x4
	2:  6a 03                   push   0x3
	4:  b8 00 00 00 00          mov    eax,0x0
	9:  ff d0                   call   eax
	b:  c3                      ret
*/
INLINE_VAR constexpr std::size_t caller_offset = 5;
INLINE_VAR constexpr std::uint8_t callee[]{
	0x8b, 0x44, 0x24, 0x04, 0x03, 0x44, 0x24, 0x08, 0xc2, 0x08, 0x00
};
INLINE_VAR constexpr std::size_t callee_sz = std::extent_v<decltype(callee)>;
/*
	0:  8b 44 24 04             mov    eax,DWORD PTR [esp+0x4]
	4:  03 44 24 08             add    eax,DWORD PTR [esp+0x8]
	8:  c2 08 00                ret    0x8
*/
INLINE_VAR constexpr std::uint8_t ths_callee[]{
	0x8b, 0x01, 0x2b, 0x44, 0x24, 0x04, 0x89, 0x01, 0xc2, 0x04, 0x00
};
/*
	0:  8b 01                   mov    eax,DWORD PTR [ecx]
	2:  2b 44 24 04             sub    eax,DWORD PTR [esp+0x4]
	6:  89 01                   mov    DWORD PTR [ecx],eax
	8:  c2 04 00                ret    0x4
*/
#else
INLINE_VAR constexpr std::uint8_t caller[]{
	0xba, 0x04, 0x00, 0x00, 0x00, 0xb9, 0x03, 0x00, 0x00, 0x00, 0x48, 0xb8, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xd0, 0xc3
};
INLINE_VAR constexpr std::size_t caller_sz = std::extent_v<decltype(caller)>;
/*
	0:  ba 04 00 00 00          mov    edx,0x4
	5:  b9 03 00 00 00          mov    ecx,0x3
	a:  48 b8 00 00 00 00 00    movabs rax,0x0
	11: 00 00 00
	14: ff d0                   call   rax
	16: c3                      ret
*/
INLINE_VAR constexpr std::size_t caller_offset = 12;
INLINE_VAR constexpr std::uint8_t callee[]{
	0x8d, 0x04, 0x11, 0xc3
};
INLINE_VAR constexpr std::size_t callee_sz = std::extent_v<decltype(callee)>;
/*
	0:  8d 04 11                lea    eax,[rcx+rdx*1]
	3:  c3                      ret
*/
INLINE_VAR constexpr std::uint8_t ths_callee[]{
	0x8b, 0x01, 0x29, 0xd0, 0x89, 0x01, 0xc3
};
/*
	0:  8b 01                   mov    eax,DWORD PTR [rcx]
	2:  29 d0                   sub    eax,edx
	4:  89 01                   mov    DWORD PTR [rcx],eax
	6:  c3                      ret
*/
#endif
int __cdecl main()
{
	dyn::function fn_reference{ add };
	int result = fn_reference.operator ()<int, dyn::call_opt_stdcall>(3, 4);
	if (result == add(3, 4))
	{
		std::wcout << L"#1:   The 'fn_reference' of type 'dyn::function' creates a reference pointed to by 'add' end up in Success." << std::endl;
	}
	else
	{
		std::wcout << L"#1:   The 'fn_reference' of type 'dyn::function' creates a reference pointed to by 'add' end up in Failure." << std::endl;
	}
	std::wcout << L"	-> Ask for whether the return value does match up what we expect." << std::endl << std::endl;
	dyn::function fn_callee{ callee };
	int ret_fn_callee = fn_callee.operator ()<int, dyn::call_opt_stdcall>(3, 4);
	if (ret_fn_callee == result)
	{
		std::wcout << L"#2:   The 'fn_callee' of type 'dyn::function' creates an instance end up in Success." << std::endl;
	}
	else
	{
		std::wcout << L"#2:   The 'fn_callee' of type 'dyn::function' creates an instance end up in Failure." << std::endl;
	}
	std::wcout << L"	-> The program creates new instances, the object code of that pure 'add' function given controls to yleid a value." << std::endl << std::endl;
	dyn::function fn_caller{ caller };
	fn_caller.operator []<intptr_t>(caller_offset) = reinterpret_cast<intptr_t>(fn_callee.raw());
	int ret_fn_caller = fn_caller.operator ()<int, dyn::call_opt_stdcall>();
	if (ret_fn_caller == result)
	{
		std::wcout << L"#3:   The 'fn_caller' of type 'dyn::function' creates an instance end up in Success." << std::endl;
	}
	else
	{
		std::wcout << L"#3:   The 'fn_caller' of type 'dyn::function' creates an instance end up in Failure." << std::endl;
	}
	std::wcout << L"	-> The program creates new instances, the object code of that caller stack frame processing dynamic relocation with operator as such." << std::endl << std::endl;
	void* ptr_callee = dyn::fn_malloc(callee_sz);
	memcpy(ptr_callee, callee, callee_sz);
	int ret_ptr_callee = dyn::fn_call<int, dyn::call_opt_stdcall>(ptr_callee, 3, 4);
	if (ret_ptr_callee == result)
	{
		std::wcout << L"#4:   The 'ptr_callee' with a storage duration invoked directly as a function end up in Success." << std::endl;
	}
	else
	{
		std::wcout << L"#4:   The 'ptr_callee' with a storage duration invoked directly as a function end up in Failure." << std::endl;
	}
	std::wcout << L"	-> Test for an invocation with a pointer whether it works." << std::endl << std::endl;
	void* ptr_caller = dyn::fn_malloc(caller_sz);
	memcpy(ptr_caller, caller, caller_sz);
	reinterpret_cast<intptr_t&>(reinterpret_cast<std::uint8_t*>(ptr_caller)[caller_offset]) = reinterpret_cast<intptr_t>(ptr_callee);
	int ret_ptr_caller = dyn::fn_call<int, dyn::call_opt_stdcall>(ptr_caller);
	if (ret_ptr_caller == result)
	{
		std::wcout << L"#5:   The 'ptr_caller' with a storage duration invoked directly as a function end up in Success." << std::endl;
	}
	else
	{
		std::wcout << L"#5:   The 'ptr_caller' with a storage duration invoked directly as a function end up in Failure." << std::endl;
	}
	std::wcout << L"	-> Test for an invocation with a pointer whether it works, while the object code of that caller stack frame processing dynamic relocation with operator as such." << std::endl << std::endl;
	dyn::fn_free(ptr_caller);
	dyn::fn_free(ptr_callee);
	box membx_result{ 8 };
	dyn::function mem_reference{ &base::sub };
	/*/
	 *   In this version of msvc, &box::sub and &base::sub both are just an address
	 *   points to which the program transfer controls without that offset portion for
	 *   which the caller adjust the this pointer.
	/*/
	int mem_result = mem_reference.operator ()<int, dyn::call_opt_thiscall, base*>(&membx_result, 5);
	/*/
	 *   In the circumstances, the 3rd template argument of 'operator ()' significantly
	 *   aids for adjusting the pointer as if the program cases it dynamically within the
	 *   chain of inheritance.
	/*/
	if (mem_result == membx_result.x && mem_result == box{ 8 }.sub(5))
	{
		std::wcout << L"#6:   The 'mem_reference' of type 'dyn::function' creates a reference pointed to by '&box::sub' end up in Success." << std::endl;
	}
	else
	{
		std::wcout << L"#6:   The 'mem_reference' of type 'dyn::function' creates a reference pointed to by '&box::sub' end up in Failure." << std::endl;
	}
	std::wcout << L"	-> Test for an invocation with a member function pointer on multiple inheritance whether it works." << std::endl << std::endl;
	box ret_membx_callee{ 8 };
	dyn::function mem_callee{ ths_callee };
	int ret_mem_callee = mem_callee.operator ()<int, dyn::call_opt_thiscall, base*>(&ret_membx_callee, 5);
	if (ret_mem_callee == ret_membx_callee.x && ret_mem_callee == mem_result)
	{
		std::wcout << L"#7:   The 'mem_callee' of type 'dyn::function' creates an instance end up in Success." << std::endl;
	}
	else
	{
		std::wcout << L"#7:   The 'mem_callee' of type 'dyn::function' creates an instance end up in Failure." << std::endl;
	}
	std::wcout << L"	-> Test for an invocation with a member function pointer on multiple inheritance with user inserted object codes whether it works." << std::endl << std::endl;
	std::wstring line;
	std::getline(std::wcin, line);
	return EXIT_SUCCESS;
};
