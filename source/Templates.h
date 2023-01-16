#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <utility>
#include <functional>
#include <vector>

template<class T> struct Greater
{
	bool operator()(T left, T right) const { return (*left > *right); }
};

template<class T> struct Less
{
	bool operator()(T left, T right) const { return (*left < *right); }
};

template<class T1, class T2> struct FirstGreater
{
	bool operator()(std::pair<T1, T2> left, std::pair<T1, T2> right) { return (left.first > right.first); }
};

struct PairHasher
{
	template <typename T, typename U>
	std::size_t operator()(const std::pair<T, U> &x) const
	{
		return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
	}
};

template<typename T>
std::vector<T> operator+(const std::vector<T>& left, const std::vector<T>& right)
{
	std::vector<T> ret;
	if (left.size() != right.size()) throw;
	for (int i = 0; i < (int)left.size(); i++) ret.push_back(left[i] + right[i]);
	return ret;
};

template<typename T>
std::vector<std::vector<T>> operator+(const std::vector<std::vector<T>>& left, const std::vector<std::vector<T>>& right)
{
	std::vector<std::vector<T>> ret;
	if (left.size() != right.size()) throw;
	for (int i = 0; i < (int)left.size(); i++) ret.push_back(left[i] + right[i]);
	return ret;
}

template<typename T>
std::vector<T> operator*(const std::vector<T>& left, const T& right)
{
	std::vector<T> ret;
	for (int i = 0; i < (int)left.size(); i++) ret.push_back(left[i] * right);
	return ret;
};

template<typename T>
std::vector<std::vector<T>> operator*(const std::vector<std::vector<T>>& left, const T& right)
{
	std::vector<std::vector<T>> ret;
	for (int i = 0; i < (int)left.size(); i++) ret.push_back(left[i] * right);
	return ret;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& right)
{
	for (int i = 0; i < (int)right.size(); i++)
	{
		if (i != 0) out << " ";
		out << right[i];
	}
	return out;
}

#endif
