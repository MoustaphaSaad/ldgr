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
	template<typename T>
	struct Bag
	{
		using iterator = cpprelude::sequential_iterator<T>;
		using const_iterator = cpprelude::sequential_iterator<const T>;
		using data_type = T;

		constexpr static cpprelude::usize INVALID_ID = static_cast<cpprelude::usize>(-1);

		//this array contains the objects themselves
		cpprelude::dynamic_array<T> _dense;
		//this array converts from dense index to sparse index
		//this array will always be as large as the dense array
		cpprelude::dynamic_array<cpprelude::usize> _dense_to_sparse;

		//this array converts from the sparse index to dense index
		//this array size will be the peak count of the bag
		cpprelude::dynamic_array<cpprelude::usize> _sparse_to_dense;
		//free slots in the sparse to dense array
		cpprelude::stack_array<cpprelude::usize> _free_slots;

		Bag(cpprelude::memory_context *context = cpprelude::platform->global_memory)
			:_dense(context),
			 _dense_to_sparse(context),
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
			return count() == 0;
		}

		void
		reserve(cpprelude::usize expected_size)
		{
			_dense.reserve(expected_size);
			_dense_to_sparse.reserve(expected_size);
		}

		bool
		has(cpprelude::usize id) const
		{
			if (id >= _sparse_to_dense.count()) return false;
			return _sparse_to_dense[id] != INVALID_ID;
		}

		T&
		operator[](cpprelude::usize index)
		{
			assert(has(index));
			return _dense[_sparse_to_dense[index]];
		}

		const T&
		operator[](cpprelude::usize index) const
		{
			assert(has(index));
			return _dense[_sparse_to_dense[index]];
		}

		template<typename ... TArgs>
		cpprelude::usize
		emplace(TArgs&& ... args)
		{
			//calculate the indices
			cpprelude::usize sparse_dense_index = INVALID_ID;
			cpprelude::usize dense_index = _dense.count();

			//check if there's free slots then reuse them
			if(!_free_slots.empty())
			{
				sparse_dense_index = _free_slots.top();
				_free_slots.pop();
				_sparse_to_dense[sparse_dense_index] = dense_index;
			}
			//just expand the sparse array
			else
			{
				sparse_dense_index = _sparse_to_dense.count();
				_sparse_to_dense.insert_back(dense_index);
			}

			//insert back the value and the dense to sparse index
			_dense_to_sparse.insert_back(sparse_dense_index);
			_dense.emplace_back(std::forward<TArgs>(value)...);
			return sparse_dense_index;
		}

		cpprelude::usize
		insert(const T& value)
		{
			//calculate the indices
			cpprelude::usize sparse_dense_index = INVALID_ID;
			cpprelude::usize dense_index = _dense.count();

			//check if there's free slots then reuse them
			if(!_free_slots.empty())
			{
				sparse_dense_index = _free_slots.top();
				_free_slots.pop();
				_sparse_to_dense[sparse_dense_index] = dense_index;
			}
			//just expand the sparse array
			else
			{
				sparse_dense_index = _sparse_to_dense.count();
				_sparse_to_dense.insert_back(dense_index);
			}

			//insert back the value and the dense to sparse index
			_dense_to_sparse.insert_back(sparse_dense_index);
			_dense.insert_back(value);
			return sparse_dense_index;
		}

		cpprelude::usize
		insert(T&& value)
		{
			//calculate the indices
			cpprelude::usize sparse_dense_index = INVALID_ID;
			cpprelude::usize dense_index = _dense.count();

			//check if there's free slots then reuse them
			if(!_free_slots.empty())
			{
				sparse_dense_index = _free_slots.top();
				_free_slots.pop();
				_sparse_to_dense[sparse_dense_index] = dense_index;
			}
			//just expand the sparse array
			else
			{
				sparse_dense_index = _sparse_to_dense.count();
				_sparse_to_dense.insert_back(dense_index);
			}

			//insert back the value and the dense to sparse index
			_dense_to_sparse.insert_back(sparse_dense_index);
			_dense.insert_back(std::move(value));
			return sparse_dense_index;
		}

		bool
		remove(cpprelude::usize id)
		{
			if(!has(id))
				return false;

			cpprelude::usize remove_sparse_index = id;
			cpprelude::usize remove_dense_index = _sparse_to_dense[remove_sparse_index];
			cpprelude::usize last_dense_index = _dense.count() - 1;
			cpprelude::usize last_sparse_index = _dense_to_sparse[last_dense_index];

			//if the value we need to remove is not the last one then
			//swap the removed value and last value
			if (remove_dense_index != last_dense_index)
			{
				//swap the removed value and the last value
				std::swap(_dense[remove_dense_index], _dense[last_dense_index]);

				//adjust the sparse to dense index of the last object to point to the new location
				_sparse_to_dense[last_sparse_index] = remove_dense_index;
				//adjust the dense to sparse index of the removed object to point to the last object sparse slot
				_dense_to_sparse[remove_dense_index] = last_sparse_index;
			}

			//just remove them from the back
			_dense.remove_back();
			_dense_to_sparse.remove_back();

			//mark the removed slot and add it to the empty slots
			_sparse_to_dense[remove_sparse_index] = INVALID_ID;
			_free_slots.push(remove_sparse_index);
			return true;
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


	//special bag for IDs
	struct ID_Bag
	{
		using iterator = cpprelude::sequential_iterator<cpprelude::usize>;
		using const_iterator = cpprelude::sequential_iterator<const cpprelude::usize>;
		using data_type = cpprelude::usize;

		constexpr static cpprelude::usize INVALID_ID = static_cast<cpprelude::usize>(-1);

		//this array contains the objects themselves
		//since the stored value here is the id itself then this is also the dense to sparse array
		cpprelude::dynamic_array<cpprelude::usize> _dense;

		//this array converts from the sparse index to dense index
		//this array size will be the peak count of the bag
		cpprelude::dynamic_array<cpprelude::usize> _sparse_to_dense;
		//free slots in the sparse to dense array
		cpprelude::stack_array<cpprelude::usize> _free_slots;

		ID_Bag(cpprelude::memory_context *context = cpprelude::platform->global_memory)
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
			return count() == 0;
		}

		void
		reserve(cpprelude::usize expected_size)
		{
			_dense.reserve(expected_size);
			_sparse_to_dense.reserve(expected_size);
		}

		bool
		has(cpprelude::usize id) const
		{
			return _sparse_to_dense[id] != INVALID_ID;
		}

		cpprelude::usize&
		operator[](cpprelude::usize index)
		{
			assert(has(index));
			return _dense[_sparse_to_dense[index]];
		}

		const cpprelude::usize&
		operator[](cpprelude::usize index) const
		{
			assert(has(index));
			return _dense[_sparse_to_dense[index]];
		}

		cpprelude::usize
		insert()
		{
			//calculate the indices
			cpprelude::usize sparse_dense_index = INVALID_ID;
			cpprelude::usize dense_index = _dense.count();

			//check if there's free slots then reuse them
			if(!_free_slots.empty())
			{
				sparse_dense_index = _free_slots.top();
				_free_slots.pop();
				_sparse_to_dense[sparse_dense_index] = dense_index;
			}
			//just expand the sparse array
			else
			{
				sparse_dense_index = _sparse_to_dense.count();
				_sparse_to_dense.insert_back(dense_index);
			}

			_dense.insert_back(sparse_dense_index);
			return sparse_dense_index;
		}

		bool
		remove(cpprelude::usize id)
		{
			if(!has(id))
				return false;

			cpprelude::usize remove_sparse_index = id;
			cpprelude::usize remove_dense_index = _sparse_to_dense[remove_sparse_index];
			cpprelude::usize last_dense_index = _dense.count() - 1;
			//since dense is where we store the ids then that's it no dense to sparse array
			cpprelude::usize last_sparse_index = _dense[last_dense_index];

			//if the value we need to remove is not the last one then
			//swap the removed value and last value
			if (remove_dense_index != last_dense_index)
			{
				//swap the removed value and the last value
				std::swap(_dense[remove_dense_index], _dense[last_dense_index]);

				//adjust the sparse to dense index of the last object to point to the new location
				_sparse_to_dense[last_sparse_index] = remove_dense_index;
			}

			//just remove them from the back
			_dense.remove_back();

			//mark the removed slot and add it to the empty slots
			_sparse_to_dense[remove_sparse_index] = INVALID_ID;
			_free_slots.push(remove_sparse_index);
			return true;
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
}