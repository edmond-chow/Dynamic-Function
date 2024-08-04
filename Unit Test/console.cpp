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
#include <string>
#include <iostream>
#include <dynamics.h>
/* Compiler optimizing the assembly, the generated object code goes a bit difference. */
int __stdcall add(int x, int y)
{
	return x + y;
};
#ifndef _WIN64
inline constexpr std::uint8_t caller[]{
	0x6a, 0x04, 0x6a, 0x03, 0xb8, 0x00, 0x00, 0x00, 0x00, 0xff, 0xd0, 0xc3
};
/*
	0:  6a 04                   push   0x4
	2:  6a 03                   push   0x3
	4:  b8 00 00 00 00          mov    eax,0x0
	9:  ff d0                   call   eax
	b:  c3                      ret
*/
inline constexpr std::size_t caller_offset = 5;
inline constexpr std::uint8_t callee[]{
	0x8b, 0x44, 0x24, 0x04, 0x03, 0x44, 0x24, 0x08, 0xc2, 0x08, 0x00
};
/*
	0:  8b 44 24 04             mov    eax,DWORD PTR [esp+0x4]
	4:  03 44 24 08             add    eax,DWORD PTR [esp+0x8]
	8:  c2 08 00                ret    0x8
*/
#else
inline constexpr std::uint8_t caller[]{
	0xba, 0x04, 0x00, 0x00, 0x00, 0xb9, 0x03, 0x00, 0x00, 0x00, 0x48, 0xb8, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xd0, 0xc3
};
/*
	0:  ba 04 00 00 00          mov    edx,0x4
	5:  b9 03 00 00 00          mov    ecx,0x3
	a:  48 b8 00 00 00 00 00    movabs rax,0x0
	11: 00 00 00
	14: ff d0                   call   rax
	16: c3                      ret
*/
inline constexpr std::size_t caller_offset = 12;
inline constexpr std::uint8_t callee[]{
	0x8d, 0x04, 0x11, 0xc3
};
/*
	0:  8d 04 11                lea    eax,[rcx+rdx*1]
	3:  c3                      ret
*/
#endif
int main()
{
	dyn::function fn_reference{ add };
	int result = fn_reference.operator()<int __stdcall(int, int)>(3, 4);
	void* fn_caller = dyn::fn_malloc(std::extent_v<decltype(caller)>);
	memcpy(fn_caller, caller, std::extent_v<decltype(caller)>);
	void* fn_callee = dyn::fn_malloc(std::extent_v<decltype(callee)>);
	memcpy(fn_callee, callee, std::extent_v<decltype(callee)>);
	*reinterpret_cast<intptr_t*>(&reinterpret_cast<std::uint8_t*>(fn_caller)[caller_offset]) = reinterpret_cast<intptr_t>(fn_callee);
	dyn::function fn_object{ callee };
	int dyn_result = dyn::fn_call(fn_caller);
	int dyn_call_result = dyn::fn_call<int __stdcall(int, int)>(fn_callee, 3, 4);
	int dyn_fn_object = fn_object.operator ()<int __stdcall(int, int)>(3, 4);
	dyn::fn_free(fn_callee);
	dyn::fn_free(fn_caller);
	if (result == dyn_result && result == dyn_call_result && result == dyn_fn_object)
	{
		std::wcout << L"Success" << std::endl;
	}
	else
	{
		std::wcout << L"Failure" << std::endl;
	}
	std::wstring line;
	std::getline(std::wcin, line);
};
