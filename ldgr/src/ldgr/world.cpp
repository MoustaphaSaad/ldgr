#include "ldgr/world.h"
using namespace cpprelude;

namespace ldgr
{
	World::World(memory_context *context)
		:_mem_context(context),
		 _version_generator(0),
		 _entities(context),
		 _components_by_type(context)
	{}

	ID
	World::entity_create()
	{
		ID entity;
		entity.id = _entities.insert();
		entity.version = _version_generator++;
		return entity;
	}

	void
	World::entity_reserve(cpprelude::usize expected_size)
	{
		_entities.reserve(expected_size);
	}

	void
	World::entity_remove(ID& entity)
	{
		_entities.remove(entity.id);
		entity.id = -1;
		entity.version = -1;
	}

	void
	World::entity_remove(ID&& entity)
	{
		entity_remove(entity);
	}
}