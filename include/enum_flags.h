/// Copyright 2017-2020 Ghassan.pl
/// Usage of the works is permitted provided that this instrument is retained with
/// the works, so that any entity that uses the works is notified of this instrument.
/// DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
#pragma once

#include "flag_bits.h"
#include <bit>

namespace ghassanpl
{
	template <detail::integral_or_enum ENUM, detail::valid_integral VALUE_TYPE = unsigned long long>
	struct enum_flags
	{
		using value_type = VALUE_TYPE;
		using enum_type = ENUM;
		using self_type = enum_flags;
		static constexpr bool enum_type_is_enum = std::is_enum_v<ENUM>;

		value_type bits = 0;

		constexpr enum_flags() noexcept = default;
		constexpr enum_flags(const enum_flags&) noexcept = default;
		constexpr enum_flags(enum_flags&&) noexcept = default;
		constexpr enum_flags& operator=(const enum_flags&) noexcept = default;
		constexpr enum_flags& operator=(enum_flags&&) noexcept = default;

		//constexpr explicit enum_flags(value_type value) noexcept : bits(value) {}

		//constexpr enum_flags(enum_type base_value) noexcept : bits(flag_bits<VALUE_TYPE>(base_value)) {}
		template <detail::integral_or_enum... ARGS>
		constexpr enum_flags(ARGS... args) noexcept : bits(flag_bits<VALUE_TYPE>(args...)) {}

		[[nodiscard]]
		constexpr static self_type from_bits(value_type val) noexcept {
			self_type ret;
			ret.bits = val;
			return ret;
		}

		[[nodiscard]]
		constexpr static self_type all() noexcept { return self_type::from_bits(~VALUE_TYPE{ 0 }); }

		template <detail::integral_or_enum T>
		[[nodiscard]]
		constexpr static self_type all(T last) noexcept { return self_type::from_bits(flag_bits<VALUE_TYPE>(last) | (flag_bits<VALUE_TYPE>(last) - 1)); }

		[[nodiscard]]
		constexpr static self_type none() noexcept { return {}; }

		template <detail::integral_or_enum T>
		[[nodiscard]]
		constexpr bool is_set(T flag) const noexcept { return (bits & flag_bits<VALUE_TYPE>(flag)) != 0; }

		template <detail::integral_or_enum... ARGS>
		[[nodiscard]]
		constexpr bool are_any_set(ARGS... args) const noexcept
		{
			return (this->is_set(args) || ...);
		}

		/// are_any_set({}) is true, which is correct or not, depending on whether you want it to be correct or not :P
		[[nodiscard]]
		constexpr bool are_any_set(self_type other) const noexcept { return other.bits == 0 /* empty set */ || (bits & other.bits) != 0; }

		template <detail::integral_or_enum... ARGS>
		[[nodiscard]]
		constexpr bool are_all_set(ARGS... args) const noexcept
		{
			return (this->is_set(args) && ...);
		}

		[[nodiscard]]
		constexpr bool are_all_set(self_type other) const noexcept { return (bits & other.bits) == other.bits; }
		
		constexpr explicit operator bool() const noexcept { return bits != 0; }
		[[nodiscard]]
		constexpr enum_type to_enum_type() const noexcept { return (enum_type)bits; }

		template <detail::integral_or_enum... ARGS>
		constexpr self_type& set(ARGS... args) noexcept { bits |= flag_bits<VALUE_TYPE>(args...); return *this; }
		constexpr self_type& set(self_type other) noexcept { bits |= other.bits; return *this; }

		template <detail::integral_or_enum... ARGS>
		constexpr self_type& unset(ARGS... args) noexcept { bits &= ~ flag_bits<VALUE_TYPE>(args...); return *this; }
		constexpr self_type& unset(self_type other) noexcept { bits &= ~other.bits; return *this; }

		template <detail::integral_or_enum... ARGS>
		constexpr self_type& toggle(ARGS... args) noexcept { bits ^= flag_bits<VALUE_TYPE>(args...); return *this; }
		constexpr self_type& toggle(self_type other) noexcept { bits ^= other.bits; return *this; }

		template <detail::integral_or_enum... ARGS>
		constexpr self_type& set_to(bool val, ARGS... args) noexcept
		{
			if (val) bits |= flag_bits<VALUE_TYPE>(args...); else bits &= ~flag_bits<VALUE_TYPE>(args...);
			return *this;
		}

		constexpr self_type& set_to(bool val, self_type other) noexcept
		{
			if (val) bits |= other.bits; else bits &= ~other.bits;
			return *this;
		}

		template <detail::integral_or_enum T>
		[[nodiscard]]
		constexpr self_type operator+(T flag) const noexcept { return self_type::from_bits(bits | flag_bits<VALUE_TYPE>(flag)); }
		template <detail::integral_or_enum T>
		[[nodiscard]]
		constexpr self_type operator-(T flag) const noexcept { return self_type::from_bits(bits & ~ flag_bits<VALUE_TYPE>(flag)); }

		template <detail::integral_or_enum T>
		constexpr self_type& operator+=(T flag) noexcept { bits |= flag_bits<VALUE_TYPE>(flag); return *this; }
		template <detail::integral_or_enum T>
		constexpr self_type& operator-=(T flag) noexcept { bits &= ~ flag_bits<VALUE_TYPE>(flag); return *this; }

		constexpr self_type& operator+=(self_type flag) noexcept { bits |= flag.bits; return *this; }
		constexpr self_type& operator-=(self_type flag) noexcept { bits &= ~flag.bits; return *this; }

		constexpr bool operator==(self_type other) const noexcept { return bits == other.bits; }
		constexpr bool operator!=(self_type other) const noexcept { return bits != other.bits; }

		/// TODO: begin() and end(), so we can iterate over the set bits
		/// TODO: See if this would be faster with a simple loop

		template <typename FUNC>
		constexpr auto for_each(FUNC&& callback) const
		{
			//using return_type = decltype(callback(enum_type{}));
			using return_type = std::invoke_result_t<FUNC, enum_type>;
			using bitset_type = std::make_unsigned_t<value_type>;
			auto bitset = static_cast<bitset_type>(bits);
			while (bitset)
			{
#pragma warning(suppress: 4146)
				const auto t = static_cast<bitset_type>(bitset & -bitset);
				const auto r = std::countr_zero(t);
				if constexpr (std::is_convertible_v<return_type, bool>)
				{
					if (auto ret = callback((enum_type)r)) return ret;
				}
				else
					callback((enum_type)r);
				bitset ^= t;
			}
			if constexpr (std::is_convertible_v<return_type, bool>)
				return return_type{};
		}
	};

}