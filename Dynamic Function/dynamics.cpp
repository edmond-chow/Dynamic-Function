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
#include <windows.h>
#include <cstddef>
#include <cstdint>
#include <utility>
namespace dyn
{
	static HANDLE fn_heap;
	static void create_fn_heap()
	{
		fn_heap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 1024, 65536);
	};
	static void destroy_fn_heap()
	{
		HeapDestroy(fn_heap);
	};
	extern "C" __declspec(dllexport) void* __stdcall fn_malloc(std::size_t s_t)
	{
		return HeapAlloc(fn_heap, 0, s_t);
	};
	extern "C" __declspec(dllexport) void* __stdcall fn_realloc(void* ptr, std::size_t s_t)
	{
		return HeapReAlloc(fn_heap, 0, ptr, s_t);
	};
	extern "C" __declspec(dllexport) void __stdcall fn_free(void* ptr)
	{
		HeapFree(fn_heap, 0, ptr);
	};
#ifndef _WIN64
	static const std::uint8_t call[] { 0xB8, 0x00, 0x00, 0x00, 0x00 ,0xFF, 0xE0 };
	/*
		0:  b8 00 00 00 00          mov    eax,0x0
		5:  ff e0                   jmp    eax
	*/
	static const std::size_t call_offset = 1;
#else
	static const std::uint8_t call[] { 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ,0xFF, 0xE0 };
	/*
		0:  48 b8 00 00 00 00 00    movabs rax,0x0
		7:  00 00 00
		a:  ff e0                   jmp    rax
	*/
	static const std::size_t call_offset = 2;
#endif
	extern "C" __declspec(dllexport) int __stdcall fn_call(void* ptr)
	{
		union {
			void* pointer;
			std::uint8_t* data;
			int(__stdcall* invoke)();
		} caller{};
		caller.pointer = fn_malloc(std::extent_v<decltype(call)>);
		memcpy(caller.pointer, call, std::extent_v<decltype(call)>);
		*reinterpret_cast<intptr_t*>(&caller.data[call_offset]) = reinterpret_cast<intptr_t>(ptr);
		int result = caller.invoke();
		fn_free(caller.pointer);
		return result;
	};
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		dyn::create_fn_heap();
		break;
	case DLL_PROCESS_DETACH:
		dyn::destroy_fn_heap();
		break;
	}
	return TRUE;
};
