#include <windows.h>
#include "debug.h"

template <class T>
class winheap_allocator
{
public:
	typedef T                  value_type;
	typedef value_type*        pointer;
	typedef const value_type*  const_pointer;
	typedef value_type&        reference;
	typedef const value_type&  const_reference;
	typedef size_t             size_type;
	typedef ptrdiff_t          difference_type;
	
	template <class U> 
	struct rebind { typedef winheap_allocator<U> other; };

	winheap_allocator(HANDLE heap)
	{
		this->heap = heap;
	}
	
//	winheap_allocator(const winheap_allocator& a)
//	{
//		heap = a.heap;
//	} 

	template <class U> 
	winheap_allocator(const winheap_allocator<U>& a)
	{
		heap = a.heap;
	}

	~winheap_allocator()
	{
	}

	pointer address(reference x) const
	{
		return &x;
	}
	
	const_pointer address(const_reference x) const
	{
		return x;
	}

	pointer allocate(size_type n, const_pointer = 0)
	{
		void* p = HeapAlloc(heap, 0, n * sizeof(T));
//		if (!p)
//			throw std::bad_alloc();
		return static_cast<pointer>(p);
	}

	char* _Charalloc(size_type n)
	{
		return rebind<char>::other(*this).allocate(n, (char*) 0);
	}

	void deallocate(void* p, size_type)
	{
		BOOL ret = HeapFree(heap, 0, p);
		DBGASSERT(ret == TRUE);
	}

	size_type max_size() const 
	{
		return static_cast<size_type>(-1) / sizeof(T);
	}

	void construct(pointer p, const value_type& x)
	{
		new (p) value_type(x); 
	}
	
	void destroy(pointer p)
	{
		p->~value_type();
	}

	HANDLE heap;

private:
	void operator=(const winheap_allocator&);
};

template<>
class winheap_allocator<void>
{
	typedef void        value_type;
	typedef void*       pointer;
	typedef const void* const_pointer;

	template <class U> 
	struct rebind { typedef winheap_allocator<U> other; };
};


template <class T>
inline bool operator==(const winheap_allocator<T>& a, 
                         const winheap_allocator<T>& b)
{
	return a.heap == b.heap;
}

template <class T>
inline bool operator!=(const winheap_allocator<T>& a, 
                         const winheap_allocator<T>& b)
{
	return a.heap != b.heap;
}
