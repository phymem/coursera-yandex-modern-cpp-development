#pragma once

#include "date.h"

#include <string>
#include <memory>

using namespace std;

enum class Comparison {
	Less,
	LessOrEqual,
	Greater,
	GreaterOrEqual,
	Equal,
	NotEqual
};

enum class LogicalOperation {
	Or,
	And
};

class Node {
public:
	virtual bool Evaluate(const Date& date, const string& event) = 0;
};

class EmptyNode : public Node {
public:

	EmptyNode() {}

	bool Evaluate(const Date& date, const string& event) override;
};

class DateComparisonNode : public Node {
public:

	DateComparisonNode(Comparison cmp, const Date& date) :
		m_comparison(cmp), m_date(date) {}

	bool Evaluate(const Date& date, const string& event) override;

private:

	Comparison m_comparison;
	Date m_date;
};

class EventComparisonNode : public Node {
public:

	EventComparisonNode(Comparison cmp, const string& event) :
		m_comparison(cmp), m_event(event) {}

	bool Evaluate(const Date& date, const string& event) override;

private:

	Comparison m_comparison;
	const string m_event;
};

class LogicalOperationNode : public Node {
public:

	LogicalOperationNode(LogicalOperation op, shared_ptr<Node> left, shared_ptr<Node> right) :
		m_operation(op), m_left(left), m_right(right) {}

	bool Evaluate(const Date& date, const string& event) override;

private:

	LogicalOperation m_operation;
	shared_ptr<Node> m_left;
	shared_ptr<Node> m_right;
};
