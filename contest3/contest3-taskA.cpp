
#include <cstdlib>
#include <iostream>
#include <utility>

class TreapSet {
 public:
  struct Node {
    Node(int x, int y, int pos = 0)
        : x(x),
          y(y),
          pos(pos),
          left(nullptr),
          right(nullptr),
          parent(nullptr) {}

    int x;
    int y;
    int pos;
    Node* left;
    Node* right;
    Node* parent;
  };

  TreapSet() : root_(nullptr) {}

  ~TreapSet() { Clear(root_); }

  void Erase(int value) {
    if (!Find(value)) {
      return;
    }
    auto left_val = Split(root_, value);
    auto tree_val = Split(left_val.first, value - 1);
    delete tree_val.second;
    root_ = Merge(tree_val.first, left_val.second);
  }
  /*
    void EraseFind(int value) {
      auto parent_node = Find(nullptr, root_, value);
      if (parent_node.first == nullptr && parent_node.second != nullptr) {
        root_ = Merge(root_->left, root_->right);
      } else {
        ((parent_node.first->left == parent_node.second) ?
    parent_node.second->left : parent_node.first->right) =
            Merge(parent_node.second->left, parent_node.second->right);
      }
      if (parent_node.second != nullptr) {
        delete parent_node.second;
      }
    }
  */
  void Insert(int value) {
    if (Find(value)) {
      return;
    }
    int y = rand();
    Node* node = new Node(value, y);
    auto trees = Split(root_, value);
    root_ = Merge(Merge(trees.first, node), trees.second);
  }

  Node* Insert(int value, int y, int pos, Node* prev) {
    Node* node = new Node(value, y, pos);
    if (prev == nullptr) {
      auto trees = Split(root_, value);
      root_ = Merge(Merge(trees.first, node), trees.second);
    } else {
      if (prev->y < y) {
        prev->left = node;
        node->parent = prev;
      } else {
        while (prev != nullptr && prev->y >= y) {
          prev = prev->parent;
        }
        if (prev == nullptr) {
          node->right = root_;
          root_->parent = node;
          root_ = node;
        } else {
          node->right = prev->left;
          if (prev->left != nullptr) {
            prev->left->parent = node;
          }
          prev->left = node;
          node->parent = prev;
        }
      }
    }

    return node;
  }

  bool Find(int value) { return Find(nullptr, root_, value).second != nullptr; }

  void Traverse() {
    int counter = 1;
    Traverse(root_, counter);
  }

  void PrintNodes() { PrintNodes(root_); }

 private:
  std::pair<Node*, Node*> Find(Node* parent, Node* node, int value) {
    if (node == nullptr) {
      return {parent, node};
    }
    if (value == node->x) {
      return {parent, node};
    }
    if (value <= node->x) {
      return Find(node, node->left, value);
    }
    return Find(node, node->right, value);
  }

  void Traverse(Node* node, int& counter) {
    if (node == nullptr) {
      return;
    }
    Traverse(node->left, counter);
    node->pos = counter;
    ++counter;
    Traverse(node->right, counter);
  }

  void PrintNodes(Node* node) {
    if (node == nullptr) {
      return;
    }
    PrintNodes(node->right);
    int key = (node->parent == nullptr ? 0 : node->parent->pos);
    int left = (node->left == nullptr ? 0 : node->left->pos);
    int right = (node->right == nullptr ? 0 : node->right->pos);
    std::cout << key << ' ' << right << ' ' << left << '\n';
    PrintNodes(node->left);
  }

  void Clear(Node* node) {
    if (node == nullptr) {
      return;
    }
    Clear(node->left);
    Clear(node->right);
    delete node;
  }

  Node* Merge(Node* first, Node* second) {
    if (first == nullptr) {
      return second;
    }
    if (second == nullptr) {
      return first;
    }  //>
    if (first->y < second->y) {
      first->right = Merge(first->right, second);
      first->right->parent = first;
      return first;
    }
    second->left = Merge(first, second->left);
    second->left->parent = second;
    return second;
  }

  std::pair<Node*, Node*> Split(Node* node, int key) {
    if (node == nullptr) {
      return {nullptr, nullptr};
    }
    if (key <= node->x) {
      auto trees = Split(node->left, key);
      node->left = trees.second;
      if (node->left != nullptr) {
        node->left->parent = node;
      }
      return {trees.first, node};
    }
    auto trees = Split(node->right, key);
    node->right = trees.first;
    if (node->right != nullptr) {
      node->right->parent = trees.first;
    }
    return {node, trees.second};
  }

  Node* root_;
};

void ReadNodes(TreapSet* treap) {
  size_t nodes;
  std::cin >> nodes;
  int key, y;
  TreapSet::Node* prev = nullptr;
  for (size_t i = 0; i < nodes; ++i) {
    std::cin >> key >> y;
    prev = treap->Insert(key, y, i + 1, prev);
  }
}

int main() {
  auto treap = new TreapSet();

  ReadNodes(treap);
  std::cout << "YES\n";

  treap->PrintNodes();

  delete treap;
  return 0;
}
