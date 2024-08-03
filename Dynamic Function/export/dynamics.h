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
#else
#define NODISCARD
#endif
#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && (_MSC_VER >= 1800))
#define CONSTEXPR20 constexpr
#else
#define CONSTEXPR20
#endif
namespace dyn
{
	/* infrastructures */
	extern "C" __declspec(dllimport) void* __stdcall fn_malloc(std::size_t sz);
	extern "C" __declspec(dllimport) void* __stdcall fn_realloc(void* ptr, std::size_t sz);
	extern "C" __declspec(dllimport) void __stdcall fn_free(void* ptr);
	extern "C" __declspec(dllimport) int __stdcall fn_call(void* ptr);
	/* specializes */
	template <std::size_t sz, typename... Args>
	struct traitor : std::bool_constant<false> {};
	template <std::size_t sz, typename Arg, typename... Others>
	struct traitor<sz, Arg, Others...> : public traitor<sz - 1, Others...> {};
	template <typename Arg, typename... Others>
	struct traitor<0, Arg, Others...> : public std::bool_constant<true>
	{
	public:
		using result = Arg;
	};
	template <>
	struct traitor<0> : public std::bool_constant<false> {};
	template <typename Ret, typename... Args>
	struct func_prototype
	{
	public:
		static constexpr std::size_t size = sizeof...(Args) + 1;
		using ret = Ret;
		template <std::size_t sz>
		using args = typename traitor<sz, Args...>::result;
	};
	template <typename Fn>
	struct func_traits : std::bool_constant<false> {};
#pragma push_macro("FUNC_TRAITS")
#pragma push_macro("FUNC_TRAITS_WITH_NO_EXCEPT")
#define FUNC_TRAITS(CALL_OPT, NO_EXCEPT)\
	template <typename Ret, typename... Args>\
	struct func_traits<Ret CALL_OPT(Args...) NO_EXCEPT> : public func_prototype<Ret, Args...>, public std::bool_constant<true>\
	{\
	public:\
		using proto = func_prototype<Ret, Args...>;\
	};
#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L) && (_MSC_VER >= 1800))
#define FUNC_TRAITS_WITH_NO_EXCEPT(CALL_OPT)\
	FUNC_TRAITS(CALL_OPT, )\
	FUNC_TRAITS(CALL_OPT, noexcept)
#else
#define FUNC_TRAITS_WITH_NO_EXCEPT(CALL_OPT)\
	FUNC_TRAITS(CALL_OPT, )
#endif
	FUNC_TRAITS_WITH_NO_EXCEPT(__cdecl)
#ifndef _WIN64
		FUNC_TRAITS_WITH_NO_EXCEPT(__stdcall)
		FUNC_TRAITS_WITH_NO_EXCEPT(__fastcall)
#endif
		FUNC_TRAITS_WITH_NO_EXCEPT(__vectorcall)
#pragma pop_macro("FUNC_TRAITS_WITH_NO_EXCEPT")
#pragma pop_macro("FUNC_TRAITS")
#if (__cplusplus >= 201402L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201402L) && (_MSC_VER >= 1800))
		template <typename Fn>
	constexpr bool func_traits_v = func_traits<Fn>::value;
#endif
	template <typename Fn, typename... Args, typename = typename std::enable_if<func_traits<Fn>::value>::type>
	typename func_traits<Fn>::ret fn_call(void* ptr, Args... args)
	{
		union {
			void* pointer;
			Fn* invoke;
		} caller{ ptr };
		return caller.invoke == nullptr ? typename func_traits<Fn>::ret{} : caller.invoke(std::forward<Args>(args)...);
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
		byte* obj;
		std::size_t sz;
		std::size_t cap;
	public:
		constexpr function() noexcept
			: obj{ nullptr }, sz{ 0 }, cap{ 0 }
		{};
		CONSTEXPR20 function(const function& other)
			: obj{ new byte[other.sz] }, sz{ other.sz }, cap{ other.sz }
		{
			std::copy_n(other.obj, other.sz, this->obj);
		};
		CONSTEXPR20 function(function&& other) noexcept
			: obj{ other.obj }, sz{ other.sz }, cap{ other.sz }
		{
			other.obj = nullptr;
			other.sz = 0;
			other.cap = 0;
		};
		function(void* ptr, std::size_t sz)
			: obj{ new byte[sz] }, sz{ sz }, cap{ sz }
		{
			std::copy_n(reinterpret_cast<const byte*>(ptr), sz, this->obj);
		};
		template <std::size_t sz>
		CONSTEXPR20 function(const std::uint8_t(&dat)[sz])
			: obj{ new byte[sz] }, sz{ sz }, cap{ sz }
		{
			std::copy_n(reinterpret_cast<const byte*>(dat), sz, this->obj);
		};
		CONSTEXPR20 function(std::size_t cap)
			: obj{ new byte[cap] }, sz{ cap }, cap{ cap }
		{
			std::fill_n(this->obj, cap, byte{ 0xc3 });
		};
		CONSTEXPR20 function& operator =(const function& other) &
		{
			if (this == &other) { return *this; }
			if (this->sz != other.sz && this->cap < other.sz)
			{
				delete[] this->obj;
				this->obj = new byte[other.sz];
			}
			this->sz = other.sz;
			std::copy_n(other.obj, other.sz, this->obj);
			return *this;
		};
		CONSTEXPR20 function& operator =(function&& other) & noexcept
		{
			if (this == &other) { return *this; }
			delete[] this->obj;
			this->obj = other.obj;
			this->sz = other.sz;
			this->cap = other.cap;
			other.obj = nullptr;
			other.sz = 0;
			other.cap = 0;
			return *this;
		};
		CONSTEXPR20 ~function() noexcept
		{
			delete[] this->obj;
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
		template <typename Fn, typename... Args, typename = typename std::enable_if<func_traits<Fn>::value>::type>
		typename func_traits<Fn>::ret operator ()(Args... args)
		{
			union {
				byte* object;
				Fn* invoke;
			} caller{ this->obj };
			return caller.invoke == nullptr ? typename func_traits<Fn>::ret{} : caller.invoke(std::forward<Args>(args)...);
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
