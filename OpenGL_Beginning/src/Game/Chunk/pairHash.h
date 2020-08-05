#pragma once
#include <iostream>
#include <string>
struct hash_pair {
	template <class T1, class T2>
	size_t operator()(const std::pair<T1, T2>& p) const
	{
		auto p_first = std::to_string(p.first);
		auto p_second = std::to_string(p.second);

		auto hash = std::hash<std::string>{}(p_first + ":" + p_second);

		return hash;
	}
};
