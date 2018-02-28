#pragma once
#include <cpprelude/memory_context.h>
#include <cpprelude/platform.h>
#include "ldgr/api.h"
#include "ldgr/bag.h"
#include "ldgr/entity.h"

namespace ldgr
{
	struct Component_Type_Entry
	{
		Base_Type_Utils *type;
		cpprelude::memory_context* allocator;
		Value_Sparse_Set<cpprelude::u32, Internal_Component> components;

		Component_Type_Entry(Base_Type_Utils* component_type,
			cpprelude::memory_context *context = cpprelude::platform->global_memory)
			:type(component_type),
			 allocator(context),
			 components(context)
		{}
	};

	struct World
	{
		cpprelude::memory_context *_mem_context;
		cpprelude::u32 _version_generator;
		Sparse_Set<cpprelude::u32> _entities;
		//this is components arranged in a slots each slot represent a type
		cpprelude::dynamic_array<Component_Type_Entry> _components_by_type;

		API_LDGR World(cpprelude::memory_context *context = cpprelude::platform->global_memory);

		API_LDGR ID
		entity_create();

		API_LDGR void
		entity_reserve(cpprelude::usize expected_size);

		API_LDGR void
		entity_remove(ID& entity);

		API_LDGR void
		entity_remove(ID&& entity);

		template<typename TComponent>
		Component<TComponent>
		component_create(const ID& entity_id, const cpprelude::string& name, const TComponent& value)
		{
			auto type = type_utils<TComponent>();
			auto& type_entry = _get_component_set(type);
			auto component_id = type_entry.components.insert(Internal_Component());
			auto& internal_component = type_entry.components[component_id];
			internal_component.id = ID{ component_id, _version_generator++ };
			internal_component.entity = entity_id;
			internal_component.name = name;
			internal_component._type = type;
			internal_component._data = type_entry.allocator->template alloc<TComponent>();
			::new (internal_component._data) TComponent(value);

			Component<TComponent> result;
			result.id = internal_component.id;
			result._data = (TComponent*)internal_component._data;
			result._world = this;
			return result;
		}

		template<typename TComponent>
		void
		component_remove(const ID& component_id)
		{
			auto type = type_utils<TComponent>();
			auto& type_entry = _get_component_set(type);
			auto& component = type_entry.components[component_id.id];
			TComponent *data_ptr = (TComponent*)component._data;
			data_ptr->~TComponent();
			type_entry.allocator->template free<TComponent>(make_slice(data_ptr, sizeof(TComponent)));

			type_entry.components.remove(component_id.id);
		}

		template<typename TComponent>
		bool
		component_change_allocator(cpprelude::memory_context *context)
		{
			auto type = type_utils<TComponent>();
			auto& type_entry = _get_component_set(type);
			if(!type_entry.components.empty())
				return false;
			type_entry.allocator = context;
			return true;
		}

		Component_Type_Entry&
		_get_component_set(Base_Type_Utils* type)
		{
			if(type->id == _components_by_type.count())
				_components_by_type.emplace_back(type, _mem_context);
			return _components_by_type[type->id];
		}
	};
}