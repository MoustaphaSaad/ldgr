#pragma once
#include "ldgr/api.h"
#include <cpprelude/string.h>
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

		template <typename T>
		static constexpr cpprelude::string
		get_type_name(void)
		{
			return internal::Name_Helper<T>::type_name();
		}
	}

	struct Base_Type_Utils
	{
		cpprelude::string name;
		cpprelude::usize size;

		virtual cpprelude::string
		stuff()
		{
			using namespace cpprelude;
			return "base"_cs;
		}
	};

	template<typename T>
	struct Type_Utils final: Base_Type_Utils
	{
		Type_Utils()
		{
			name = internal::get_type_name<T>();
			size = sizeof(T);
		}

		cpprelude::string
		stuff() override
		{
			using namespace cpprelude;
			return "concrete"_cs;
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