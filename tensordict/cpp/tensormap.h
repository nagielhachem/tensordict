#ifndef TensorMap_h
#define TensorMap_h

#include <memory>
#include <torch/extension.h>
#include <unordered_map>
#include <set>
#include <variant>
#include <string>
#include <iostream>
#include <vector>

namespace py = pybind11;

class TensorMap {
    typedef std::variant<torch::Tensor, TensorMap> node;
    typedef std::unordered_map<std::string, node> map;
    typedef std::variant<std::string, py::tuple> key;
    private:
        std::shared_ptr<map> internalMap;
        std::vector<int64_t> batchSize; // TODO something about batch size

    public:
        TensorMap(std::vector<int64_t> batchSize);
        ~TensorMap() = default;
        TensorMap(const TensorMap&) = default;
        TensorMap(TensorMap&&) = default;

        // Index Single Point
        node GetAt(const std::string key) const;
        void SetTensorAt(const std::string key, const torch::Tensor& value);
        void SetMapAt(const std::string key, const TensorMap& value);
        // Index Path
        node GetAtPath(const py::tuple key);
        void SetTensorAtPath(const py::tuple key, const torch::Tensor& value);
        void SetMapAtPath(const py::tuple key, const TensorMap& value);
        // TODO add keys - check iterator
        std::set<key> GetKeys(const bool includeNested = false, const bool leavesOnly = false);

        TensorMap& operator=(const TensorMap& other) & {
           internalMap = other.internalMap;
           batchSize = other.batchSize;
           return *this;
        }
        TensorMap& operator=(TensorMap& other) & {
            internalMap = std::move(other.internalMap);
            batchSize = other.batchSize;
            return *this;
        }

        bool is_same(const TensorMap& other) const noexcept {
            return this->internalMap == other.internalMap;
        }

        bool operator==(const TensorMap& other) const {
            return this->internalMap == other.internalMap;
        }

        bool operator!=(const TensorMap& other) const {
            return this->internalMap != other.internalMap;
        }

        // Helpers
        private:
            map* unsafeGetInternalMap() const;
            node GetRecursive(TensorMap* currentMap, const py::tuple indices, const int index);
            void SetRecursive(TensorMap* currentMap, const py::tuple  indices, const int index, node value);
            void GetKeysRecursiveAll(std::set<key>& result, py::tuple currentPath, const node& currentNode);
            void GetKeysRecursiveLeavesOnly(std::set<key>& result, py::tuple currentPath, const node& currentNode);
            void GetKeysFirstLevel(std::set<key>& result, bool leavesOnly);
            key GetCleanKey(py::tuple path);
            void ValidateBatchSize(const c10::IntArrayRef shape);

            bool Contains(std::string key) const;
            bool HoldsMap(std::string key) const;

};

#endif
