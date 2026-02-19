//chinykian
//last updated: 29 Oct
//naive implementation of an object pool
#pragma once

#include <list>

template <class T, int N>
class ObjectPool
{
public:
	ObjectPool()
	{
		//make initial pool
		for (int i = 0; i < N; ++i)
			m_freeList.push_back(new T());
	}

	~ObjectPool()
	{
		//free all allocated memory from the heap
		for (T* pObj : m_freeList)
			delete pObj;
		for (T* pObj : m_activeList)
			delete pObj;
	}

	T* GetFromPool()
	{
		if (m_freeList.empty())
		{
			size_t size = m_activeList.size();
			//double pool size
			for (int i = 0; i < size; ++i)
			{
				m_freeList.push_back(new T());
			}
		}

		T* pObj = m_freeList.front();
		m_freeList.pop_front(); //remove from freelist
		m_activeList.push_back(pObj);

		return pObj;
	}

	void ReturnToPool(T* pObj)
	{
		//see if object is actually in active list
		auto it = std::find(m_activeList.begin(), m_activeList.end(), pObj);
		if (it != m_activeList.end())
		{
			m_freeList.push_back(pObj);
			m_activeList.erase(it);
		}
	}

	//NOTE
	//returns a copy of list instead of a reference because functions like ReturnToPool
	//will invalidate iterators
	std::list<T*> GetActiveList()
	{
		return m_activeList;
	}

private:
	std::list<T*> m_freeList;
	std::list<T*> m_activeList;
};
