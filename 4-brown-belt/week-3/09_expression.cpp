#include "Common.h"
#include "test_runner.h"

#include <exception>
#include <sstream>

using namespace std;

class ValueExpr : public Expression {
public:

	explicit ValueExpr(int value_) :
		value(value_) {}

	int Evaluate() const override {
		return value;
	}

	std::string ToString() const override {
		return std::to_string(value);
	}

private:

	int value;
};

class BinaryExpr : public Expression {
public:

	explicit BinaryExpr(
		ExpressionPtr left_,
		const char* operation_,
		ExpressionPtr right_) :
		operation(operation_),
		left(std::move(left_)),
		right(std::move(right_)) {}

	int Evaluate() const override {
		switch (*operation) {
		case '+':
			return left->Evaluate() + right->Evaluate();
		case '*':
			return left->Evaluate() * right->Evaluate();
		default:
			throw std::logic_error("invalid operation");
		}
		return 0;
	}

	std::string ToString() const override {
		return "(" + left->ToString() + ")"
			+ operation
			+ "(" + right->ToString() + ")";
	}

private:

	const char* operation;
	ExpressionPtr left;
	ExpressionPtr right;
};

// Функции для формирования выражения
ExpressionPtr Value(int value) {
	return std::make_unique<ValueExpr>(value);
}
ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
	return std::make_unique<BinaryExpr>(std::move(left), "+", std::move(right));
}
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
	return std::make_unique<BinaryExpr>(std::move(left), "*", std::move(right));
}

string Print(const Expression* e) {
	if (!e) {
		return "Null expression provided";
	}
	stringstream output;
	output << e->ToString() << " = " << e->Evaluate();
	return output.str();
}

void Test() {
	ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
	ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

	ExpressionPtr e2 = Sum(move(e1), Value(5));
	ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

	ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, Test);
	return 0;
}
