/*
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
#include <excpt.h>
#include <cstdarg>
#include <string>
#include <iostream>
#include <dynamics.h>
#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L) && (_MSC_VER >= 1800))
#define INLINE_VAR inline
#else
#define INLINE_VAR
#endif
/* Compiler optimizing the assembly, the generated object code goes a bit difference. */
static constexpr int __stdcall add(int x, int y)
{
	return x + y;
};
static int __cdecl sum(int count, ...)
{
	int result{ 0 };
	va_list ap;
	va_start(ap, count);
	while (count > 0)
	{
		result += va_arg(ap, int);
		--count;
	}
	va_end(ap);
	return result;
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
static void __cdecl testor(bool(__fastcall* process)(int), int result, const wchar_t* success, const wchar_t* failure, const wchar_t* comment) noexcept
{
	bool fine = false;
	__try { fine = process(result); }
	__except (EXCEPTION_EXECUTE_HANDLER) {}
	std::wcout << (fine ? success : failure) << std::endl << comment << std::endl << std::endl;
};
static bool __fastcall test_ref(int result)
{
	dyn::function fn{ add };
	int rst = fn.operator ()<int, dyn::option::stdcall>(3, 4);
	return rst == result;
};
static bool glo_fn_callee_fine{ false };
static dyn::function glo_fn_callee{};
static bool __fastcall test_callee(int result)
{
	dyn::function fn{ callee };
	int rst = fn.operator ()<int, dyn::option::stdcall>(3, 4);
	glo_fn_callee = std::move(fn);
	return glo_fn_callee_fine = rst == result;
};
static bool __fastcall test_caller(int result)
{
	if (!glo_fn_callee_fine) { return false; }
	dyn::function fn{ caller };
	fn.operator []<intptr_t>(caller_offset) = reinterpret_cast<intptr_t>(glo_fn_callee.raw());
	int rst = fn.operator ()<int, dyn::option::stdcall>();
	return rst == result;
};
static bool __fastcall test_ptr(int result)
{
	void* ptr = dyn::fn<void>(&add);
	int rst = dyn::fn_call<int, dyn::option::stdcall>(ptr, 3, 4);
	return rst == result;
};
static bool glo_ptr_callee_fine{ false };
static void* glo_ptr_callee{};
static bool __fastcall test_ptr_callee(int result)
{
	void* ptr = dyn::fn_malloc(callee_sz);
	memcpy(ptr, callee, callee_sz);
	int rst = dyn::fn_call<int, dyn::option::stdcall>(ptr, 3, 4);
	glo_ptr_callee = ptr;
	return glo_ptr_callee_fine = rst == result;
};
static bool __fastcall test_ptr_caller(int result)
{
	if (!glo_ptr_callee_fine) { return false; }
	void* ptr = dyn::fn_malloc(caller_sz);
	memcpy(ptr, caller, caller_sz);
	reinterpret_cast<intptr_t&>(reinterpret_cast<std::uint8_t*>(ptr)[caller_offset]) = reinterpret_cast<intptr_t>(glo_ptr_callee);
	int rst = dyn::fn_call<int, dyn::option::stdcall>(ptr);
	dyn::fn_free(ptr);
	dyn::fn_free(glo_ptr_callee);
	return rst == result;
};
static bool __fastcall test_mem_ref(int result)
{
	box bx{ 8 };
	dyn::function fn{ &base::sub };
	/*/
	 *   In this version of msvc, &box::sub and &base::sub both are just an address
	 *   points to which the program transfer controls without that offset portion for
	 *   which the caller adjust the this pointer.
	/*/
	int rst = fn.operator ()<int, dyn::option::thiscall, base*>(&bx, 5);
	/*/
	 *   In the circumstances, the 3rd template argument of 'operator ()' significantly
	 *   aids for adjusting the pointer as if the program cases it dynamically within the
	 *   chain of inheritance.
	/*/
	return rst == bx.x && rst == result;
};
static bool __fastcall test_mem_callee(int result)
{
	box bx{ 8 };
	dyn::function fn{ ths_callee };
	int rst = fn.operator ()<int, dyn::option::thiscall, base*>(&bx, 5);
	return rst == bx.x && rst == result;
};
static bool __fastcall test_varargs_ref(int result)
{
	dyn::function fn{ sum };
	int rst = fn.operator ()<int, dyn::option::c_decl>(4, 37, 63, 87, 91);
	return rst = result;
};
static bool __fastcall test_varargs_ptr(int result)
{
	void* ptr = dyn::fn<void>(&sum);
	int rst = dyn::fn_call<int, dyn::option::c_decl>(ptr, 4, 37, 63, 87, 91);
	return rst == result;
};
int __cdecl main()
{
	constexpr int result = add(3, 4);
	testor(
		test_ref, result,
		L"#1:   The 'fn_ref' of type 'dyn::function' creates a reference pointed to by 'add' end up in Success.",
		L"#1:   The 'fn_ref' of type 'dyn::function' creates a reference pointed to by 'add' end up in Failure.",
		L"	-> Ask for whether the return value does match up what we expect."
	);
	testor(
		test_callee, result,
		L"#2:   The 'fn_callee' of type 'dyn::function' creates an instance end up in Success.",
		L"#2:   The 'fn_callee' of type 'dyn::function' creates an instance end up in Failure.",
		L"	-> The program creates new instances, the object code of that pure 'add' function given controls to yleid a value."
	);
	testor(
		test_caller, result,
		L"#3:   The 'fn_caller' of type 'dyn::function' creates an instance end up in Success.",
		L"#3:   The 'fn_caller' of type 'dyn::function' creates an instance end up in Failure.",
		L"	-> The program creates new instances, the object code of that caller stack frame processing dynamic relocation with operator as such."
	);
	testor(
		test_ptr, result,
		L"#4:   The 'ptr' with a raw pointer referring to 'add' invoked directly as a function end up in Success.",
		L"#4:   The 'ptr' with a raw pointer referring to 'add' invoked directly as a function end up in Failure.",
		L"	-> Test for an invocation with a raw pointer whether it works."
	);
	testor(
		test_ptr_callee, result,
		L"#5:   The 'ptr_callee' with a storage duration invoked directly as a function end up in Success.",
		L"#5:   The 'ptr_callee' with a storage duration invoked directly as a function end up in Failure.",
		L"	-> Test for an invocation with a pointer whether it works, while the object code of that pure 'add' function given controls to yleid a value."
	);
	testor(
		test_ptr_caller, result,
		L"#6:   The 'ptr_caller' with a storage duration invoked directly as a function end up in Success.",
		L"#6:   The 'ptr_caller' with a storage duration invoked directly as a function end up in Failure.",
		L"	-> Test for an invocation with a pointer whether it works, while the object code of that caller stack frame processing dynamic relocation with operator as such."
	);
	constexpr int mem_result = box{ 8 }.sub(5);
	testor(
		test_mem_ref, mem_result,
		L"#7:   The 'mem_ref' of type 'dyn::function' creates a reference pointed to by '&box::sub' end up in Success.",
		L"#7:   The 'mem_ref' of type 'dyn::function' creates a reference pointed to by '&box::sub' end up in Failure.",
		L"	-> Test for an invocation with a member function pointer on multiple inheritance whether it works."
	);
	testor(
		test_mem_callee, mem_result,
		L"#8:   The 'mem_callee' of type 'dyn::function' creates an instance end up in Success.",
		L"#8:   The 'mem_callee' of type 'dyn::function' creates an instance end up in Failure.",
		L"	-> Test for an invocation with a member function pointer on multiple inheritance with user inserted object codes whether it works."
	);
	int varargs_result = sum(4, 37, 63, 87, 91);
	testor(
		test_varargs_ref, varargs_result,
		L"#9:   The 'fn_varargs_ref' of type 'dyn::function' creates a reference pointed to by 'sum' end up in Success.",
		L"#9:   The 'fn_varargs_ref' of type 'dyn::function' creates a reference pointed to by 'sum' end up in Failure.",
		L"	-> Ask for whether the return value does match up what we expect when there is varargs."
	);
	testor(
		test_varargs_ptr, varargs_result,
		L"#10:   The 'ptr_varargs' with a raw pointer referring to '&sum' invoked directly as a function end up in Success.",
		L"#10:   The 'ptr_varargs' with a raw pointer referring to '&sum' invoked directly as a function end up in Failure.",
		L"	-> Test for an invocation with a raw pointer whether it works when there is varargs."
	);
	std::wstring line;
	std::getline(std::wcin, line);
	return EXIT_SUCCESS;
};
