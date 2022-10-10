#include <algorithm>
#include <iostream>
#include <optional>
#include <string>

const int kInf = 1e9 + 2;
const std::string kError = "error";

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

  std::optional<int> Back() {
    if (top == NULL) {
      return std::nullopt;
    }
    return top->val;
  }

  std::optional<int> Min() {
    if (top == NULL) {
      return std::nullopt;
    }
    return top->min;
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
    std::optional<int> back = from->Back();
    while (back.has_value()) {
      to->Push(back.value());
      from->Pop();
      back = from->Back();
    }
  }

  int Size() { return front->size + back->size; }

  bool IsEmpty() { return this->Size() == 0; }

  // return oldest element
  std::optional<int> Back() {
    if (this->IsEmpty()) {
      return std::nullopt;
    }
    int back_value;
    if (this->back->IsEmpty()) {
      back_value = this->front->root->val;
    } else {
      back_value = this->back->top->val;
    }
    return back_value;
  }

  // return newest element
  std::optional<int> Front() {
    if (this->IsEmpty()) {
      return std::nullopt;
    }
    int front_value;
    if (this->back->IsEmpty()) {
      front_value = this->front->root->val;
    } else {
      front_value = this->back->top->val;
    }
    return front_value;
  }

  std::optional<int> Min() {
    std::optional<int> front_min = front->Min();
    std::optional<int> back_min = back->Min();
    std::optional<int> min_value;
    if (front_min.has_value()) {
      min_value = front_min;
    }
    if (back_min.has_value()) {
      if (min_value.has_value()) {
        min_value = std::min(min_value, back_min);
      } else {
        min_value = back_min;
      }
    }
    return min_value;
  }

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

bool CheckValue(const std::optional<int> kValue) {
  return (kValue == kInf || kValue == -kInf);
}

std::string GetValue(const std::optional<int> kValue) {
  if (kValue.has_value()) {
    return std::to_string(kValue.value());
  }
  return kError;
}

std::optional<int> PopBack(Deque* deque) {
  auto back = deque->Back();
  if (back.has_value()) {
    deque->PopBack();
  }
  return back;
}

std::optional<int> PopFront(Deque* deque) {
  auto front = deque->Front();
  if (front.has_value()) {
    deque->PopFront();
  }
  return front;
}

void ExecuteCommand(const std::string* command, Deque* deque) {
  int value;
  if (*command == "enqueue") {
    std::cin >> value;
    deque->PushFront(value);
    std::cout << "ok";
  } else if (*command == "dequeue") {
    std::cout << GetValue(PopBack(deque));
  } else if (*command == "front") {
    std::cout << GetValue(deque->Back());
  } else if (*command == "min") {
    std::cout << GetValue(deque->Min());
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
