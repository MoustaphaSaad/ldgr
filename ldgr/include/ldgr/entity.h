#pragma once
#include <cpprelude/defines.h>
#include <cpprelude/memory.h>
#include <ldgr/type_utils.h>

namespace ldgr
{
	using ID = cpprelude::usize;
	constexpr static ID INVALID_ID = static_cast<ID>(-1);

	struct World;
	struct Entity
	{
		ID id = INVALID_ID;
		World *world = nullptr;
	};

	struct Base_Component
	{
		ID id = INVALID_ID;
		Entity entity;
		Base_Type_Utils *_type_utils;
		cpprelude::slice<cpprelude::byte> _data;
	};

	template<typename T>
	struct Component: Base_Component
	{
		T*
		operator->()
		{
			return (T*)_data.ptr;
		}

		const T*
		operator->() const
		{
			return (const T*)_data.ptr;
		}

		T&
		operator*()
		{
			return (*(T*)_data.ptr);
		}

		const T&
		operator*() const
		{
			return (*(const T*)_data.ptr);
		}
	};
}