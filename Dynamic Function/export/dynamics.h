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
#if (__cplusplus >= 201103L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201103L) && (_MSC_VER >= 1800))
#pragma once
#ifndef __DYN_FUNC__
#define __DYN_FUNC__
#include <cstddef>
#include <cstdint>
namespace dyn
{
	struct captured_registers
	{
	public:
		std::size_t ax;
		std::size_t bx;
		std::size_t cx;
		std::size_t dx;
		std::size_t si;
		std::size_t di;
		std::size_t ip;
		std::size_t sp;
		std::size_t bp;
#ifdef _WIN64
		std::size_t r8;
		std::size_t r9;
		std::size_t r10;
		std::size_t r11;
		std::size_t r12;
		std::size_t r13;
		std::size_t r14;
		std::size_t r15;
#endif
	};
	extern "C" __declspec(dllimport) void* __cdecl captured();
	extern "C" __declspec(dllimport) void __stdcall capture_registers();
	extern "C" __declspec(dllimport) void* __stdcall fn_malloc(std::size_t s_t);
	extern "C" __declspec(dllimport) void* __stdcall fn_realloc(void* ptr, std::size_t s_t);
	extern "C" __declspec(dllimport) void __stdcall fn_free(void* ptr);
	extern "C" __declspec(dllimport) std::ptrdiff_t fn_size(const void* ptr);
}
#endif
#endif
