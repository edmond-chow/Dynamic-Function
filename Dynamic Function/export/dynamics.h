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
#include <utility>
#pragma push_macro("NODISCARD")
#pragma push_macro("CONSTEXPR20")
#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L) && (_MSC_VER >= 1800))
#define NODISCARD [[nodiscard]]
#define INLINE_VAR inline
#else
#define NODISCARD
#define INLINE_VAR
#endif
#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && (_MSC_VER >= 1800))
#define CONSTEXPR20 constexpr
#else
#define CONSTEXPR20 inline
#endif
namespace dyn
{
	/* infrastructures */
	extern "C" __declspec(dllimport) void* __stdcall fn_malloc(std::size_t sz);
	extern "C" __declspec(dllimport) void* __stdcall fn_realloc(void* ptr, std::size_t sz);
	extern "C" __declspec(dllimport) void __stdcall fn_free(void* ptr);
	/* specializes */
	template <std::size_t Ix, typename... Args>
	struct argument_traits
		: std::bool_constant<false>
	{};
	template <std::size_t Ix, typename Arg, typename... Others>
	struct argument_traits<Ix, Arg, Others...>
		: public argument_traits<Ix - 1, Others...>
	{};
	template <typename Arg, typename... Others>
	struct argument_traits<0, Arg, Others...>
		: public std::bool_constant<true>
	{
	public:
		using result = Arg;
	};
	template <>
	struct argument_traits<0>
		: public std::bool_constant<false>
	{};
	template <typename Ret, typename... Args>
	struct function_proto
	{
	public:
		static constexpr std::size_t size = sizeof...(Args) + 1;
		using ret = Ret;
		template <std::size_t Ix>
		using args = typename argument_traits<Ix, Args...>::result;
	};
	template <typename Fn>
	struct function_traits
		: public std::bool_constant<false>
	{};
#pragma push_macro("FUNCTION_TRAITS")
#pragma push_macro("FUNCTION_TRAITS_WITH_NO_EXCEPT")
#define FUNCTION_TRAITS(CALL_OPT, NO_EXCEPT)\
	template <typename Ret, typename... Args>\
	struct function_traits<Ret CALL_OPT(Args...) NO_EXCEPT>\
		: public function_proto<Ret, Args...>, public std::bool_constant<true>\
	{\
	public:\
		using proto = function_proto<Ret, Args...>;\
	};
#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L) && (_MSC_VER >= 1800))
#define FUNCTION_TRAITS_WITH_NO_EXCEPT(CALL_OPT)\
	FUNCTION_TRAITS(CALL_OPT, )\
	FUNCTION_TRAITS(CALL_OPT, noexcept)
#else
#define FUNCTION_TRAITS_WITH_NO_EXCEPT(CALL_OPT)\
	FUNCTION_TRAITS(CALL_OPT, )
#endif
	FUNCTION_TRAITS_WITH_NO_EXCEPT(__cdecl)
#ifndef _WIN64
	FUNCTION_TRAITS_WITH_NO_EXCEPT(__stdcall)
	FUNCTION_TRAITS_WITH_NO_EXCEPT(__fastcall)
#endif
	FUNCTION_TRAITS_WITH_NO_EXCEPT(__vectorcall)
#pragma pop_macro("FUNCTION_TRAITS_WITH_NO_EXCEPT")
#pragma pop_macro("FUNCTION_TRAITS")
#if (__cplusplus >= 201402L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201402L) && (_MSC_VER >= 1800))
	template <typename Fn>
	INLINE_VAR constexpr bool function_traits_v = function_traits<Fn>::value;
#endif
	INLINE_VAR constexpr std::size_t call_opt_cdecl = 0;
	INLINE_VAR constexpr std::size_t call_opt_stdcall = 1;
	INLINE_VAR constexpr std::size_t call_opt_fastcall = 2;
	INLINE_VAR constexpr std::size_t call_opt_thiscall = 3;
	INLINE_VAR constexpr std::size_t call_opt_vectorcall = 4;
	template <std::size_t Opt = call_opt_cdecl, typename Ret = void, typename... Args>
	struct make_function_type;
	template <typename Ret, typename... Args>
	struct make_function_type<call_opt_cdecl, Ret, Args...>
	{
	public:
		using type = Ret __cdecl(Args...);
		static_assert(std::is_same<type, Ret(Args...)>::value, "Always true in any case!");
	};
	template <typename Ret, typename... Args>
	struct make_function_type<call_opt_stdcall, Ret, Args...>
	{
	public:
		using type = Ret __stdcall(Args...);
#ifdef _WIN64
		static_assert(std::is_same<type, Ret(Args...)>::value, "Always true in x64!");
#endif
	};
	template <typename Ret, typename... Args>
	struct make_function_type<call_opt_fastcall, Ret, Args...>
	{
	public:
		using type = Ret __fastcall(Args...);
#ifdef _WIN64
		static_assert(std::is_same<type, Ret(Args...)>::value, "Always true in x64!");
#endif
	};
	template <typename Ret, typename Ths, typename... Args>
	struct make_function_type<call_opt_thiscall, Ret, Ths*, Args...>
	{
	public:
#ifndef _WIN64
		using type = Ret __fastcall(Ths*, int, Args...);
		// In x86 mode, functions as such with thiscall likewise what we have in fastcall, while both the callee the responsibility to clean up the stack for arguments passed through by the caller, but fastcall using both ecx and edx registers pass through leftmost 2 arguments while thiscall only using ecx register pass through that pointer.
		// As we inject the 2nd parameter of type 'int' to shift the rest of the parameters all pushed onto the stack if needed, whenever the function is invoked the edx register leave as unspecified, preventing the abuse of injection in object codes or inline assembly.
#else
		using type = Ret __cdecl(Ths*, Args...);
		// In x64 mode, functions as such with thiscall is same as cdecl, while the this argument treated as the first implicit parameter in which that pointer is one-to-one correspondence with rcx register to pass through with x64 calling convention.
#endif
	};
	template <typename Ret, typename... Args>
	struct make_function_type<call_opt_vectorcall, Ret, Args...>
	{
	public:
		using type = Ret __vectorcall(Args...);
	};
	template <typename Fn, typename... Args, typename = typename std::enable_if<function_traits<Fn>::value>::type>
	typename function_traits<Fn>::ret fn_call(void* ptr, Args... args)
	{
		union {
			void* pointer;
			Fn* invoke;
		} caller{};
		caller.pointer = ptr;
		return caller.invoke == nullptr ? typename function_traits<Fn>::ret{} : caller.invoke(std::forward<Args>(args)...);
	};
	template <typename Ret = int, std::size_t Opt = call_opt_cdecl, typename... Args, typename Fn = typename make_function_type<Opt, Ret, Args...>::type>
	Ret fn_call(void* ptr, Args... args)
	{
		return fn_call<Fn>(ptr, std::forward<Args>(args)...);
	};
	/* functionalities */
	struct byte
	{
	private:
		std::uint8_t b;
	public:
		constexpr byte() noexcept
			: b{ 0 }
		{};
		constexpr byte(const byte&) noexcept = default;
		constexpr byte(byte&&) noexcept = default;
		constexpr byte(std::uint8_t b) noexcept : b{ b } {};
		constexpr explicit byte(int b) noexcept : b{ static_cast<std::uint8_t>(b) } {};
		constexpr byte& operator =(const byte&) & noexcept = default;
		constexpr byte& operator =(byte&&) & noexcept = default;
		CONSTEXPR20 ~byte() noexcept = default;
		NODISCARD void* operator new(std::size_t sz)
		{
			return fn_malloc(sz);
		};
		NODISCARD void* operator new(std::size_t sz, void* ptr)
		{
			return ptr;
		};
		NODISCARD void operator delete(void* ptr)
		{
			return fn_free(ptr);
		};
		NODISCARD void* operator new[](std::size_t sz)
		{
			return fn_malloc(sz);
		};
		NODISCARD void* operator new[](std::size_t sz, void* ptr)
		{
			return ptr;
		};
		NODISCARD void operator delete[](void* ptr)
		{
			return fn_free(ptr);
		};
		constexpr byte operator <<(int shift) noexcept
		{
			return byte{ this->b << shift };
		};
		constexpr byte operator >>(int shift) noexcept
		{
			return byte{ this->b >> shift };
		};
		NODISCARD friend constexpr byte operator |(const byte& left, const byte& right) noexcept
		{
			return byte{ left.b | right.b };
		};
		NODISCARD friend constexpr byte operator &(const byte& left, const byte& right) noexcept
		{
			return byte{ left.b & right.b };
		};
		NODISCARD friend constexpr byte operator ^(const byte& left, const byte& right) noexcept
		{
			return byte{ left.b ^ right.b };
		};
		constexpr byte operator ~() noexcept
		{
			return byte{ ~this->b };
		};
		NODISCARD friend constexpr bool operator ==(const byte& left, const byte& right) noexcept
		{
			return left.b == right.b;
		};
		NODISCARD friend constexpr bool operator !=(const byte& left, const byte& right) noexcept
		{
			return left.b != right.b;
		};
		constexpr byte& operator <<=(int shift) & noexcept
		{
			return *this = *this << shift;
		};
		constexpr byte& operator >>=(int shift) & noexcept
		{
			return *this = *this >> shift;
		};
		constexpr byte& operator |=(byte right) & noexcept
		{
			return *this = *this | right;
		};
		constexpr byte& operator &=(byte right) & noexcept
		{
			return *this = *this & right;
		};
		constexpr byte& operator ^=(byte right) & noexcept
		{
			return *this = *this ^ right;
		};
		constexpr const std::uint8_t& raw() & noexcept
		{
			return this->b;
		};
	};
	class function
	{
	private:
		bool ref;
		byte* obj;
		std::size_t sz;
		std::size_t cap;
	public:
		constexpr function() noexcept // Default constructions treat as 'this->ref' in 'false' case.
			: ref{ false }, obj{ nullptr }, sz{ 0 }, cap{ 0 }
		{};
		CONSTEXPR20 function(const function& other) // Copy constructions with a preprocessed move action collapse 'this->cap' to 'this->sz' with 'new byte[other.sz]'.
			: ref{ other.ref }, obj{ other.obj }, sz{ other.sz }, cap{ other.sz }
		{
			if (!other.ref && other.obj != nullptr)
			{
				this->obj = new byte[other.sz];
				std::copy_n(other.obj, other.sz, this->obj);
			}
		};
		CONSTEXPR20 function(function&& other) noexcept // Move constructions are fundamentally the way memberised every subobject of the instance.
			: ref{ other.ref }, obj{ other.obj }, sz{ other.sz }, cap{ other.cap }
		{
			other.ref = false;
			other.obj = nullptr;
			other.sz = 0;
			other.cap = 0;
		};
		template <typename Fn, typename = typename std::enable_if<function_traits<Fn>::value>::type>
		constexpr function(Fn* invoker) noexcept // Constructions with a function reference treat as 'this->ref' in 'true' case.
			: ref{ true }, obj{ nullptr }, sz{ 0 }, cap{ 0 }
		{
			union {
				byte* object;
				Fn* invoke;
			} caller{};
			caller.invoke = invoker;
			this->obj = caller.object;
		};
		template <typename Ty, typename Ret, typename... Args>
		constexpr function(Ret(Ty::* invoker)(Args...)) noexcept // Constructions with a member function reference treat as 'this->ref' in 'true' case.
			: ref{ true }, obj{ nullptr }, sz{ 0 }, cap{ 0 }
		{
			union {
				byte* object;
				Ret(Ty::* invoke)(Args...);
			} caller{};
			caller.invoke = invoker;
			this->obj = caller.object;
		};
		function(void* ptr, std::size_t sz) // Constructions with a storage duration treat as 'this->ref' in 'false' case.
			: ref{ false }, obj{ new byte[sz] }, sz{ sz }, cap{ sz }
		{
			std::copy_n(reinterpret_cast<const byte*>(ptr), sz, this->obj);
		};
		template <std::size_t sz>
		CONSTEXPR20 function(const std::uint8_t(&dat)[sz]) // Constructions with object codes treat as 'this->ref' in 'false' case.
			: ref{ false }, obj{ new byte[sz] }, sz{ sz }, cap{ sz }
		{
			std::copy_n(reinterpret_cast<const byte*>(dat), sz, this->obj);
		};
		CONSTEXPR20 function(std::size_t cap) // Constructions with a given capacity treat as 'this->ref' in 'false' case.
			: ref{ false }, obj{ new byte[cap] }, sz{ cap }, cap{ cap }
		{
			std::fill_n(this->obj, cap, byte{ 0xc3 });
		};
		CONSTEXPR20 function& operator =(const function& other) & // Copy assignment operators involve copy construction with initializer list whenever not be in self-assignment, forwarding the value of 'other.ref', move action only works with the exclusing case of the condition of if-clause thereof within the copy constructor.
		{
			if (this == &other) { return *this; }
			this->ref = other.ref;
			if (other.ref || other.obj == nullptr) // The instance constructed with a function reference or default constructor at which the code not held on any storage duration allocated.
			{
				this->obj = other.obj;
				this->sz = other.sz;
				this->cap = other.sz;
				return *this;
			}
			else if (this->sz != other.sz && this->cap < other.sz) // Extending capacity when the storage duration is not enough space.
			{
				delete[] this->obj;
				this->obj = new byte[other.sz];
			}
			this->sz = other.sz;
			std::copy_n(other.obj, other.sz, this->obj);
			return *this;
		};
		CONSTEXPR20 function& operator =(function&& other) & noexcept // Move assignment operators with 'delete[] this->obj' including the exclusing case of the condition of if-clause thereof within the copy constructor involve move construction with initializer list whenever not be in self-assignment.
		{
			if (this == &other) { return *this; }
			if (!this->ref && this->obj != nullptr) { delete[] this->obj; }
			this->ref = other.ref;
			this->obj = other.obj;
			this->sz = other.sz;
			this->cap = other.cap;
			other.ref = false;
			other.obj = nullptr;
			other.sz = 0;
			other.cap = 0;
			return *this;
		};
		CONSTEXPR20 ~function() noexcept // Destructions with 'delete[] this->obj' including the exclusing case of the condition of if-clause thereof within the copy constructor involve default construction.
		{
			if (!this->ref && this->obj != nullptr) { delete[] this->obj; }
			this->ref = false;
			this->obj = nullptr;
			this->sz = 0;
			this->cap = 0;
		};
		NODISCARD friend constexpr bool operator ==(const function& left, const function& right) noexcept
		{
			if (left.sz != right.sz) { return false; }
			for (std::size_t i = 0; i < left.sz; ++i)
			{
				if (left.obj[i] != right.obj[i]) { return false; }
			}
			return true;
		};
		NODISCARD friend constexpr bool operator !=(const function& left, const function& right) noexcept
		{
			return !(left == right);
		};
		CONSTEXPR20 byte& operator [](std::size_t sz) & noexcept
		{
			return this->obj[sz];
		};
		CONSTEXPR20 const byte& operator [](std::size_t sz) const & noexcept
		{
			return this->obj[sz];
		};
		template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
		CONSTEXPR20 T& operator [](std::size_t sz) & noexcept
		{
			return reinterpret_cast<T&>(this->obj[sz]);
		};
		template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
		CONSTEXPR20 const T& operator [](std::size_t sz) const & noexcept
		{
			return reinterpret_cast<const T&>(this->obj[sz]);
		};
		template <typename Fn, typename... Args, typename = typename std::enable_if<function_traits<Fn>::value>::type>
		typename function_traits<Fn>::ret operator ()(Args... args) const &
		{
			union {
				byte* object;
				Fn* invoke;
			} caller{};
			caller.object = this->obj;
			return caller.invoke == nullptr ? typename function_traits<Fn>::ret{} : caller.invoke(std::forward<Args>(args)...);
		};
		template <typename Ret = int, std::size_t Opt = call_opt_cdecl, typename... Args, typename Fn = typename make_function_type<Opt, Ret, Args...>::type, typename = typename std::enable_if<Opt != call_opt_thiscall>::type>
		Ret operator ()(Args... args) const &
		{
			return this->operator ()<Fn>(std::forward<Args>(args)...);
		};
		template <typename Ret = int, std::size_t Opt = call_opt_thiscall, typename Ths = void, typename... Args, typename Fn = typename make_function_type<Opt, Ret, Ths*, Args...>::type, typename = typename std::enable_if<Opt == call_opt_thiscall>::type>
		Ret operator ()(Ths* ths, Args... args) const &
		{
#ifndef _WIN64
			return this->operator ()<Fn>(ths, 0, std::forward<Args>(args)...);
#else
			return this->operator ()<Fn>(ths, std::forward<Args>(args)...);
#endif
		};
		CONSTEXPR20 const byte* raw() const & noexcept
		{
			return this->obj;
		};
		CONSTEXPR20 std::size_t size() const & noexcept
		{
			return this->sz;
		};
	};
}
#pragma pop_macro("CONSTEXPR20")
#pragma pop_macro("NODISCARD")
#endif
#endif
