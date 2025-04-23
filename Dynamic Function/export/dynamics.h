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
	enum struct option : std::size_t
	{
		c_decl = 0,
		stdcall = 1,
		fastcall = 2,
		thiscall = 3,
		vectorcall = 4,
	};
	template <std::size_t Ix, typename... Args>
	struct arg_traits
		: std::bool_constant<false>
	{};
	template <std::size_t Ix, typename Arg1, typename... Args>
	struct arg_traits<Ix, Arg1, Args...>
		: public arg_traits<Ix - 1, Args...>
	{};
	template <typename Arg, typename... Args>
	struct arg_traits<0, Arg, Args...>
		: public std::bool_constant<true>
	{
	public:
		using result = Arg;
	};
	template <>
	struct arg_traits<0>
		: public std::bool_constant<false>
	{};
	template <typename Ret, typename... Args>
	struct fn_proto
	{
	public:
		static constexpr std::size_t size = sizeof...(Args) + 1;
		using ret = Ret;
		template <std::size_t Ix>
		using args = typename arg_traits<Ix, Args...>::result;
	};
	template <typename Fn>
	struct fn_traits
		: public std::bool_constant<false>
	{};
	template <typename Ret, typename... Args>
	struct fn_traits<Ret __cdecl(Args......)>
	/*/
	 *   We just provide a specialises of 'fn_traits<Fn>' for varargs.
	 *   'Ret __cdecl(Args......)' and 'Ret(Args......)' are always the same.
	/*/
		: public fn_proto<Ret, Args...>, public std::bool_constant<true>
	{
	public:
		using proto = fn_proto<Ret, Args...>;
		static constexpr option opt = option::c_decl;
	};
	template <typename Ret, typename... Args>
	struct fn_traits<Ret __cdecl(Args...)>
	/*/
	 *   'Ret __cdecl(Args...)' and 'Ret(Args...)' are always the same.
	/*/
		: public fn_proto<Ret, Args...>, public std::bool_constant<true>
	{
	public:
		using proto = fn_proto<Ret, Args...>;
		static constexpr option opt = option::c_decl;
	};
#ifndef _WIN64
	template <typename Ret, typename... Args>
	struct fn_traits<Ret __stdcall(Args...)>
	/*/
	 *   'Ret __stdcall(Args...)' and 'Ret(Args...)' are the same in x64.
	/*/
		: public fn_proto<Ret, Args...>, public std::bool_constant<true>
	{
	public:
		using proto = fn_proto<Ret, Args...>;
		static constexpr option opt = option::stdcall;
	};
	template <typename Ret, typename... Args>
	struct fn_traits<Ret __fastcall(Args...)>
	/*/
	 *   'Ret __fastcall(Args...)' and 'Ret(Args...)' are the same in x64.
	/*/
		: public fn_proto<Ret, Args...>, public std::bool_constant<true>
	{
	public:
		using proto = fn_proto<Ret, Args...>;
		static constexpr option opt = option::fastcall;
	};
#endif
	template <typename Ret, typename... Args>
	struct fn_traits<Ret __vectorcall(Args...)>
		: public fn_proto<Ret, Args...>, public std::bool_constant<true>
	{
	public:
		using proto = fn_proto<Ret, Args...>;
		static constexpr option opt = option::vectorcall;
	};
#if (__cplusplus >= 201402L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201402L) && (_MSC_VER >= 1800))
	template <typename Fn>
	INLINE_VAR constexpr bool fn_traits_v = fn_traits<Fn>::value;
#endif
	template <option Opt = option::c_decl, typename Ret = void, typename... Args>
	struct make_fn;
	template <typename Ret, typename... Args>
	struct make_fn<option::c_decl, Ret, Args...>
	{
	public:
		/*/
		 *   'Ret __cdecl(Args...)' and 'Ret(Args...)' are always the same.
		/*/
		using type = Ret __cdecl(Args...);
	};
	template <typename Ret, typename... Args>
	struct make_fn<option::stdcall, Ret, Args...>
	{
	public:
		/*/
		 *   'Ret __stdcall(Args...)' and 'Ret(Args...)' are the same in x64.
		/*/
		using type = Ret __stdcall(Args...);
	};
	template <typename Ret, typename... Args>
	struct make_fn<option::fastcall, Ret, Args...>
	{
	public:
		/*/
		 *   'Ret __fastcall(Args...)' and 'Ret(Args...)' are the same in x64.
		/*/
		using type = Ret __fastcall(Args...);
	};
	template <typename Ret, typename Ths, typename... Args>
	struct make_fn<option::thiscall, Ret, Ths*, Args...>
	{
	public:
#ifndef _WIN64
		using type = Ret __fastcall(Ths*, int, Args...);
		/*/
		 *   In x86 mode, functions as such with thiscall likewise what we have in fastcall,
		 *   while both the callee the responsibility to clean up the stack for arguments
		 *   passed through by the caller, but fastcall using both ecx and edx registers pass
		 *   through the first 2 dword arguments while thiscall only using ecx register pass
		 *   through that pointer. As we inject the 2nd parameter of type 'int' to shift the
		 *   legacy of the parameters all pushed onto the stack if needed, whenever functions
		 *   are invoked where the edx register left as unspecified, preventing the abuse of
		 *   injection on object codes or inline assembly.
		/*/
#else
		using type = Ret __cdecl(Ths*, Args...);
		/*/
		 *   In x64 mode, such functions with thiscall is same as cdecl, while the this
		 *   argument treated as the first implicit parameter in which that pointer is
		 *   correspondence with rcx register to pass through with the default x64 calling
		 *   convention.
		/*/
#endif
	};
	template <typename Ret, typename... Args>
	struct make_fn<option::vectorcall, Ret, Args...>
	{
	public:
		using type = Ret __vectorcall(Args...);
	};
#if (__cplusplus >= 201402L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201402L) && (_MSC_VER >= 1800))
	template <dyn::option Opt, typename Ret, typename... Args>
	using make_fn_t = typename make_fn<Opt, Ret, Args...>::type;
#endif
	template <typename Rst, typename Dat>
	constexpr Rst* fn(Dat dat)
	{
		union casing {
			Dat data;
			Rst* result;
		} rst{ dat };
		return rst.result;
	};
	template <
		typename Fn, typename... Args,
		typename = typename std::enable_if<fn_traits<Fn>::value>::type
		/*/
		 *   This function prototype only specializes with a global function pointer in which
		 *   'fn_traits<Fn>' is constrained by type.
		/*/
	>
	typename fn_traits<Fn>::ret fn_call(void* ptr, Args... args)
	{
		Fn* invoker{ fn<Fn>(ptr) };
		return invoker == nullptr ? typename fn_traits<Fn>::ret{} : invoker(std::forward<Args>(args)...);
	};
	template <
		typename Ret = int, option Opt = option::c_decl, typename... Args,
		typename Fn = typename make_fn<Opt, Ret, Args...>::type,
		typename = typename std::enable_if<Opt != option::thiscall>::type
		/*/
		 *   This function prototype only specializes with a global function pointer at which
		 *   the 'Opt' non-type template argument controls.
		/*/
	>
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
		/*/
		 *   Default constructions treat as 'this->ref' in 'false' case.
		/*/
		constexpr function() noexcept
			: ref{ false }, obj{ nullptr }, sz{ 0 }, cap{ 0 }
		{};
		/*/
		 *   Copy constructions with a preprocessed move action collapse 'this->cap' to
		 *   'this->sz' with 'new byte[other.cap]'.
		/*/
		CONSTEXPR20 function(const function& other)
			: ref{ other.ref }, obj{ other.obj }, sz{ other.sz }, cap{ other.cap }
		{
			if (!other.ref && other.obj != nullptr)
			{
				this->obj = new byte[other.cap];
				std::copy_n(other.obj, other.sz, this->obj);
			}
		};
		/*/
		 *   Move constructions are fundamentally the way memberised every subobject of the
		 *   instance.
		/*/
		CONSTEXPR20 function(function&& other) noexcept
			: ref{ other.ref }, obj{ other.obj }, sz{ other.sz }, cap{ other.cap }
		{
			other.ref = false;
			other.obj = nullptr;
			other.sz = 0;
			other.cap = 0;
		};
		/*/
		 *   Constructions with a global function pointer treat as 'this->ref' in 'true' case
		 *   in which type of that pointer is erased.
		/*/
		template <typename Fn, typename = typename std::enable_if<fn_traits<Fn>::value>::type>
		constexpr function(Fn* invoker) noexcept
			: ref{ true }, obj{ nullptr }, sz{ 0 }, cap{ 0 }
		{
			this->obj = fn<byte>(invoker);
		};
#ifndef _WIN64
		/*/
		 *   Constructions with a member function pointer treat as 'this->ref' in 'true' case
		 *   in which type of that pointer is erased for cdecl.
		/*/
		template <typename Ty, typename Ret, typename... Args>
		constexpr function(Ret(__cdecl Ty::* invoker)(Args...)) noexcept
			: ref{ true }, obj{ nullptr }, sz{ 0 }, cap{ 0 }
		{
			this->obj = fn<byte>(invoker);
		};
		/*/
		 *   Constructions with a member function pointer treat as 'this->ref' in 'true' case
		 *   in which type of that pointer is erased for stdcall.
		/*/
		template <typename Ty, typename Ret, typename... Args>
		constexpr function(Ret(__stdcall Ty::* invoker)(Args...)) noexcept
			: ref{ true }, obj{ nullptr }, sz{ 0 }, cap{ 0 }
		{
			this->obj = fn<byte>(invoker);
		};
		/*/
		 *   Constructions with a member function pointer treat as 'this->ref' in 'true' case
		 *   in which type of that pointer is erased for fastcall.
		/*/
		template <typename Ty, typename Ret, typename... Args>
		constexpr function(Ret(__fastcall Ty::* invoker)(Args...)) noexcept
			: ref{ true }, obj{ nullptr }, sz{ 0 }, cap{ 0 }
		{
			this->obj = fn<byte>(invoker);
		};
#endif
		/*/
		 *   Constructions with a member function pointer treat as 'this->ref' in 'true' case
		 *   in which type of that pointer is erased for vectorcall.
		/*/
		template <typename Ty, typename Ret, typename... Args>
		constexpr function(Ret(__vectorcall Ty::* invoker)(Args...)) noexcept
			: ref{ true }, obj{ nullptr }, sz{ 0 }, cap{ 0 }
		{
			this->obj = fn<byte>(invoker);
		};
		/*/
		 *   Constructions with a member function pointer treat as 'this->ref' in 'true' case
		 *   in which type of that pointer is erased for thiscall.
		/*/
		template <typename Ty, typename Ret, typename... Args>
		/*/
		 *   'Ret(__thiscall Ty::*)(Args...)' and 'Ret(Ty::*)(Args...)' are always the same.
		/*/
		constexpr function(Ret(__thiscall Ty::* invoker)(Args...)) noexcept
			: ref{ true }, obj{ nullptr }, sz{ 0 }, cap{ 0 }
		{
			this->obj = fn<byte>(invoker);
		};
		/*/
		 *   Constructions with a storage duration to a given size treat as 'this->ref' in
		 *   'false' case.
		/*/
		function(void* ptr, std::size_t sz)
			: ref{ false }, obj{ new byte[sz] }, sz{ sz }, cap{ sz }
		{
			std::copy_n(reinterpret_cast<const byte*>(ptr), sz, this->obj);
		};
		/*/
		 *   Constructions with a fixed size array and object codes thereof treat as
		 *   'this->ref' in 'false' case.
		/*/
		template <std::size_t Sz>
		CONSTEXPR20 function(const std::uint8_t(&dat)[Sz])
			: ref{ false }, obj{ new byte[Sz] }, sz{ Sz }, cap{ Sz }
		{
			std::copy_n(reinterpret_cast<const byte*>(dat), Sz, this->obj);
		};
		/*/
		 *   Constructions with a given capacity treat as 'this->ref' in 'false' case.
		/*/
		CONSTEXPR20 function(std::size_t cap)
			: ref{ false }, obj{ new byte[cap] }, sz{ cap }, cap{ cap }
		{
			std::fill_n(this->obj, cap, byte{ 0xc3 });
		};
		/*/
		 *   Copy assignment operators involve copy construction with initializer list
		 *   whenever not be in self-assignment, forwarding the value of 'other.ref' and
		 *   'other.sz', move action only works with the exclusing case of the condition of
		 *   if-clause thereof within the copy constructor.
		/*/
		CONSTEXPR20 function& operator =(const function& other) &
		{
			if (this == &other) { return *this; }
			this->ref = other.ref;
			this->sz = other.sz;
			/*/
			 *   The instance constructed with a function pointer or default constructor at which
			 *   the code not held on any storage duration allocated.
			/*/
			if (other.ref || other.obj == nullptr)
			{
				this->obj = other.obj;
				this->cap = other.cap;
				return *this;
			}
			/*/
			 *   The capacity is extended when the storage duration is not enough space.
			/*/
			else if (this->sz != other.sz && this->cap < other.cap)
			{
				delete[] this->obj;
				this->obj = new byte[other.cap];
				this->cap = other.cap;
			}
			std::copy_n(other.obj, other.sz, this->obj);
			return *this;
		};
		/*/
		 *   Move assignment operators with 'delete[] this->obj' including the exclusing case
		 *   of the condition of if-clause thereof within the copy constructor involve move
		 *   construction with initializer list whenever not be in self-assignment.
		/*/
		CONSTEXPR20 function& operator =(function&& other) & noexcept
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
		/*/
		 *   Destructions with 'delete[] this->obj' including the exclusing case of the
		 *   condition of if-clause thereof within the copy constructor involve default
		 *   construction.
		/*/
		CONSTEXPR20 ~function() noexcept 
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
		template <
			typename Fn, typename... Args,
			typename = typename std::enable_if<fn_traits<Fn>::value>::type
		>
		typename fn_traits<Fn>::ret operator ()(Args... args) const &
		{
			Fn* invoker{ fn<Fn>(this->obj) };
			return invoker == nullptr ? typename fn_traits<Fn>::ret{} : invoker(std::forward<Args>(args)...);
		};
		template <
			typename Ret = int, option Opt = option::c_decl, typename... Args,
			typename Fn = typename make_fn<Opt, Ret, Args...>::type,
			typename = typename std::enable_if<Opt != option::thiscall>::type
			/*/
			 *   This function prototype only specializes with a global function pointer at which
			 *   the 'Opt' non-type template argument controls.
			/*/
		>
		Ret operator ()(Args... args) const &
		{
			return this->operator ()<Fn>(std::forward<Args>(args)...);
		};
		template <
			typename Ret = int, option Opt = option::thiscall, typename Ths = void*, typename... Args,
			typename Fn = typename make_fn<Opt, Ret, Ths, Args...>::type,
			typename = typename std::enable_if<Opt == option::thiscall>::type
			/*/
			 *   This function prototype only specializes with a member function pointer at which
			 *   the 'Opt' non-type template argument controls.
			/*/
		>
		Ret operator ()(Ths ths, Args... args) const &
		{
#ifndef _WIN64
			/*/
			 *   In x86 mode, functions as such with thiscall likewise what we have in fastcall,
			 *   while both the callee the responsibility to clean up the stack for arguments
			 *   passed through by the caller, but fastcall using both ecx and edx registers pass
			 *   through the first 2 dword arguments while thiscall only using ecx register pass
			 *   through that pointer. As we inject the 2nd parameter of type 'int' to shift the
			 *   legacy of the parameters all pushed onto the stack if needed, whenever functions
			 *   are invoked where the edx register left as unspecified, preventing the abuse of
			 *   injection on object codes or inline assembly.
			/*/
			return this->operator ()<Fn>(ths, 0, std::forward<Args>(args)...);
#else
			/*/
			 *   In x64 mode, such functions with thiscall is same as cdecl, while the this
			 *   argument treated as the first implicit parameter in which that pointer is
			 *   correspondence with rcx register to pass through with the default x64 calling
			 *   convention.
			/*/
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
