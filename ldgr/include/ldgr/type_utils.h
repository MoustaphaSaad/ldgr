#pragma once
#include <cpprelude/string.h>
#include <cpprelude/memory.h>
#include <string.h>

namespace ldgr
{
	namespace internal
	{
		static constexpr cpprelude::usize FRONT_SIZE = sizeof("ldgr::internal::Name_Helper<") - 1U;
		static constexpr cpprelude::usize BACK_SIZE = sizeof(">::type_name") - 1U;

		template <typename T>
		struct Name_Helper
		{
			static constexpr cpprelude::string
			type_name(void)
			{
				static const cpprelude::usize size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
				static char typeName[size] = {};
				memcpy(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u);

				return cpprelude::string(cpprelude::make_slice(typeName, size), nullptr);
			}
		};

		static constexpr cpprelude::usize
		type_id(void)
		{
			static cpprelude::usize generator = 0;
			return generator++;
		}

		template <typename T>
		constexpr cpprelude::string
		get_type_name(void)
		{
			return internal::Name_Helper<T>::type_name();
		}

		template<typename T>
		constexpr cpprelude::usize
		get_type_id(void)
		{
			static cpprelude::usize id = internal::type_id();
			return id;
		}
	}

	struct Base_Type_Utils
	{
		cpprelude::string name;
		cpprelude::usize id;
		cpprelude::usize size;
		cpprelude::usize alignment;

		virtual void
		invoke_destructor(const cpprelude::slice<cpprelude::byte>& data) = 0;
	};

	template<typename T>
	struct Type_Utils final: Base_Type_Utils
	{
		Type_Utils()
		{
			name = internal::get_type_name<T>();
			id = internal::get_type_id<T>();
			size = sizeof(T);
			alignment = alignof(T);
		}

		void
		invoke_destructor(const cpprelude::slice<cpprelude::byte>& data) override
		{
			auto values = data.template convert<T>();
			for(usize i = 0; i < values.count(); ++i)
				values[i].~T();
		}
	};

	template<typename T>
	constexpr Type_Utils<T>*
	type_utils()
	{
		static Type_Utils<T> utils;
		return &utils;
	}
}