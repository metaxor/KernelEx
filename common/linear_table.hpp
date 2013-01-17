#ifndef __LINEAR_TABLE_HPP
#define __LINEAR_TABLE_HPP

#include <algorithm>

template<typename _Key, typename _Value, int _N>
class linear_table
{
public:
	linear_table()
	{
		m_count = 0;
	}

	void free(const _Key& _k)
	{
		for (int i = 0 ; i < m_count ; i++)
			if (m_data[i].k == _k)
			{
				std::copy(m_data + i + 1, m_data + m_count, m_data + i);
				--m_count;
				break;
			}
	}

	_Value get(const _Key& _k)
	{
		for (int i = 0 ; i < m_count ; i++)
			if (m_data[i].k == _k)
				return m_data[i].v;
		return 0;
	}

	bool set(const _Key& _k, const _Value& _v)
	{
		for (int i = 0 ; i < m_count ; i++)
			if (m_data[i].k == _k)
			{
				m_data[i].v = _v;
				return true;
			}
		// alloc if there's no such key in table
		if (!alloc(_k))
			return false;
		m_data[get_count() - 1].v = _v;
		return true;
	}

	int get_count()
	{
		return m_count;
	}

protected:
	bool alloc(const _Key& _k)
	{
		if (m_count == _N)
			return false;
		m_data[m_count].k = _k;
		m_data[m_count].v = 0;
		++m_count;
		return true;
	}

private:
	int m_count;
	struct
	{
		_Key k;
		_Value v;
	} m_data[_N];
};

#endif
