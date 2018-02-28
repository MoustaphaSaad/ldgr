#include "benchmark.h"
#include <cpprelude/micro_benchmark.h>
#include <cpprelude/dynamic_array.h>
#include <cpprelude/allocator.h>
#include <cpprelude/fmt.h>
#include "ldgr/world.h"
#include <entt/entt.hpp>
using namespace cpprelude;
using namespace ldgr;


arena_t arena(GIGABYTES(1));

struct Position
{
	r32 x, y, z;
};

usize
bm_entites(workbench *bench, usize limit)
{
	usize result = 0;
	usize offset = rand();

	cpprelude::dynamic_array<ID> entities(limit);
	World world;

	bench->watch.start();
		for(usize i = 0; i < limit; ++i)
			entities[i] = world.entity_create();

		for (usize i = 0; i < limit; ++i)
		{
			if ((i + offset) % 2 == 0)
			{
				world.entity_remove(entities[i]);
				++result;
			}
		}
	bench->watch.stop();
	return result;
}

usize
bm_entt_entities(workbench *bench, usize limit)
{
	usize result = 0;
	usize offset = rand();

	cpprelude::dynamic_array<u64> entities(limit);
	entt::Registry<u64> world;

	bench->watch.start();
	for (usize i = 0; i < limit; ++i)
		entities[i] = world.create();

	for (usize i = 0; i < limit; ++i)
	{
		if ((i + offset) % 2 == 0)
		{
			world.destroy(entities[i]);
			++result;
		}
	}
	bench->watch.stop();
	return result;
}

usize
bm_components(workbench *bench, usize limit)
{
	usize result = 0;
	usize offset = rand();
	arena.free_all();
	cpprelude::dynamic_array<Component<Position>> components(limit);
	World world(arena);
	bench->watch.start();
		world.component_change_allocator<Position>(arena);
		for(usize i = 0; i < limit; ++i)
		{
			auto entity = world.entity_create();
			components[i] = world.component_create<Position>(entity, "position"_cs, Position{ 1.0f, 1.0f, 1.0f });
		}
		for (usize i = 0; i < limit; ++i)
		{
			if ((i + offset) % 2 == 0)
			{
				world.component_remove<Position>(components[i].id);
				++result;
			}
		}
	bench->watch.stop();
	return result;
}

usize
bm_entt_components(workbench *bench, usize limit)
{
	usize result = 0;
	usize offset = rand();

	cpprelude::dynamic_array<u64> entities(limit);
	entt::Registry<u64> world;

	bench->watch.start();
		for (usize i = 0; i < limit; ++i)
		{
			entities[i] = world.create();
			world.assign<Position>(entities[i], 1.0f, 1.0f, 1.0f);
		}

		for (usize i = 0; i < limit; ++i)
		{
			if ((i + offset) % 2 == 0)
			{
				world.remove<Position>(entities[i]);
				++result;
			}
		}
	bench->watch.stop();
	return result;
}

// usize
// bm_update(workbench *bench, usize limit)
// {
// 	usize result = 0;
// 	usize offset = rand();

// 	cpprelude::dynamic_array<VID> components(limit);
// 	World world;

// 	bench->watch.start();
// 	world.change_type_allocator<Position>(arena);
// 	for (usize i = 0; i < limit; ++i)
// 	{
// 		auto entity = world.create_entity();
// 		components[i] = world.create_component<Position>(entity);
// 	}

// 	for (auto& component : world._components._values)
// 	{
// 		Position* position = (Position*)component._data;
// 		position->x = offset;
// 		position->y = offset;
// 		position->z = offset;
// 		++result;
// 	}
// 	bench->watch.stop();
// 	return result;
// }

// usize
// bm_entt_update(workbench *bench, usize limit)
// {
// 	usize result = 0;
// 	usize offset = rand();

// 	cpprelude::dynamic_array<u64> entities(limit);
// 	entt::Registry<u64> world;

// 	bench->watch.start();
// 	for (usize i = 0; i < limit; ++i)
// 	{
// 		entities[i] = world.create();
// 		world.assign<Position>(entities[i], 1.0f, 1.0f, 1.0f);
// 	}

// 	world.view<Position>().each([&offset, &result](u64 entity, Position& position) {
// 		position.x = offset;
// 		position.y = offset;
// 		position.z = offset;
// 		++result;
// 	});
// 	bench->watch.stop();
// 	return result;
// }

void
benchmark()
{
	srand(time(0));
	usize limit = 100000;
	println("limit: ", limit);

	compare_benchmark(std::cout, {
		CPPRELUDE_BENCHMARK(bm_entt_entities, limit),
		CPPRELUDE_BENCHMARK(bm_entites, limit)
	});

	compare_benchmark(std::cout, {
		CPPRELUDE_BENCHMARK(bm_entt_components, limit),
		CPPRELUDE_BENCHMARK(bm_components, limit)
	});

	// compare_benchmark(std::cout, {
	// 	CPPRELUDE_BENCHMARK(bm_entt_update, limit),
	// 	CPPRELUDE_BENCHMARK(bm_update, limit)
	// });
}