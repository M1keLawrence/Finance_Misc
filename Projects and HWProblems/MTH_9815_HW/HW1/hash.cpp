#pragma once
#include <vector>
#include <list>
#include <functional>
#include <memory>
#include <stdexcept>

// Base class for hashing function
template <typename K>
class Hasher {
public:
    virtual ~Hasher() = default;
    virtual size_t hash(const K& key) const = 0;
    virtual Hasher* clone() const = 0;
};

// Base class for equality predicate
template <typename K>
class EqualityPredicate {
public:
    virtual ~EqualityPredicate() = default;
    virtual bool operator()(const K& a, const K& b) const = 0;
    virtual EqualityPredicate* clone() const = 0;
};

// Default hasher implementation using std::hash
template <typename K>
class DefaultHasher : public Hasher<K> {
public:
    size_t hash(const K& key) const override {
        return std::hash<K>{}(key);
    }

    DefaultHasher* clone() const override {
        return new DefaultHasher<K>();
    }
};

// Default equality predicate implementation
template <typename K>
class DefaultEquality : public EqualityPredicate<K> {
public:
    bool operator()(const K& a, const K& b) const override {
        return a == b;
    }

    DefaultEquality* clone() const override {
        return new DefaultEquality<K>();
    }
};

// Hash table class
template <typename K, typename V>
class Hashtable {
private:
    struct KeyValuePair {
        K key;
        V value;
        KeyValuePair(const K& k, const V& v) : key(k), value(v) {}
    };

    std::vector<std::list<KeyValuePair>> buckets;
    Hasher<K>* hasher;
    EqualityPredicate<K>* equality;
    size_t elementCount;
    double loadFactorThreshold;
    bool ownsHasher;
    bool ownsEquality;

    void rehash() {
        std::vector<std::list<KeyValuePair>> oldBuckets = std::move(buckets);
        buckets.resize(oldBuckets.size() * 2);
        elementCount = 0;

        for (const auto& bucket : oldBuckets) {
            for (const auto& pair : bucket) {
                insert(pair.key, pair.value);
            }
        }
    }

    size_t getBucketIndex(const K& key) const {
        return hasher->hash(key) % buckets.size();
    }

    void cleanup() {
        if (ownsHasher && hasher) {
            delete hasher;
            hasher = nullptr;
        }
        if (ownsEquality && equality) {
            delete equality;
            equality = nullptr;
        }
    }

public:
    Hashtable(size_t initialSize = 16,
        Hasher<K>* hasherObj = nullptr,
        EqualityPredicate<K>* equalityObj = nullptr)
        : buckets(initialSize), elementCount(0), loadFactorThreshold(0.75) {

        if (hasherObj) {
            hasher = hasherObj;
            ownsHasher = false;
        }
        else {
            hasher = new DefaultHasher<K>();
            ownsHasher = true;
        }

        if (equalityObj) {
            equality = equalityObj;
            ownsEquality = false;
        }
        else {
            equality = new DefaultEquality<K>();
            ownsEquality = true;
        }
    }

    Hashtable(const Hashtable& other)
        : buckets(other.buckets.size()),
        elementCount(other.elementCount),
        loadFactorThreshold(other.loadFactorThreshold),
        ownsHasher(true),
        ownsEquality(true) {

        hasher = other.hasher->clone();
        equality = other.equality->clone();

        for (size_t i = 0; i < other.buckets.size(); ++i) {
            buckets[i] = other.buckets[i];
        }
    }

    Hashtable& operator=(const Hashtable& other) {
        if (this != &other) {
            cleanup();

            buckets.resize(other.buckets.size());
            elementCount = other.elementCount;
            loadFactorThreshold = other.loadFactorThreshold;

            hasher = other.hasher->clone();
            equality = other.equality->clone();
            ownsHasher = true;
            ownsEquality = true;

            for (size_t i = 0; i < other.buckets.size(); ++i) {
                buckets[i] = other.buckets[i];
            }
        }
        return *this;
    }
    // move constructor
    Hashtable(Hashtable&& other) noexcept
        : buckets(std::move(other.buckets)),
        hasher(other.hasher),
        equality(other.equality),
        elementCount(other.elementCount),
        loadFactorThreshold(other.loadFactorThreshold),
        ownsHasher(other.ownsHasher),
        ownsEquality(other.ownsEquality) {

        other.hasher = nullptr;
        other.equality = nullptr;
        other.elementCount = 0;
        other.ownsHasher = false;
        other.ownsEquality = false;
    }

    // move assignment
    Hashtable& operator=(Hashtable&& other) noexcept {
        if (this != &other) {
            cleanup();

            buckets = std::move(other.buckets);
            hasher = other.hasher;
            equality = other.equality;
            elementCount = other.elementCount;
            loadFactorThreshold = other.loadFactorThreshold;
            ownsHasher = other.ownsHasher;
            ownsEquality = other.ownsEquality;

            other.hasher = nullptr;
            other.equality = nullptr;
            other.elementCount = 0;
            other.ownsHasher = false;
            other.ownsEquality = false;
        }
        return *this;
    }

    ~Hashtable() {
        cleanup();
    }

    void insert(const K& key, const V& value) {
        if (loadFactor() > loadFactorThreshold) {
            rehash();
        }

        size_t index = getBucketIndex(key);
        auto& bucket = buckets[index];

        for (auto& pair : bucket) {
            if (equality->operator()(pair.key, key)) {
                pair.value = value;
                return;
            }
        }

        bucket.emplace_back(key, value);
        ++elementCount;
    }

    V& at(const K& key) {
        size_t index = getBucketIndex(key);
        auto& bucket = buckets[index];

        for (auto& pair : bucket) {
            if (equality->operator()(pair.key, key)) {
                return pair.value;
            }
        }

        throw std::out_of_range("Key not found");
    }

    const V& at(const K& key) const {
        size_t index = getBucketIndex(key);
        const auto& bucket = buckets[index];

        for (const auto& pair : bucket) {
            if (equality->operator()(pair.key, key)) {
                return pair.value;
            }
        }

        throw std::out_of_range("Key not found");
    }

    bool contains(const K& key) const {
        size_t index = getBucketIndex(key);
        const auto& bucket = buckets[index];

        for (const auto& pair : bucket) {
            if (equality->operator()(pair.key, key)) {
                return true;
            }
        }

        return false;
    }

    bool erase(const K& key) {
        size_t index = getBucketIndex(key);
        auto& bucket = buckets[index];

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (equality->operator()(it->key, key)) {
                bucket.erase(it);
                --elementCount;
                return true;
            }
        }

        return false;
    }

    size_t size() const {
        return elementCount;
    }

    bool empty() const {
        return elementCount == 0;
    }

    double loadFactor() const {
        return static_cast<double>(elementCount) / buckets.size();
    }

    void setLoadFactorThreshold(double threshold) {
        loadFactorThreshold = threshold;
    }

    V& operator[](const K& key) {
        try {
            return at(key);
        }
        catch (const std::out_of_range&) {
            insert(key, V{});
            return at(key);
        }
    }
};