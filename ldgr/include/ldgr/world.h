#pragma once
#include "ldgr/api.h"
#include "ldgr/entity.h"
#include "ldgr/type_utils.h"
#include "ldgr/bag.h"
#include <cpprelude/hash_array.h>
#include <cpprelude/slinked_list.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/platform.h>

namespace ldgr
{
	struct Entity_Ledger_Entry
	{
		ID id;
		cpprelude::dynamic_array<Base_Component> _components;

		API_LDGR Entity_Ledger_Entry(ID entity_id = INVALID_ID,
			cpprelude::memory_context *context = cpprelude::platform->global_memory);
	};

	struct World
	{
		cpprelude::memory_context *_mem_context;
		ID_Bag _entities_bag;
		// Bag<Base_Component> _components_bag;
		// ID _id_generator;
		// cpprelude::hash_array<ID, Entity_Ledger_Entry> _entities;

		API_LDGR World(cpprelude::memory_context *context = cpprelude::platform->global_memory);

		API_LDGR Entity
		create_entity();

		template<typename TEntity>
		void
		insert_entity(TEntity& entity)
		{
			entity.id = _entities_bag.insert();
			entity.world = this;
		}

		template<typename TEntity>
		void
		insert_entity(TEntity&& entity)
		{
			insert_entity(entity);
		}

		template<typename TEntity>
		bool
		remove_entity(TEntity& entity)
		{
			bool result = _entities_bag.remove(entity.id);
			entity.id = INVALID_ID;
			entity.world = nullptr;
			return result;
		}

		template<typename TEntity>
		bool
		remove_entity(Entity&& entity)
		{
			return remove_entity(entity);
		}

		API_LDGR bool
		remove_entity(ID entity_id);

		template<typename TEntity>
		bool
		entity_exists(const TEntity& entity) const
		{
			return _entities_bag.has(entity.id);
		}

		API_LDGR bool
		entity_exists(ID entity_id) const;

		// template<typename T>
		// Component<T>
		// create_component(const Entity& entity, const T& data)
		// {
		// 	Component<T> result;
		// 	result.id = _id_generator++;
		// 	result.entity = entity;
		// 	result._type_utils = type_utils<T>();
		// 	result._data = _mem_context->template alloc<byte>(sizeof(T));
		// 	new (result._data) T(data);
		// 	_entities[entity.id]._components.emplace_back(result);
		// 	return result;
		// }

		// API_LDGR cpprelude::dynamic_array<Base_Component>&
		// get_all_components(const Entity& entity);

		// API_LDGR const cpprelude::dynamic_array<Base_Component>&
		// get_all_components(const Entity& entity) const;

		// API_LDGR cpprelude::dynamic_array<Base_Component>&
		// get_all_components(ID entity_id);

		// API_LDGR const cpprelude::dynamic_array<Base_Component>&
		// get_all_components(ID entity_id) const;

		// template<typename T>
		// Component<T>
		// get_component(const Entity& entity)
		// {
		// 	return get_component<T>(entity.id);
		// }

		// template<typename T>
		// Component<T>
		// get_component(ID entity_id)
		// {
		// 	Component<T> result;
		// 	auto it = _entities.lookup(entity_id);
		// 	if (it == _entities.end())
		// 		return result;

		// 	auto type = type_utils<T>();
		// 	for (const auto& component : it->_components)
		// 	{
		// 		if (component._type_utils->hash == type->hash &&
		// 			component._type_utils->name == type->name)
		// 		{
		// 			result.id = component.id;
		// 			result.entity = component.entity;
		// 			result._data = component._data;
		// 			result._type_utils = component._type_utils;
		// 			break;
		// 		}
		// 	}

		// 	return result;
		// }
	};

	struct Base_Entity
	{
		ID id;
		World *world;

		Base_Entity()
			:id(INVALID_ID),
			 world(nullptr)
		{}

		Base_Entity(const Base_Entity& other) = delete;

		Base_Entity&
		operator=(const Base_Entity& other) = delete;

		Base_Entity(Base_Entity&& other)
			:id(other.id),
			 world(other.world)
		{
			other.id = INVALID_ID;
			other.world = nullptr;
		}

		Base_Entity&
		operator=(Base_Entity&& other)
		{
			//reset the entity
			if(world)
				world->remove_entity(id);

			//move the values
			id = other.id;
			world = other.world;
			//reset the other
			other.id = INVALID_ID;
			other.world = nullptr;
		}

		virtual ~Base_Entity()
		{
			if(world)
				world->remove_entity(id);
			id = INVALID_ID;
			world = nullptr;
		}
	};
}