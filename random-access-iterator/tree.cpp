#include "tree.hpp"
#include <memory>
#include <utility>
#include <vector>

auto tree::begin() const -> iterator {
    if (empty()) return end();
    auto it = iterator{root.get(), 0};
    while (it->left) {
        ++it.index;
        it.node = it->left.get();
    }
    return it;
}

auto tree::end() const -> iterator {
    return iterator{nullptr, 0};
}

reference tree_iterator::at(size_t n) const {
    if (!node || n >= node->subtree_size) throw std::out_of_range("Index out of range");
    auto current = node;
    size_t count = 0;
    while (count < n && current) {
        if (current->left) {
            current = current->left.get();
            count += current->subtree_size - count;
        } else {
            --count;
            current = current->right.get();
        }
    }
    return *current;
}


tree::iterator& tree::iterator::operator+(ptrdiff_t diff) {
    if (diff > 0) {
        size_t target_index = static_cast<size_t>(index + diff);
        if (target_index >= node->subtree_size) {
            while (node->right) {
                node = node->right.get();
            }
            target_index -= node->subtree_size;
        }
        find_child_by_index(node, true, target_index);
    } else if (diff < 0) {
        size_t target_index = static_cast<size_t>(index + diff);
        if (target_index <= node->subtree_size) {
            node = node->parent.lock().get();
            find_child_by_index(node, false, target_index);
        } else {
            target_index -= node->subtree_size - 1;
            find_child_by_index(node, false, target_index);
        }
    }
    return *this;
}

ptrdiff_t tree::iterator::operator-(const iterator& other) const {
    if (node == other.node) return 0;
    size_t dist = 0;
    for (auto curr = this->node, prev = other.node; curr != prev; ) {
        if (curr->parent.lock()->left.get() == curr.node) {
            dist += curr->parent.lock()->subtree_size - curr->index;
            curr = curr->parent.lock();
        } else {
            dist += curr->index;
            curr = curr->left.get();
        }
    }
    return dist;
}
