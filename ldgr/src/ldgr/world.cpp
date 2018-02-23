#include "ldgr/world.h"
#include <cpprelude/error.h>
using namespace cpprelude;

namespace ldgr
{
	Entity_Ledger_Entry::Entity_Ledger_Entry(ID entity_id, memory_context *context)
		:id(entity_id),
		 _components_ids(context),
		 _components_by_name(context)
	{}

	Component_Type_Ledger_Entry::Component_Type_Ledger_Entry(Base_Type_Utils* type,
		memory_context* array_context,
		memory_context* component_context)
		:_component_context(component_context),
		 _type_utils(type),
		 _components(array_context)
	{}


	World::World(memory_context *context)
		:_mem_context(context),
		 _entities_bag(_mem_context),
		 _components_bag(_mem_context),
		 _entities_ldgr(_mem_context),
		 _components_by_type(_mem_context)
	{}

	//Entites Functions
	Entity
	World::create_entity()
	{
		return Entity{ _entities_bag.insert(), this };
	}

	bool
	World::remove_entity(ID entity_id)
	{
		if(!entity_exists(entity_id))
			return false;

		//remove all components inside this entity
		auto entity_ledger_it = _entities_ldgr.lookup(entity_id);
		if(entity_ledger_it != _entities_ldgr.end())
		{
			for (auto component_id : entity_ledger_it->_components_ids)
			{
				_destroy_component(component_id);
				_components_bag.remove(component_id);
			}

			//remove the ledger entry
			_entities_ldgr.remove(entity_ledger_it);
		}
		//remove from the bag
		return _entities_bag.remove(entity_id);
	}

	bool
	World::entity_exists(ID entity_id) const
	{
		return _entities_bag.has(entity_id);
	}

	void
	World::reserve_entites(usize expected_size)
	{
		_entities_bag.reserve(expected_size);
		_entities_ldgr.reserve(expected_size);
	}

	//Components funcitons
	bool
	World::has_component(ID entity_id, const string& name)
	{
		if(!entity_exists(entity_id))
			return false;

		auto entity_ledger_it = _entities_ldgr.lookup(entity_id);
		if (entity_ledger_it == _entities_ldgr.end())
			return false;

		return (entity_ledger_it->_components_by_name.lookup(name) !=
				entity_ledger_it->_components_by_name.end());
	}

	bool
	World::has_component(ID entity_id, ID component_id)
	{
		if(!_components_bag.has(component_id))
			return false;
		return _components_bag[component_id].entity.id == entity_id;
	}

	bool
	World::remove_component(ID component_id)
	{
		if(!_components_bag.has(component_id))
			return false;

		auto& component = _components_bag[component_id];

		//detach from the entity ledger entity
		{
			auto& entity_ledger_entry = _entities_ldgr[component.entity.id];
			entity_ledger_entry._components_ids.remove(component.id);
			entity_ledger_entry._components_by_name.remove(component.name);
		}

		//destruct and free the component
		{
			auto& component_type_ledger_entry = _components_by_type[component._type_utils];
			component._type_utils->invoke_destructor(component._data);
			component_type_ledger_entry._components.remove(component_id);
			component_type_ledger_entry._component_context->free(component._data);
		}

		return _components_bag.remove(component_id);
	}

	void
	World::_destroy_component(ID component_id)
	{
		auto& component = _components_bag[component_id];
		auto& component_type_ledger_entry = _components_by_type[component._type_utils];
		component._type_utils->invoke_destructor(component._data);
		component_type_ledger_entry._components.remove(component_id);
		component_type_ledger_entry._component_context->free(component._data);
	}

	// dynamic_array<Base_Component>&
	// World::get_all_components(const Entity& entity)
	// {
	// 	return get_all_components(entity.id);
	// }

	// const dynamic_array<Base_Component>&
	// World::get_all_components(const Entity& entity) const
	// {
	// 	return get_all_components(entity.id);
	// }

	// dynamic_array<Base_Component>&
	// World::get_all_components(ID entity_id)
	// {
	// 	check(entity_exists(entity_id), "Can't get the components of an entity that does not exist"_cs);
	// 	return _entities[entity_id]._components;
	// }

	// const dynamic_array<Base_Component>&
	// World::get_all_components(ID entity_id) const
	// {
	// 	check(entity_exists(entity_id), "Can't get the components of an entity that does not exist"_cs);
	// 	//add const operator []
	// 	return _entities.lookup(entity_id)->_components;
	// }
}