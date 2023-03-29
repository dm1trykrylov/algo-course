#include <iostream>
#include <queue>
#include <set>
#include <unordered_set>
#include <vector>

template <class V>
class FibonacciHeap;

template <class V>
struct node {
 private:
  node<V>* prev;
  node<V>* next;
  node<V>* child;
  node<V>* parent;
  V value;
  int degree;
  bool marked;

 public:
  friend class FibonacciHeap<V>;
  node<V>* getPrev() { return prev; }
  node<V>* getNext() { return next; }
  node<V>* getChild() { return child; }
  node<V>* getParent() { return parent; }
  V getValue() { return value; }
  bool isMarked() { return marked; }

  bool hasChildren() { return child; }
  bool hasParent() { return parent; }
};

template <class V>
class FibonacciHeap {
 protected:
  node<V>* heap;

 public:
  FibonacciHeap() { heap = _empty(); }
  virtual ~FibonacciHeap() {
    if (heap) {
      _deleteAll(heap);
    }
  }
  node<V>* insert(V value) {
    node<V>* ret = _singleton(value);
    heap = _merge(heap, ret);
    return ret;
  }
  void merge(FibonacciHeap& other) {
    heap = _merge(heap, other.heap);
    other.heap = _empty();
  }

  bool isEmpty() { return heap == NULL; }

  V getMinimum() { return heap->value; }

  V removeMinimum() {
    node<V>* old = heap;
    heap = _removeMinimum(heap);
    V ret = old->value;
    delete old;
    return ret;
  }

  void decreaseKey(node<V>* n, V value) { heap = _decreaseKey(heap, n, value); }

  node<V>* find(V value) { return _find(heap, value); }

 private:
  node<V>* _empty() { return NULL; }

  node<V>* _singleton(V value) {
    node<V>* n = new node<V>;
    n->value = value;
    n->prev = n->next = n;
    n->degree = 0;
    n->marked = false;
    n->child = NULL;
    n->parent = NULL;
    return n;
  }

  node<V>* _merge(node<V>* a, node<V>* b) {
    if (a == NULL) return b;
    if (b == NULL) return a;
    if (a->value > b->value) {
      node<V>* temp = a;
      a = b;
      b = temp;
    }
    node<V>* an = a->next;
    node<V>* bp = b->prev;
    a->next = b;
    b->prev = a;
    an->prev = bp;
    bp->next = an;
    return a;
  }

  void _deleteAll(node<V>* n) {
    if (n != NULL) {
      node<V>* c = n;
      do {
        node<V>* d = c;
        c = c->next;
        _deleteAll(d->child);
        delete d;
      } while (c != n);
    }
  }

  void _addChild(node<V>* parent, node<V>* child) {
    child->prev = child->next = child;
    child->parent = parent;
    parent->degree++;
    parent->child = _merge(parent->child, child);
  }

  void _unMarkAndUnParentAll(node<V>* n) {
    if (n == NULL) return;
    node<V>* c = n;
    do {
      c->marked = false;
      c->parent = NULL;
      c = c->next;
    } while (c != n);
  }

  node<V>* _removeMinimum(node<V>* n) {
    _unMarkAndUnParentAll(n->child);
    if (n->next == n) {
      n = n->child;
    } else {
      n->next->prev = n->prev;
      n->prev->next = n->next;
      n = _merge(n->next, n->child);
    }
    if (n == NULL) return n;
    node<V>* trees[64] = {NULL};

    while (true) {
      if (trees[n->degree] != NULL) {
        node<V>* t = trees[n->degree];
        if (t == n) break;
        trees[n->degree] = NULL;
        if (n->value < t->value) {
          t->prev->next = t->next;
          t->next->prev = t->prev;
          _addChild(n, t);
        } else {
          t->prev->next = t->next;
          t->next->prev = t->prev;
          if (n->next == n) {
            t->next = t->prev = t;
            _addChild(t, n);
            n = t;
          } else {
            n->prev->next = t;
            n->next->prev = t;
            t->next = n->next;
            t->prev = n->prev;
            _addChild(t, n);
            n = t;
          }
        }
        continue;
      } else {
        trees[n->degree] = n;
      }
      n = n->next;
    }
    node<V>* min = n;
    node<V>* start = n;
    do {
      if (n->value < min->value) min = n;
      n = n->next;
    } while (n != start);
    return min;
  }

  node<V>* _cut(node<V>* heap, node<V>* n) {
    if (n->next == n) {
      n->parent->child = NULL;
    } else {
      n->next->prev = n->prev;
      n->prev->next = n->next;
      n->parent->child = n->next;
    }
    n->next = n->prev = n;
    n->marked = false;
    return _merge(heap, n);
  }

  node<V>* _decreaseKey(node<V>* heap, node<V>* n, V value) {
    if (n->value < value) return heap;
    n->value = value;
    if (n->parent) {
      if (n->value < n->parent->value) {
        heap = _cut(heap, n);
        node<V>* parent = n->parent;
        n->parent = NULL;
        while (parent != NULL && parent->marked) {
          heap = _cut(heap, parent);
          n = parent;
          parent = n->parent;
          n->parent = NULL;
        }
        if (parent != NULL && parent->parent != NULL) parent->marked = true;
      }
    } else {
      if (n->value < heap->value) {
        heap = n;
      }
    }
    return heap;
  }

  node<V>* _find(node<V>* heap, V value) {
    node<V>* n = heap;
    if (n == NULL) return NULL;
    do {
      if (n->value == value) return n;
      node<V>* ret = _find(n->child, value);
      if (ret) return ret;
      n = n->next;
    } while (n != heap);
    return NULL;
  }
};

const size_t kMax = 2e9;

size_t Prim(size_t count, size_t mod, std::vector<size_t>& costs) {
  std::vector<size_t> key(count);
  for (size_t i = 0; i < count; ++i) {
    key[i] = kMax;
  }
  size_t start = 0;
  size_t counter = count;
  key[0] = 0;
  FibonacciHeap<std::pair<size_t, size_t>> queue;
  std::unordered_set<size_t> queue_v;
  // std::set<std::pair<size_t, size_t>> queue;
  for (size_t i = 0; i < count; ++i) {
    queue.insert({key[i], i});
    queue_v.insert(i);
  }
  std::vector<size_t> parents(count);
  parents[0] = 0;
  size_t from;
  size_t mst_cost = 0;
  size_t old_key;
  while (!queue.isEmpty()) {
    from = queue.getMinimum().second;
    old_key = queue.getMinimum().first;
    queue.removeMinimum();
    queue_v.erase(from);
    if (old_key > key[from]) {
      continue;
    }

    --count;
    if (from != start) {
      mst_cost += ((costs[from] + costs[parents[from]]) % mod);
    }
    for (auto v : queue_v) {
      if (key[v] > ((costs[from] + costs[v]) % mod)) {
        old_key = key[v];
        parents[v] = from;
        key[v] = ((costs[from] + costs[v]) % mod);
        // auto old = std::make_pair(old_key, v);
        queue.insert({key[v], v});
        // queue.decreaseKey();
      }
    }
  }
  return mst_cost;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  size_t vertices_count, M;
  std::cin >> vertices_count >> M;
  std::vector<size_t> costs(vertices_count);
  for (size_t i = 0; i < vertices_count; ++i) {
    std::cin >> costs[i];
  }
  std::cout << Prim(vertices_count, M, costs);
  return 0;
}