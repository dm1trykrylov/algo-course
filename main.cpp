#include <algorithm>
#include <iostream>
#include <string>

const int kInf = 1e9 + 2;

struct Node {
  int min;
  int val;
  Node* prev;

  Node() : Node(0, kInf, nullptr) {}

  Node(int new_val) : Node(new_val, kInf, nullptr) {}

  Node(int new_val, int new_min, Node* previous) {
    min = new_min;
    val = new_val;
    prev = previous;
  }
};

struct Stack {
  Node* top;
  Node* root;
  int size;

  Stack() {
    top = nullptr;
    root = nullptr;
    size = 0;
  }

  void Push(int value) {
    Node* new_top = new Node(value, value, top);
    int prev_min = kInf;
    if (this->size > 0) {
      prev_min = top->min;
    } else {
      this->root = new_top;
    }
    new_top->min = std::min(value, prev_min);
    top = new_top;
    ++size;
  }

  void Pop() {
    if (this->size > 0) {
      Node* old_top = top;
      --size;
      top = top->prev;
      delete (old_top);
    }
  }

  int Back() {
    int back = -kInf;
    if (top != NULL) {
      back = top->val;
    }
    return back;
  }

  int Min() {
    int min_value = kInf;
    if (top != NULL) {
      min_value = top->min;
    }
    return min_value;
  }

  int Size() { return this->size; }

  bool IsEmpty() { return this->top == NULL; }

  void Clear() {
    while (this->top != NULL) {
      this->Pop();
    }
  }

  ~Stack() { delete top; }
};

struct Deque {
  Stack* front;
  Stack* back;
  // deque: front--new_elements ... old_elements--back

  Deque() {
    front = new Stack();
    back = new Stack();
  }

  void PushFront(int value) { front->Push(value); }

  // delete oldest element
  void PopBack() {
    if (back->IsEmpty()) {
      Shift(front, back);
    }
    back->Pop();
  }

  // delete newest element
  void PopFront() {
    if (front->IsEmpty()) {
      Shift(back, front);
    }
    front->Pop();
  }

  void Shift(Stack* from, Stack* to) {
    while (!from->IsEmpty()) {
      to->Push((from->Back()));
      from->Pop();
    }
  }

  int Size() { return front->size + back->size; }

  bool IsEmpty() { return this->Size() == 0; }

  // return oldest element
  int Back() {
    int back_value = 0;
    if (this->IsEmpty()) {
      back_value = -kInf;
    } else {
      if (this->back->IsEmpty()) {
        back_value = this->front->root->val;
      } else {
        back_value = this->back->top->val;
      }
    }
    return back_value;
  }

  // return newest element
  int Front() {
    int front_value = 0;
    if (this->IsEmpty()) {
      front_value = -kInf;
    } else {
      if (this->back->IsEmpty()) {
        front_value = this->front->root->val;
      } else {
        front_value = this->back->top->val;
      }
    }
    return front_value;
  }

  int Min() { return std::min(front->Min(), back->Min()); }

  void Clear() {
    if (front != NULL) {
      front->Clear();
    }
    if (back != NULL) {
      back->Clear();
    }
  }

  ~Deque() {
    delete front;
    delete back;
  }
};

void CheckValue(const int kValue) {
  if (kValue == kInf || kValue == -kInf) {
    std::cout << "error";
  } else {
    std::cout << kValue;
  }
}

void PopBack(Deque* deque) {
  if (deque->IsEmpty()) {
    std::cout << "error";
  } else {
    std::cout << deque->Back();
    deque->PopBack();
  }
}

void PopFront(Deque* deque) {
  if (deque->IsEmpty()) {
    std::cout << "error";
  } else {
    std::cout << deque->Front();
    deque->PopFront();
  }
}

void ExecuteCommand(const std::string* command, Deque* deque) {
  int value;

  if (*command == "enqueue") {
    std::cin >> value;
    deque->PushFront(value);
    std::cout << "ok";
  } else if (*command == "dequeue") {
    PopBack(deque);
  } else if (*command == "front") {
    CheckValue(deque->Back());
  } else if (*command == "min") {
    CheckValue(deque->Min());
  } else if (*command == "size") {
    std::cout << deque->Size();
  } else if (*command == "clear") {
    deque->Clear();
    std::cout << "ok";
  }
  std::cout << '\n';
}

int main() {
  size_t commands_count;
  std::string command;

  Deque* deque = new Deque();

  std::cin >> commands_count;

  for (size_t i = 0; i < commands_count; ++i) {
    std::cin >> command;
    ExecuteCommand(&command, deque);
  }

  deque->Clear();
  delete deque;
  return 0;
}

