#if (__cplusplus >= 201103L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201103L) && (_MSC_VER >= 1800))
#pragma once
#ifndef __DYN_FUNC__
#define __DYN_FUNC__
#include <type_traits>
#pragma pack(push)
#pragma push_macro("CALL")
#pragma push_macro("DYN_FUNC_CALL")
#pragma push_macro("DYN_FUNC_NATIVE_CALL")
#pragma push_macro("DYN_FUNC_INSTANCE_CALL")
#pragma push_macro("ENABLE_IF")
#pragma push_macro("REQUIRES")
#pragma push_macro("CONSTEXPR20")
#pragma push_macro("NODISCARD")
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
#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && (_MSC_VER >= 1800))
#define ENABLE_IF(value)
#define REQUIRES(value) requires value
#define CONSTEXPR20 constexpr
#else
#define ENABLE_IF(value) , typename = typename std::enable_if<value>::type
#define REQUIRES(value)
#define CONSTEXPR20
#endif
#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L) && (_MSC_VER >= 1800))
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif
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
	extern thread_local const captured_registers& cr;
	void DYN_FUNC_CALL capture();
	extern "C" void* DYN_FUNC_NATIVE_CALL malloc_func(std::size_t s_t);
	extern "C" void* DYN_FUNC_NATIVE_CALL realloc_func(void* ptr, std::size_t s_t);
	extern "C" void DYN_FUNC_NATIVE_CALL free_func(void* ptr);
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
		CONSTEXPR20 ~byte() noexcept = default;
		NODISCARD void* DYN_FUNC_CALL operator new(std::size_t s_t);
		NODISCARD void* DYN_FUNC_CALL operator new(std::size_t s_t, void* ptr);
		NODISCARD void DYN_FUNC_CALL operator delete(void* ptr);
		NODISCARD void* DYN_FUNC_CALL operator new[](std::size_t s_t);
		NODISCARD void* DYN_FUNC_CALL operator new[](std::size_t s_t, void* ptr);
		NODISCARD void DYN_FUNC_CALL operator delete[](void* ptr);
		template <class IntegerType>
		friend constexpr byte operator <<(byte ths, IntegerType shift) noexcept;
		template <class IntegerType>
		friend constexpr byte operator >>(byte ths, IntegerType shift) noexcept;
		friend constexpr byte operator |(byte left, byte right) noexcept;
		friend constexpr byte operator &(byte left, byte right) noexcept;
		friend constexpr byte operator ^(byte left, byte right) noexcept;
		friend constexpr byte operator ~(byte ths) noexcept;
		template <class IntegerType>
		constexpr byte& operator<<=(IntegerType shift) & noexcept { *this = *this << shift; };
		template <class IntegerType>
		constexpr byte& operator>>=(IntegerType shift) & noexcept { *this = *this >> shift; };
		NODISCARD constexpr byte& operator |=(byte right) & noexcept { return *this = *this | right; }
		NODISCARD constexpr byte& operator &=(byte right) & noexcept { return *this = *this & right; }
		NODISCARD constexpr byte& operator ^=(byte right) & noexcept { return *this = *this ^ right; }
	};
	template <class IntegerType>
	NODISCARD constexpr byte operator <<(byte ths, IntegerType shift) noexcept { ths.__byte << shift; };
	template <class IntegerType>
	NODISCARD constexpr byte operator >>(byte ths, IntegerType shift) noexcept { ths.__byte >> shift; };
	NODISCARD constexpr byte operator |(byte left, byte right) noexcept { return left.__byte | right.__byte; }
	NODISCARD constexpr byte operator &(byte left, byte right) noexcept { return left.__byte & right.__byte; }
	NODISCARD constexpr byte operator ^(byte left, byte right) noexcept { return left.__byte ^ right.__byte; }
	NODISCARD constexpr byte operator ~(byte ths) noexcept { return ~ths.__byte; }
	template <std::size_t s_t, typename... Args>
	struct traitor : std::bool_constant<false> {};
	template <std::size_t s_t, typename Arg, typename... Others>
	struct traitor<s_t, Arg, Others...> : public traitor<s_t - 1, Others...> {};
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
		template <std::size_t s_t>
		using args = typename traitor<s_t, Args...>::result;
	};
	template <typename Func>
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
	template <typename Func>
	constexpr bool func_traits_v = func_traits<Func>::value;
#endif
	std::size_t DYN_FUNC_CALL sizeof_function(const void* func);
	template <typename Func ENABLE_IF(func_traits<Func>::value)>
	std::size_t sizeof_function(Func* func) REQUIRES(func_traits_v<Func>)
	{
		const union Union { const void* const data; Func* func; } Union{ func };
		return sizeof_function(Union.data);
	};
	template <std::size_t size>
	constexpr std::size_t sizeof_function(const std::uint8_t(&__opcode)[size]) { return size; };
	class function
	{
	private:
		void* data;
		std::size_t s_t;
	public:
		DYN_FUNC_INSTANCE_CALL function() noexcept;
		explicit DYN_FUNC_INSTANCE_CALL function(std::size_t s_t);
		DYN_FUNC_INSTANCE_CALL function(const void* func, std::size_t s_t);
		template <std::size_t size>
		function(const std::uint8_t(&__opcode)[size]) : data(nullptr), s_t(size)
		{
			while (this->data == nullptr) { this->data = malloc_func(size); }
			memcpy(this->data, __opcode, size);
		};
#pragma push_macro("SIZE")
#pragma push_macro("OPCODE")
#pragma push_macro("OFFSET")
#ifndef _WIN64
#define SIZE static_cast<std::size_t>(7)
#define OPCODE std::uint8_t __opcode[7]{ 0xB8, 0x00, 0x00, 0x00, 0x00 ,0xFF, 0xE0, };
#define OFFSET &__opcode[1]
#else
#define SIZE static_cast<std::size_t>(12)
#define OPCODE std::uint8_t __opcode[12]{ 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ,0xFF, 0xE0, };
#define OFFSET &__opcode[2]
#endif
		template <typename Func ENABLE_IF(func_traits<Func>::value)>
		function(Func* func) REQUIRES(func_traits_v<Func>) : data(nullptr), s_t(func == nullptr ? 0 : SIZE)
		{
			if (func == nullptr) { return; }
			while (this->data == nullptr) { this->data = malloc_func(SIZE); }
			const union Union { const void* const data; Func* func; } Union{ func };
			OPCODE
			*reinterpret_cast<intptr_t*>(OFFSET) = reinterpret_cast<const intptr_t>(Union.data);
			memcpy(this->data, __opcode, SIZE);
		};
#pragma pop_macro("OFFSET")
#pragma pop_macro("OPCODE")
#pragma pop_macro("SIZE")
		DYN_FUNC_INSTANCE_CALL function(const function& func);
		DYN_FUNC_INSTANCE_CALL function(function&& func) noexcept;
		function& DYN_FUNC_INSTANCE_CALL operator =(const function& func) &;
		function& DYN_FUNC_INSTANCE_CALL operator =(function&& func) & noexcept;
		template <typename Func, typename... Args ENABLE_IF(func_traits<Func>::value && (func_traits<Func>::size == sizeof...(Args) + 1))>
		typename func_traits<Func>::ret Invoke(Args... args) const REQUIRES(func_traits_v<Func> && (func_traits<Func>::size == sizeof...(Args) + 1))
		{
			const union Union { const void* const data; Func* func; } Union{ this->data };
			return Union.func == nullptr ? typename func_traits<Func>::ret() : Union.func(args...);
		};
		const void* DYN_FUNC_INSTANCE_CALL instance() const;
		std::size_t DYN_FUNC_INSTANCE_CALL size() const;
		DYN_FUNC_INSTANCE_CALL ~function() noexcept;
	};
	bool DYN_FUNC_CALL operator ==(const function& lhs, const function& rhs);
}
#pragma pop_macro("NODISCARD")
#pragma pop_macro("CONSTEXPR20")
#pragma pop_macro("REQUIRES")
#pragma pop_macro("ENABLE_IF")
#pragma pop_macro("DYN_FUNC_INSTANCE_CALL")
#pragma pop_macro("DYN_FUNC_NATIVE_CALL")
#pragma pop_macro("DYN_FUNC_CALL")
#pragma pop_macro("CALL")
#pragma pack(pop)
#endif
#endif
