#pragma once

#include <unordered_map>

enum VarType
{
	Fleet = 0,
	FullEdge = 1,
	Vehicle = 2,
	Task = 3,
};

class VarKey
{
public:
	VarKey(int depot = -1, int id = -1, VarType type = VarType::Vehicle) : depot(depot), id(id), type(type) {}
	int depot;
	int id;
	VarType type;
	bool operator==(const VarKey& right) const { return ((depot == right.depot) && (id == right.id) && (type == right.type)); }
};

class VarHasher
{
public:
	size_t operator()(const VarKey& key) const { return std::hash<int>()(key.depot) ^ std::hash<int>()(key.id) ^ std::hash<int>()(key.type); }
};

using Solution = std::unordered_map<VarKey, double, VarHasher>;

class BoundValue
{
public:
	BoundValue(int lb = 0, int ub = std::numeric_limits<int>::max()) : lb(lb), ub(ub) {}
	int lb;
	int ub;
};

class Bound
{
public:
	Bound(const VarKey& key, const BoundValue& value) : key(key), value(value) {}
	VarKey key;
	BoundValue value;
};

class Bounds
{
public:
	std::vector<Bound> info;
	std::vector<int> edges;
};

class Duals
{
public:
	std::vector<double> vertices;
	std::vector<double> bounds;
};
