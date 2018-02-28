#pragma once
#include <cpprelude/dynamic_array.h>
#include <cpprelude/iterator.h>
#include <cpprelude/stack_array.h>
#include <cpprelude/memory_context.h>
#include <cpprelude/platform.h>
#include <cassert>
#include <utility>

namespace ldgr
{
	//specialized for the ledger entity component system
	template<typename T>
	struct Sparse_Set
	{
		//type used for the id generation
		using id_type = T;
		using data_type = id_type;
		using iterator = cpprelude::sequential_iterator<data_type>;
		using const_iterator = cpprelude::sequential_iterator<const data_type>;


		constexpr static id_type INVALID_ID = static_cast<id_type>(-1);

		//stores the ID
		//since the stored value here is the id itself then this is also the dense to sparse array
		cpprelude::dynamic_array<id_type> _dense;

		//stores the position type of the corresponding id
		cpprelude::dynamic_array<id_type> _sparse_to_dense;

		//free slots in the sparse to dense array
		cpprelude::stack_array<id_type> _free_slots;

		Sparse_Set(cpprelude::memory_context *context = cpprelude::platform->global_memory)
			:_dense(context),
			 _sparse_to_dense(context),
			 _free_slots(context)
		{}

		cpprelude::usize
		count() const
		{
			return _dense.count();
		}

		cpprelude::usize
		capacity() const
		{
			return _dense.capacity();
		}

		bool
		empty() const
		{
			return _dense.empty();
		}

		void
		reserve(cpprelude::usize expected_size)
		{
			_dense.reserve(expected_size);
			if(_free_slots.count() < expected_size)
				_sparse_to_dense.reserve(expected_size - _free_slots.count());
		}

		bool
		exists(id_type id) const
		{
			return _sparse_to_dense[id] != INVALID_ID;
		}

		id_type&
		operator[](id_type index)
		{
			assert(exists(index));
			return _dense[_sparse_to_dense[index]];
		}

		const id_type&
		operator[](id_type index) const
		{
			assert(exists(index));
			return _dense[_sparse_to_dense[index]];
		}

		id_type
		insert()
		{
			//calculate the indices
			id_type sparse_dense_index = INVALID_ID;
			id_type dense_index = _dense.count();

			//reuse the existing slots
			if(!_free_slots.empty())
			{
				sparse_dense_index = _free_slots.top();
				_free_slots.pop();
				_sparse_to_dense[sparse_dense_index] = dense_index;
			}
			//just expand the sparse indices
			else
			{
				sparse_dense_index = _sparse_to_dense.count();
				_sparse_to_dense.insert_back(dense_index);
			}

			_dense.insert_back(sparse_dense_index);
			return sparse_dense_index;
		}

		void
		remove(id_type id)
		{
			assert(exists(id));

			id_type remove_sparse_index = id;
			id_type remove_dense_index = _sparse_to_dense[remove_sparse_index];
			id_type last_dense_index = _dense.count() - 1;
			//since dense is where we store the ids then that's it no dense to sparse array
			id_type last_sparse_index = _dense[last_dense_index];

			//if the value we need to remove is not the last one then
			//swap the removed value and last value
			if(remove_dense_index != last_dense_index)
			{
				//swap the removed value and the last value
				std::swap(_dense[remove_dense_index], _dense[last_dense_index]);

				//adjust the sparse to dense index of the last object to point to the new location
				_sparse_to_dense[last_sparse_index] = remove_dense_index;
			}

			//just remove the id from the back
			_dense.remove_back();

			//mark the removed slot and add to the empty slots
			_sparse_to_dense[remove_sparse_index] = INVALID_ID;
			_free_slots.push(remove_sparse_index);
		}

		const_iterator
		front() const
		{
			return _dense.front();
		}

		iterator
		front()
		{
			return _dense.front();
		}

		const_iterator
		back() const
		{
			return _dense.back();
		}

		iterator
		back()
		{
			return _dense.back();
		}

		const_iterator
		begin() const
		{
			return _dense.cbegin();
		}

		const_iterator
		cbegin() const
		{
			return _dense.cbegin();
		}

		iterator
		begin()
		{
			return _dense.begin();
		}

		const_iterator
		cend() const
		{
			return _dense.cend();
		}

		const_iterator
		end() const
		{
			return _dense.cend();
		}

		iterator
		end()
		{
			return _dense.end();
		}
	};

	template<typename TID, typename TValue>
	struct Value_Sparse_Set: Sparse_Set<TID>
	{
		using _implementation = Sparse_Set<TID>;

		using id_type = TID;
		using value_type = TValue;

		cpprelude::dynamic_array<value_type> _values;

		Value_Sparse_Set(cpprelude::memory_context* context = cpprelude::platform->global_memory)
			:_implementation(context),
			 _values(context)
		{}

		void
		reserve(cpprelude::usize expected_size)
		{
			_implementation::reserve(expected_size);
			_values.reserve(expected_size);
		}

		value_type&
		operator[](id_type index)
		{
			assert(exists(index));
			return _values[_sparse_to_dense[index]];
		}

		const value_type&
		operator[](id_type index) const
		{
			assert(exists(index));
			return _values[_sparse_to_dense[index]];
		}

		id_type
		insert(const value_type& value)
		{
			_values.insert_back(value);
			return _implementation::insert();
		}

		id_type
		insert(value_type&& value)
		{
			_values.insert_back(std::move(value));
			return _implementation::insert();
		}

		void
		remove(id_type id)
		{
			std::swap(_values[_sparse_to_dense[id]], *_values.back());
			_values.remove_back();
			_implementation::remove(id);
		}
	};
}