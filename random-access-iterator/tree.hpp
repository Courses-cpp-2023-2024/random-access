#include <cassert>
#include <cstddef>
#include <iterator>
#include <memory>

struct tree_node {
    int value;
    tree_node *up = nullptr;
    std::unique_ptr<tree_node> left{}, right{};
    int subtree_size = 1;

    // Используется только для простого описания дерева в коде
    struct nullptr_node {};

    // Правильнее бы этому конструктору быть explicit, хочется красиво дерево
    // описывать
    tree_node(int val) : value(val) {}
    tree_node(nullptr_node /*unused*/, int val, tree_node &&b)
        : value(val), left(nullptr),
          right(std::make_unique<tree_node>(std::move(b))) {
        right->up = this;
    }
    tree_node(tree_node &&a, int val, nullptr_node /*unused*/)
        : value(val), left(std::make_unique<tree_node>(std::move(a))),
          right(nullptr) {
        left->up = this;
    }
    tree_node(tree_node &&a, int val, tree_node &&b)
        : value(val), left(std::make_unique<tree_node>(std::move(a))),
          right(std::make_unique<tree_node>(std::move(b))),
          subtree_size(left->subtree_size + 1 + right->subtree_size) {
        left->up = right->up = this;
    }
    tree_node(tree_node &&other) noexcept
        : value(other.value), up(other.up), left(std::move(other.left)),
          right(std::move(other.right)),
          subtree_size((left ? left->subtree_size : 0) + 1 +
                       (right ? right->subtree_size : 0)) {
        assert(up == nullptr);
        if (left)
            left->up = this;
        if (right)
            right->up = this;
    }
    tree_node(tree_node const &) = delete;
    tree_node &operator=(tree_node const &) = delete;
    tree_node &operator=(tree_node &&) = delete;
    ~tree_node() = default;
};

struct tree {
    std::unique_ptr<tree_node> root;
    class iterator {
        tree_node *p;

      public:
        iterator operator++(int) { // post-increment
            iterator it = *this;
            ++*this;
            return it;
        }
        iterator operator--(int) { // post-decrement
            iterator it = *this;
            --*this;
            return it;
        }
        [[nodiscard]] int const &operator*() const { return p->value; }
        // реализовать проверку на попытку получения элемента за пределами
        // индексов дерева
        [[nodiscard]] int const &at() const {
            if (p == nullptr) {
        throw std::out_of_range("index out of range");
            }
            return p->value;
        }

        [[nodiscard]] int const *operator->() const { return &p->value; }
        [[nodiscard]] friend bool operator==(iterator const &a,
                                             iterator const &b) {
            return a.p == b.p;
        }
        [[nodiscard]] friend bool operator!=(iterator const &a,
                                             iterator const &b) {
            return a.p != b.p;
        }
        [[nodiscard]] friend bool operator<(iterator const &a,
                                            iterator const &b) {
            return a - b < 0;
        }
        [[nodiscard]] friend bool operator<=(iterator const &a,
                                             iterator const &b) {
            return a - b <= 0;
        }
        [[nodiscard]] friend bool operator>(iterator const &a,
                                            iterator const &b) {
            return a - b > 0;
        }
        [[nodiscard]] friend bool operator>=(iterator const &a,
                                             iterator const &b) {
            return a - b >= 0;
        }
        [[nodiscard]] friend iterator operator-(iterator const &it,
                                                ptrdiff_t diff) {
            return it + (-diff);
        }
        [[nodiscard]] int const &operator[](ptrdiff_t idx) const {
            return *(*this + idx);
        }
        iterator &operator+=(ptrdiff_t diff) { return *this = *this + diff; }
        iterator &operator-=(ptrdiff_t diff) { return *this = *this - diff; }
        iterator &operator++() { return *this += 1; } // pre-increment
        iterator &operator--() { return *this -= 1; } // pre-decrement
        [[nodiscard]] friend iterator operator+(ptrdiff_t diff,
                                                iterator const &self) {
            return self + diff;
        }
        // Это всё нужно реализовать
        friend iterator operator+(iterator const &self, ptrdiff_t diff);
        friend ptrdiff_t operator-(iterator const &self, iterator other);
        // Скорее всего придётся реализовать
        explicit iterator(tree_node *x) : p(x) {}

        // Начиная с C++20 следующие 5 строк генерируются автоматически
        using difference_type = ptrdiff_t;
        using value_type = int;
        using pointer = int const *;
        using reference = int const &;
        using iterator_category = std::random_access_iterator_tag;
    };

    explicit tree(tree_node &&a)
        : root(std::make_unique<tree_node>(std::move(a))) {
        root->up = nullptr;
    }
    // И эти две функции тоже нужно реализовать
    [[nodiscard]] iterator begin() const;
    [[nodiscard]] iterator end() const;
    // Эту функцию реализовать намного проще, чем operator+ и operator-
    // Можете для разминки реализовать её напрямую, а потом уже приступать к
    // более сложной части
    [[nodiscard]] int operator[](ptrdiff_t idx) const { return begin()[idx]; }
    [[nodiscard]] int const &at(ptrdiff_t idx) const {
        return (begin() + idx).at();
    };
};

constexpr tree_node::nullptr_node nil{};
