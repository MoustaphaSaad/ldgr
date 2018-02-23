#pragma once
#include "ldgr/api.h"
#include "ldgr/entity.h"
#include "ldgr/type_utils.h"
#include "ldgr/bag.h"
#include <cpprelude/hash_array.h>
#include <cpprelude/dynamic_array.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/platform.h>
#include <cpprelude/string.h>
#include <cassert>

namespace cpprelude
{
	template<>
	struct hash<ldgr::Base_Type_Utils*>
	{
		inline usize
		operator()(ldgr::Base_Type_Utils* ptr) const
		{
			return reinterpret_cast<usize>(ptr);
		}
	};
}

namespace ldgr
{
	struct Entity_Ledger_Entry
	{
		ID id;
		cpprelude::hash_set<ID> _components_ids;
		cpprelude::hash_array<cpprelude::string, ID> _components_by_name;

		API_LDGR Entity_Ledger_Entry(ID entity_id = INVALID_ID,
			cpprelude::memory_context *context = cpprelude::platform->global_memory);
	};

	struct Component_Type_Ledger_Entry
	{
		cpprelude::memory_context* _component_context;
		Base_Type_Utils* _type_utils;
		cpprelude::hash_set<ID> _components;

		API_LDGR Component_Type_Ledger_Entry(Base_Type_Utils* type = nullptr,
			cpprelude::memory_context* array_context = cpprelude::platform->global_memory,
			cpprelude::memory_context* component_context = cpprelude::platform->global_memory);
	};

	struct World
	{
		cpprelude::memory_context *_mem_context;
		ID_Bag _entities_bag;
		Bag<Base_Component> _components_bag;
		cpprelude::hash_array<ID, Entity_Ledger_Entry> _entities_ldgr;
		cpprelude::hash_array<Base_Type_Utils*, Component_Type_Ledger_Entry> _components_by_type;

		API_LDGR World(cpprelude::memory_context *context = cpprelude::platform->global_memory);

		//Entity Functions
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
			bool result = remove_entity(entity.id);
			entity.id = INVALID_ID;
			entity.world = nullptr;
			return result;
		}

		template<typename TEntity>
		bool
		remove_entity(Entity&& entity)
		{
			bool result = remove_entity(entity.id);
			entity.id = INVALID_ID;
			entity.world = nullptr;
			return result;
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

		API_LDGR void
		reserve_entites(cpprelude::usize expected_size);


		//Component Functions
		template<typename TComponent, typename TEntity>
		Component<TComponent>
		create_component(const TEntity& entity, const cpprelude::string& name, const TComponent& data)
		{
			return create_component(entity.id, name, data);
		}

		template<typename TComponent>
		Component<TComponent>
		create_component(ID entity_id, const cpprelude::string& name, const TComponent& data)
		{
			assert(!has_component(entity_id, name));

			Component<TComponent> result;
			result.entity.id = entity_id;
			result.entity.world = this;
			result.name = name;
			result._type_utils = type_utils<TComponent>();
			result._data = _mem_context->template alloc<byte>(sizeof(TComponent));
			new (result._data) TComponent(data);

			//insert the component into the world
			auto id = _components_bag.insert(result);
			auto& component = _components_bag[id];
			component.id = id;

			//add the id to the result component
			result.id = id;
			_add_component_to_entity(result.entity.id, result);
			return result;
		}

		template<typename TComponent, typename TEntity>
		Component<TComponent>
		create_component(const TEntity& entity, const cpprelude::string& name, TComponent&& data)
		{
			return create_component(entity.id, name, std::move(data));
		}

		template<typename TComponent>
		Component<TComponent>
		create_component(ID entity_id, const cpprelude::string& name, TComponent&& data)
		{
			assert(!has_component(entity_id, name));

			Component<TComponent> result;
			result.entity.id = entity_id;
			result.entity.world = this;
			result.name = name;
			result._type_utils = type_utils<TComponent>();
			result._data = _mem_context->template alloc<byte>(sizeof(TComponent));
			new (result._data) TComponent(std::move(data));

			//insert the component into the world
			auto id = _components_bag.insert(result);
			auto& component = _components_bag[id];
			component.id = id;

			//add the id to the result component
			result.id = id;
			_add_component_to_entity(result.entity.id, result);
			return result;
		}

		template<typename TComponent, typename TEntity, typename ... TArgs>
		Component<TComponent>
		emplace_component(const TEntity& entity, const cpprelude::string& name, TArgs&& ... args)
		{
			return create_component(entity.id, name, std::forward<TArgs>(args)...);
		}

		template<typename TComponent, typename ... TArgs>
		Component<TComponent>
		emplace_component(ID entity_id, const cpprelude::string& name, TArgs&& ... args)
		{
			assert(!has_component(entity_id, name));

			Component<TComponent> result;
			result.entity.id = entity_id;
			result.entity.world = this;
			result.name = name;
			result._type_utils = type_utils<TComponent>();
			result._data = _mem_context->template alloc<byte>(sizeof(TComponent));
			new (result._data) TComponent(std::forward<TArgs>(args)...);

			//insert the component into the world
			auto id = _components_bag.insert(result);
			auto& component = _components_bag[id];
			component.id = id;

			//add the id to the result component
			result.id = id;
			_add_component_to_entity(result.entity.id, result);
			return result;
		}

		template<typename TEntity>
		bool
		has_component(const TEntity& entity, const cpprelude::string& name)
		{
			return has_component(entity.id, name);
		}

		API_LDGR bool
		has_component(ID entity_id, const cpprelude::string& name);

		template<typename TEntity>
		bool
		has_component(const TEntity& entity, ID component_id)
		{
			return has_component(entity.id, component_id);
		}

		API_LDGR bool
		has_component(ID entity_id, ID component_id);

		API_LDGR bool
		remove_component(ID component_id);


		template<typename TComponent>
		void
		_add_component_to_entity(ID entity_id, const Component<TComponent>& component)
		{
			//insert into the _entities_ldgr
			{
				auto it = _entities_ldgr.lookup(entity_id);
				//no entry for the entity then add it
				if(it == _entities_ldgr.end())
					it = _entities_ldgr.insert(entity_id, Entity_Ledger_Entry(entity_id, _mem_context));
				//add the component id
				it->_components_ids.insert(component.id);
				it->_components_by_name.insert(component.name, component.id);
			}

			//insert into the components ldgr
			{
				auto it = _components_by_type.lookup(component._type_utils);
				if(it == _components_by_type.end())
				{
					it = _components_by_type.insert(component._type_utils,
													Component_Type_Ledger_Entry(component._type_utils,
														_mem_context, _mem_context));
				}
				it->_components.insert(component.id);
			}
		}

		API_LDGR void
		_destroy_component(ID component_id);

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