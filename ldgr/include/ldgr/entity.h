#pragma once
#include <cpprelude/defines.h>
#include <cpprelude/string.h>
#include "ldgr/type_utils.h"

namespace ldgr
{
	struct ID
	{
		cpprelude::u32 id = -1;
		cpprelude::u32 version = -1;

		inline bool
		operator==(const ID& other) const
		{
			return id == other.id && version == other.version;
		}

		inline bool
		operator!=(const ID& other) const
		{
			return !operator==(other);
		}

		inline bool
		valid()
		{
			return (id == -1 && version == -1);
		}
	};

	struct Internal_Component
	{
		ID id;
		ID entity;
		cpprelude::string name;
		Base_Type_Utils* _type;
		void* _data;
	};

	struct World;

	template<typename T>
	struct Component
	{
		ID id;
		T *_data;
		World *_world;

		T*
		operator->()
		{
			return _data;
		}

		const T*
		operator->() const
		{
			return _data;
		}

		T&
		operator*()
		{
			return *_data;
		}

		const T&
		operator*() const
		{
			return *_data;
		}
	};
}