#include <iostream>
#include "ListIterator.hpp"
#include "LinkedList.hpp"
#include "DoubleLinkedList.hpp"
#include "hash.cpp"

#include <iostream>
#include <string>
#include <cassert>

// Custom hasher for strings that ignores case
class CaseInsensitiveHasher : public Hasher<std::string> {
public:
    size_t hash(const std::string& key) const override {
        std::string lowerKey = key;
        for (char& c : lowerKey) {
            c = std::tolower(c);
        }
        return std::hash<std::string>{}(lowerKey);
    }

    CaseInsensitiveHasher* clone() const override {
        return new CaseInsensitiveHasher();
    }
};

// Custom equality for strings that ignores case
class CaseInsensitiveEquality : public EqualityPredicate<std::string> {
public:
    bool operator()(const std::string& a, const std::string& b) const override {
        if (a.length() != b.length()) return false;
        for (size_t i = 0; i < a.length(); ++i) {
            if (std::tolower(a[i]) != std::tolower(b[i])) {
                return false;
            }
        }
        return true;
    }

    CaseInsensitiveEquality* clone() const override {
        return new CaseInsensitiveEquality();
    }
};

void test_basic_operations() {
    std::cout << "=== Testing Basic Operations ===" << std::endl;

    Hashtable<int, std::string> table;

    table.insert(1, "one");
    table.insert(2, "two");
    table.insert(3, "three");

    std::cout << "Size: " << table.size() << " (expected: 3)" << std::endl;
    assert(table.size() == 3);
    assert(table.at(1) == "one");
    assert(table[2] == "two");

    std::cout << "Basic operations test PASSED!\n" << std::endl;
}

void test_custom_hasher_equality() {
    std::cout << "Testing Custom Hasher and Equality" << std::endl;

    // 在堆上创建自定义对象
    CaseInsensitiveHasher* customHasher = new CaseInsensitiveHasher();
    CaseInsensitiveEquality* customEquality = new CaseInsensitiveEquality();

    Hashtable<std::string, int> table(16, customHasher, customEquality);

    table.insert("Hello", 100);
    table.insert("HELLO", 200);  // 应该更新而不是新建

    std::cout << "table[\"hello\"]: " << table["hello"] << " (expected: 200)" << std::endl;
    assert(table["hello"] == 200);
    assert(table.size() == 1);  // 大小应为1，因为"Hello"和"HELLO"被视为相同

    std::cout << "Custom hasher/equality test PASSED!\n" << std::endl;
}

void test_copy_and_move() {
    std::cout << "Testing Copy and Move Operations" << std::endl;

    Hashtable<int, std::string> original;
    original.insert(1, "one");
    original.insert(2, "two");

    // 测试拷贝构造
    Hashtable<int, std::string> copy(original);
    assert(copy.size() == 2);
    assert(copy[1] == "one");

    // 测试拷贝赋值
    Hashtable<int, std::string> assigned;
    assigned = original;
    assert(assigned.size() == 2);
    assert(assigned[2] == "two");

    // 测试移动构造
    Hashtable<int, std::string> moved(std::move(copy));
    assert(moved.size() == 2);
    assert(moved[1] == "one");

    std::cout << "Copy and move operations test PASSED!\n" << std::endl;
}


int main() {
    cout << "Linked List Tests\n";
    LinkedList<int> list;

    list.Add(10);
    list.Add(20);
    list.Add(30);
    list.Insert(15, 1);

    auto it = list.Iterator();

    std::cout << "List contents:\n";
    while (it.HasNext()) {
        std::cout << it.Next() << " ";
    }
    std::cout << "\n";

    std::cout << "IndexOf(20) = " << list.IndexOf(20) << "\n";
    std::cout << "Get(2) = " << list.Get(2) << "\n";
    std::cout << "Get(1) = " << list.Get(1) << "\n";

    std::cout << "Remove(1) = " << list.Remove(1) << "\n";

    auto it2 = list.Iterator();

    std::cout << "After removal:\n";
    while (it2.HasNext()) {
        std::cout << it2.Next() << " ";
    }
    std::cout << "\n";
    cout << "Doubly Linked List Tests:\n";
    DoublyLinkedList<int> dlist;

    cout << "Add 1~5" << endl;
    for (int i = 1; i <= 5; i++) dlist.Add(i);

    auto dit = dlist.Iterator();
    while (dit.HasNext()) {
        std::cout << dit.Next() << " ";
    }
    cout << "\n\n";

    int x = 999;
    cout << "Insert 999 at index 2\n";
    dlist.Insert(x, 2);

    dit = dlist.Iterator();
    while (dit.HasNext()) cout << dit.Next() << " ";
    cout << "\n\n";

    cout << "Get index 3 = " << dlist.Get(3) << "\n";
    cout << "IndexOf(999) = " << dlist.IndexOf(x) << "\n";

    cout << "Remove index 2 = " << dlist.Remove(2) << "\n";

    dit = dlist.Iterator();
    while (dit.HasNext()) cout << dit.Next() << " ";
    cout << "\n";
    
    std::cout << "Hash Table Tests\n" << std::endl;

    try {
        test_basic_operations();
        test_custom_hasher_equality();
        test_copy_and_move();

        std::cout << "ALL TESTS PASSED SUCCESSFULLY!" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Starting Fixed Hash Table Tests...\n" << std::endl;

    try {
        test_basic_operations();
        test_custom_hasher_equality();
        test_copy_and_move();

        std::cout << "ALL TESTS PASSED SUCCESSFULLY!" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
