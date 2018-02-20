#include "ldgr/world.h"
using namespace cpprelude;

namespace ldgr
{
	Entity_Ledger_Entry::Entity_Ledger_Entry(ID entity_id, memory_context *context)
		:id(entity_id),
		 _components(context)
	{}


	World::World(memory_context *context)
		:_mem_context(context),
		 _id_generator(0),
		 _entities(_mem_context)
	{}

	Entity
	World::create_entity()
	{
		Entity result;
		result.id = _id_generator++;
		result.world = this;
		_entities.insert(result.id, Entity_Ledger_Entry(result.id, _mem_context));
		return result;
	}

	bool
	World::remove_entity(Entity& entity)
	{
		if(!remove_entity(entity.id))
			return false;

		entity.id = INVALID_ID;
		entity.world = nullptr;
		return true;
	}

	bool
	World::remove_entity(Entity&& entity)
	{
		if(!remove_entity(entity.id))
			return false;

		entity.id = INVALID_ID;
		entity.world = nullptr;
		return true;
	}

	bool
	World::remove_entity(const ID& entity_id)
	{
		auto it = _entities.lookup(entity_id);
		if(it == _entities.end())
			return false;

		//free the componenets
		for(auto& component: it->_components)
			_mem_context->template free<byte>(component._data);

		return _entities.remove(it);
	}
}