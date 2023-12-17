#include "tree.hpp"

tree::iterator operator+(tree::iterator const &self, ptrdiff_t diff) {
    auto tmp = self;
    ptrdiff_t i = 0;

    while (i > diff) {
        tmp--;
        i--;
    }

    while (i < diff) {
        tmp++;
        i++;
    }

    return tmp;
}

ptrdiff_t operator-(tree::iterator const &self, tree::iterator other) {
    return self.index - other.index;
}

auto tree::begin() const -> iterator {
    auto ptr = root.get();
    while (ptr->left) {
        ptr = ptr->left.get();
    }
    return {ptr, 0};
}

auto tree::end() const -> iterator {
    return iterator{root.get(), root->subtree_size};
}
