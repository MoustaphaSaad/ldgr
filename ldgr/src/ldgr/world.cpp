#include "ldgr/world.h"
#include <cpprelude/error.h>
using namespace cpprelude;

namespace ldgr
{
	Entity_Ledger_Entry::Entity_Ledger_Entry(ID entity_id, memory_context *context)
		:id(entity_id),
		 _components(context)
	{}


	World::World(memory_context *context)
		:_mem_context(context),
		 _entities_bag(_mem_context)
	{}

	Entity
	World::create_entity()
	{
		cpprelude::usize id = _entities_bag.insert(Entity());
		Entity& entity = _entities_bag[id];
		entity.id = id;
		entity.world = this;
		return entity;
	}

	bool
	World::remove_entity(Entity& entity)
	{
		bool result = _entities_bag.remove(entity.id);
		entity.id = INVALID_ID;
		entity.world = nullptr;
		return result;
	}

	bool
	World::remove_entity(Entity&& entity)
	{
		return remove_entity(entity);
	}

	bool
	World::remove_entity(ID entity_id)
	{
		return _entities_bag.remove(entity_id);
	}

	bool
	World::entity_exists(const Entity& entity) const
	{
		return entity_exists(entity.id);
	}

	bool
	World::entity_exists(ID entity_id) const
	{
		return _entities_bag.has(entity_id);
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