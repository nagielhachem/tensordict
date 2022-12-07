#include "tensormap.h"
#include <exception>
#include <string>
#include <torch/torch.h>
#include <variant>
#include <vector>
#include <string>
#include <map>

namespace tensordict {

void TensorMap::set(std::string key, torch::Tensor value)
{
    this->map[key] = value;
}

void TensorMap::set(std::string key, TensorMap value)
{
    this->map[key] = value;
}

void TensorMap::set(std::vector<std::string> indices, torch::Tensor value)
{
    if (indices.size() == 0)
        throw std::invalid_argument("indices must have at least one element");

    auto lastMap = GetRecursive(this->map, indices, 0);
    auto key = indices[indices.size() - 1];

    lastMap->at(key) = value;
}

void TensorMap::set(std::vector<std::string> indices, TensorMap value)
{
    if (indices.size() == 0)
        throw std::invalid_argument("indices must have at least one element");

    auto lastMap = GetRecursive(this->map, indices, 0);
    auto key = indices[indices.size() - 1];

    lastMap->at(key) = value;
}

std::variant<torch::Tensor, TensorMap> TensorMap::get(std::string key)
{
    if (!this->map.contains(key))
        throw std::invalid_argument("Invalid key: " + key);

    return this->map[key];
}

std::variant<torch::Tensor, TensorMap> TensorMap::get(std::vector<std::string> indices)
{
    if (indices.size() == 0)
        throw std::invalid_argument("indices must have at least one element");

    auto lastMap = GetRecursive(this->map, indices, 0);
    auto key = indices[indices.size() - 1];

    return lastMap->at(key);
}

std::map<std::string, std::variant<torch::Tensor, TensorMap>>* TensorMap::GetRecursive(
    std::map<std::string, std::variant<torch::Tensor, TensorMap>>& map,
    std::vector<std::string>& indices,
    int index)
{
    auto key = indices[index];
    if (!map.contains(key))
        throw std::invalid_argument("Invalid key " + key + " at index: " + std::to_string(index));

    if (index == indices.size() - 1)
    {
        return &map;
    }

    if (std::holds_alternative<TensorMap>(map[key]))
    {
        auto currentMap = std::get<TensorMap>(map[key]);
        return GetRecursive(currentMap.map, indices, index + 1);
    }
    else
        throw std::invalid_argument("Expected to have a Map at index " + std::to_string(index) + " but found tensor");
}
}
