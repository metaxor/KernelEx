template<typename FI>
FI is_sorted_until(FI begin, FI end)
{
	if (begin == end)
		return end;

	FI next = begin;
	for (++next ; next != end ; begin = next, ++next)
		if (*next < *begin)
			return next;
	
	return end;
}

template<typename FI>
inline bool is_sorted(FI begin, FI end)
{
	return ::is_sorted_until(begin, end) == end;
}
