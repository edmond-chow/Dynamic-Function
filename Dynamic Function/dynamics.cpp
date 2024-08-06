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
