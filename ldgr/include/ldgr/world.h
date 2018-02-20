#pragma once
#include "ldgr/api.h"
#include "ldgr/entity.h"
#include <cpprelude/hash_array.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/platform.h>

namespace ldgr
{
	struct Component_Ledger_Entry
	{
		ID id;
		cpprelude::slice<cpprelude::byte> _data;
	};

	struct Entity_Ledger_Entry
	{
		ID id;
		cpprelude::dynamic_array<Component_Ledger_Entry> _components;

		API_LDGR Entity_Ledger_Entry(ID entity_id = INVALID_ID,
			cpprelude::memory_context *context = cpprelude::platform->global_memory);
	};

	struct World
	{
		cpprelude::memory_context *_mem_context;
		ID _id_generator;
		cpprelude::hash_array<ID, Entity_Ledger_Entry> _entities;

		API_LDGR World(cpprelude::memory_context *context = cpprelude::platform->global_memory);

		API_LDGR Entity
		create_entity();

		API_LDGR bool
		remove_entity(Entity& entity);

		API_LDGR bool
		remove_entity(Entity&& entity);

		API_LDGR bool
		remove_entity(const ID& entity_id);

		template<typename T>
		Component<T>
		create_component(const Entity& entity, const T& data)
		{
			Component<T> result;
			result.id = _id_generator++;
			result.entity = entity;
			result._type_utils = type_utils<T>();
			result._data = _mem_context->template alloc<byte>(sizeof(T));
			new (result._data) T(data);
			_entities[entity.id]._components.emplace_back(Component_Ledger_Entry{ result.id, result._data });
			return result;
		}
	};
}