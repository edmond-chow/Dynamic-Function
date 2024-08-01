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
#pragma pack(push)
#pragma push_macro("CALL")
#pragma push_macro("DYN_FUNC_CALL")
#pragma push_macro("DYN_FUNC_NATIVE_CALL")
#pragma push_macro("DYN_FUNC_INSTANCE_CALL")
#ifndef _WIN64
#pragma pack(4)
#else
#pragma pack(8)
#endif
#define CALL(c) c
#ifdef _DEBUG
#define DYN_FUNC_CALL CALL(__stdcall)
#else
#define DYN_FUNC_CALL CALL(__fastcall)
#endif
#define DYN_FUNC_NATIVE_CALL CALL(__cdecl)
#define DYN_FUNC_INSTANCE_CALL CALL(__thiscall)
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
	thread_local captured_registers __cr{};
	thread_local const captured_registers& cr = __cr;
	extern "C" void __cdecl capture() {};
	extern "C" const void* const __cdecl cr_ptr() { return &__cr; }
	static const struct init {
		const HANDLE heap;
		init() : heap(HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 1024, 65536)) {};
		~init() { HeapDestroy(heap); };
	} __heap_init;
	extern "C" void* DYN_FUNC_NATIVE_CALL malloc_func(std::size_t s_t) { return HeapAlloc(__heap_init.heap, HEAP_NO_SERIALIZE, s_t); };
	extern "C" void* DYN_FUNC_NATIVE_CALL realloc_func(void* ptr, std::size_t s_t) { return HeapReAlloc(__heap_init.heap, HEAP_NO_SERIALIZE, ptr, s_t); };
	extern "C" void DYN_FUNC_NATIVE_CALL free_func(void* ptr) { HeapFree(__heap_init.heap, HEAP_NO_SERIALIZE, ptr); };
	std::size_t DYN_FUNC_CALL sizeof_function(const void* func)
	{
		const std::uint8_t* ite = static_cast<const std::uint8_t*>(func);
		for (; *ite != 0xC3; ++ite) {}
		++ite;
		return ite - static_cast<const std::uint8_t*>(func);
	};
	struct byte
	{
	private:
		std::uint8_t __byte;
	public:
		constexpr byte() noexcept : __byte() {};
		constexpr byte(const byte&) noexcept = default;
		constexpr byte(byte&&) noexcept = default;
		constexpr byte(std::uint8_t __byte) noexcept : __byte(__byte) {};
		constexpr byte& operator =(const byte&) & noexcept = default;
		constexpr byte& operator =(byte&&) & noexcept = default;
		constexpr ~byte() noexcept = default;
		void* DYN_FUNC_CALL operator new(std::size_t s_t);
		void* DYN_FUNC_CALL operator new(std::size_t s_t, void* ptr);
		void DYN_FUNC_CALL operator delete(void* ptr);
		void* DYN_FUNC_CALL operator new[](std::size_t s_t);
		void* DYN_FUNC_CALL operator new[](std::size_t s_t, void* ptr);
		void DYN_FUNC_CALL operator delete[](void* ptr);
	};
	void* DYN_FUNC_CALL byte::operator new(std::size_t s_t) { return malloc_func(s_t); };
	void* DYN_FUNC_CALL byte::operator new(std::size_t s_t, void* ptr) { return ptr; };
	void DYN_FUNC_CALL byte::operator delete(void* ptr) { free_func(ptr); };
	void* DYN_FUNC_CALL byte::operator new[](std::size_t s_t) { return malloc_func(s_t); };
	void* DYN_FUNC_CALL byte::operator new[](std::size_t s_t, void* ptr) { return ptr; };
	void DYN_FUNC_CALL byte::operator delete[](void* ptr) { free_func(ptr); }
	class function
	{
	private:
		byte* data;
		std::size_t s_t;
	public:
		DYN_FUNC_INSTANCE_CALL function() noexcept;
		DYN_FUNC_INSTANCE_CALL function(std::size_t s_t);
		DYN_FUNC_INSTANCE_CALL function(const void* func, std::size_t s_t);
		DYN_FUNC_INSTANCE_CALL function(const function& func);
		DYN_FUNC_INSTANCE_CALL function(function&& func) noexcept;
		function& DYN_FUNC_INSTANCE_CALL operator =(const function& func) &;
		function& DYN_FUNC_INSTANCE_CALL operator =(function&& func) & noexcept;
		friend bool DYN_FUNC_CALL operator ==(const function& lhs, const function& rhs);
		const void* DYN_FUNC_INSTANCE_CALL instance() const;
		std::size_t DYN_FUNC_INSTANCE_CALL size() const;
		DYN_FUNC_INSTANCE_CALL ~function() noexcept;
	};
	DYN_FUNC_INSTANCE_CALL function::function() noexcept : data(nullptr), s_t(0) {};
	DYN_FUNC_INSTANCE_CALL function::function(std::size_t s_t) : data(new byte[s_t]), s_t(s_t) { memset(this->data, 0xCC, s_t); };
	DYN_FUNC_INSTANCE_CALL function::function(const void* func, std::size_t s_t) : data(new byte[s_t]), s_t(s_t) { memcpy(this->data, func, s_t); };
	DYN_FUNC_INSTANCE_CALL function::function(const function& func) : data(new byte[func.s_t]), s_t(func.s_t) { memcpy(this->data, func.data, func.s_t); };
	DYN_FUNC_INSTANCE_CALL function::function(function&& func) noexcept : data(func.data), s_t(func.s_t) { func.data = nullptr; };
	function& DYN_FUNC_INSTANCE_CALL function::operator =(const function& func) &
	{
		if (*this == func) { return *this; }
		delete[] this->data;
		this->data = new byte[s_t];
		this->s_t = func.s_t;
		memcpy(this->data, func.data, func.s_t);
		return *this;
	};
	function& DYN_FUNC_INSTANCE_CALL function::operator =(function&& func) & noexcept
	{
		this->data = func.data;
		this->s_t = func.s_t;
		func.data = nullptr;
		return *this;
	};
	bool DYN_FUNC_CALL operator ==(const function& lhs, const function& rhs) { return lhs.data == rhs.data && lhs.s_t == rhs.s_t; };
	const void* DYN_FUNC_INSTANCE_CALL function::instance() const { return this->data; };
	std::size_t DYN_FUNC_INSTANCE_CALL function::size() const { return this->s_t; };
	DYN_FUNC_INSTANCE_CALL function::~function() noexcept { free_func(this->data); };
}
#pragma pop_macro("DYN_FUNC_INSTANCE_CALL")
#pragma pop_macro("DYN_FUNC_NATIVE_CALL")
#pragma pop_macro("DYN_FUNC_CALL")
#pragma pop_macro("CALL")
#pragma pack(pop)
