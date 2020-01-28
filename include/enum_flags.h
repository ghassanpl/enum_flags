/// Copyright 2017-2020 Ghassan.pl
/// Usage of the works is permitted provided that this instrument is retained with
/// the works, so that any entity that uses the works is notified of this instrument.
/// DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
#pragma once

#include <type_traits>
#include <cstdint>
#include "flag_bits.h"

namespace enum_flags
{
	template <CONCEPT(detail::integral_or_enum) ENUM, CONCEPT(detail::bit_integral) VALUE_TYPE = uint64_t>
	struct enum_flags
	{
		using value_type = VALUE_TYPE;
		using enum_type = ENUM;
		using self_type = enum_flags;

		value_type bits = 0;

		constexpr enum_flags() noexcept = default;
		constexpr enum_flags(const enum_flags&) noexcept = default;
		constexpr enum_flags(enum_flags&&) noexcept = default;
		constexpr enum_flags& operator=(const enum_flags&) noexcept = default;
		constexpr enum_flags& operator=(enum_flags&&) noexcept = default;

		constexpr enum_flags(enum_type base_value) noexcept : bits(flag_bits<VALUE_TYPE>(base_value)) {}
		constexpr explicit enum_flags(value_type value) noexcept : bits(value) {}

		template <typename... ARGS>
		constexpr enum_flags(enum_type base_value, ARGS... args) noexcept : bits(flag_bits<VALUE_TYPE>(base_value, args...)) {}

		constexpr static enum_flags from_bits(value_type val) noexcept {
			enum_flags ret;
			ret.bits = val;
			return ret;
		}

		constexpr static enum_flags all() noexcept { return self_type::from_bits(~VALUE_TYPE{ 0 }); }

		constexpr bool is_set(enum_type flag) const noexcept { return (bits & flag_bits<VALUE_TYPE>(flag)) != 0; }

		template <typename... ARGS>
		constexpr bool are_any_set(ARGS... args) const noexcept
		{
			return (this->is_set(args) || ...);
		}

		/// are_any_set({}) is true, which is correct or not, depending on whether you want it to be correct or not :P
		constexpr bool are_any_set(enum_flags other) const noexcept { return other.bits == 0 /* empty set */ || (bits & other.bits) != 0; }

		template <typename... ARGS>
		constexpr bool are_all_set(ARGS... args) const noexcept
		{
			return (this->is_set(args) && ...);
		}

		constexpr bool are_all_set(enum_flags other) const noexcept { return (bits & other.bits) == other.bits; }
		
		constexpr explicit operator bool() const noexcept { return bits != 0; }
		constexpr enum_type to_enum_type() const noexcept { return (enum_type)bits; }

		template <typename... ARGS>
		constexpr enum_flags& set(ARGS... args) noexcept { bits |= flag_bits<VALUE_TYPE>(args...); return *this; }
		constexpr enum_flags& set(enum_flags other) noexcept { bits |= other.bits; return *this; }

		template <typename... ARGS>
		constexpr enum_flags& unset(ARGS... args) noexcept { bits &= ~ flag_bits<VALUE_TYPE>(args...); return *this; }
		constexpr enum_flags& unset(enum_flags other) noexcept { bits &= ~other.bits; return *this; }

		template <typename... ARGS>
		constexpr enum_flags& toggle(ARGS... args) noexcept { bits ^= flag_bits<VALUE_TYPE>(args...); return *this; }
		constexpr enum_flags& toggle(enum_flags other) noexcept { bits ^= other.bits; return *this; }

		template <typename... ARGS>
		constexpr enum_flags& set_to(bool val, ARGS... args) noexcept
		{
			if (val)
				bits |= flag_bits<VALUE_TYPE>(args...);
			else
				bits &= ~ flag_bits<VALUE_TYPE>(args...);
			return *this;
		}

		constexpr enum_flags operator+(enum_type flag) const noexcept { return self_type::from_bits(bits | flag_bits<VALUE_TYPE>(flag)); }
		constexpr enum_flags operator-(enum_type flag) const noexcept { return self_type::from_bits(bits & ~ flag_bits<VALUE_TYPE>(flag)); }

		constexpr enum_flags& operator+=(enum_type flag) noexcept { bits |= flag_bits<VALUE_TYPE>(flag); return *this; }
		constexpr enum_flags& operator-=(enum_type flag) noexcept { bits &= ~ flag_bits<VALUE_TYPE>(flag); return *this; }

		constexpr enum_flags& operator+=(enum_flags flag) noexcept { bits |= flag.bits; return *this; }
		constexpr enum_flags& operator-=(enum_flags flag) noexcept { bits &= ~flag.bits; return *this; }

		constexpr bool operator==(enum_flags other) const noexcept { return bits == other.bits; }
		constexpr bool operator!=(enum_flags other) const noexcept { return bits != other.bits; }
	};

#undef NS
}