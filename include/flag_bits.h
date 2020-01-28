/// Copyright 2017-2020 Ghassan.pl
/// Usage of the works is permitted provided that this instrument is retained with
/// the works, so that any entity that uses the works is notified of this instrument.
/// DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
#pragma once

#include <type_traits>
#include <cstdint>
#include "flag_bits_v.h"

#if __INTELLISENSE__ || !defined(_cpp_concepts)
#define concept inline constexpr bool
#define requires(...)
#define CONCEPT(...) typename
#else
#include <concepts>
#define CONCEPT(...) __VA_ARGS__
#endif

namespace enum_flags
{
	namespace detail
	{
		template <typename RESULT_TYPE, typename... ENUM_TYPES>
		concept valid_flag_bits_arguments = detail::bit_integral<RESULT_TYPE> && (detail::integral_or_enum<ENUM_TYPES> && ...);
	}

	template <typename RESULT_TYPE = uint64_t, typename... ARGS>
	constexpr RESULT_TYPE flag_bits(ARGS... args) noexcept
		requires (detail::valid_flag_bits_arguments<RESULT_TYPE, ARGS...>)
	{
		return ((RESULT_TYPE(1) << detail::to_underlying_type(args)) | ... | 0);
	}

	template <typename INTEGRAL, typename ENUM_TYPE>
		requires (detail::valid_flag_bits_arguments<INTEGRAL, ENUM_TYPE>)
	constexpr bool is_flag_set(INTEGRAL bits, ENUM_TYPE flag) noexcept { return (bits & flag_bits<INTEGRAL>(flag)) != 0; }

	template <typename INTEGRAL, typename... ARGS>
		requires (detail::valid_flag_bits_arguments<INTEGRAL, ARGS>)
	constexpr bool are_any_flags_set(INTEGRAL bits, ARGS... args) noexcept
	{
		return (is_flag_set(bits, args) || ...);
	}

	template <typename INTEGRAL, typename... ARGS>
		requires (detail::valid_flag_bits_arguments<INTEGRAL, ARGS>)
	constexpr bool are_all_flags_set(INTEGRAL bits, ARGS... args) noexcept
	{
		return (is_flag_set(bits, args) && ...);
	}

	template <typename INTEGRAL, typename... ARGS>
		requires (detail::valid_flag_bits_arguments<INTEGRAL, ARGS>)
	constexpr void set_flag(INTEGRAL& bits, ARGS... args) noexcept { bits |= flag_bits<INTEGRAL>(args...); }
	
	template <typename INTEGRAL, typename... ARGS>
		requires (detail::valid_flag_bits_arguments<INTEGRAL, ARGS>)
	constexpr void unset_flag(INTEGRAL& bits, ARGS... args) noexcept { bits &= ~ flag_bits<INTEGRAL>(args...); }
	
	template <typename INTEGRAL, typename... ARGS>
		requires (detail::valid_flag_bits_arguments<INTEGRAL, ARGS>)
	constexpr void toggle_flag(INTEGRAL& bits, ARGS... args) noexcept { bits ^= flag_bits<INTEGRAL>(args...); }
	
	template <typename INTEGRAL, typename... ARGS>
		requires (detail::valid_flag_bits_arguments<INTEGRAL, ARGS>)
	constexpr void set_flag_to(INTEGRAL& bits, bool val, ARGS... args) noexcept
	{
		if (val)
			bits |= flag_bits<INTEGRAL>(args...);
		else
			bits &= ~ flag_bits<INTEGRAL>(args...);
	}
}

#undef concept
#undef requires
#undef CONCEPT