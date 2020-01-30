#include "node.h"

bool EmptyNode::Evaluate(const Date& date, const string& event) {
	return true;
}

bool DateComparisonNode::Evaluate(const Date& date, const string& event) {
	switch (m_comparison) {
	case Comparison::Less:
		return date < m_date;
	case Comparison::LessOrEqual:
		return date < m_date || m_date == date;
	case Comparison::Greater:
		return m_date < date;
	case Comparison::GreaterOrEqual:
		return m_date < date || m_date == date;
	case Comparison::Equal:
		return m_date == date;
	case Comparison::NotEqual:
		return !(m_date == date);
	default: ;
	}
	throw logic_error("DateComparisonNode: invalid Comparison value");
}

bool EventComparisonNode::Evaluate(const Date& date, const string& event) {
	switch (m_comparison) {
	case Comparison::Less:
		return event < m_event;
	case Comparison::LessOrEqual:
		return event < m_event || m_event == event;
	case Comparison::Greater:
		return m_event < event;
	case Comparison::GreaterOrEqual:
		return m_event < event || m_event == event;
	case Comparison::Equal:
		return m_event == event;
	case Comparison::NotEqual:
		return !(m_event == event);
	default: ;
	}
	throw logic_error("EventComparisonNode: invalid Comparison value");
}

bool LogicalOperationNode::Evaluate(const Date& date, const string& event) {
	switch (m_operation) {
	case LogicalOperation::And:
		return m_left->Evaluate(date, event) && m_right->Evaluate(date, event);
	case LogicalOperation::Or:
		return m_left->Evaluate(date, event) || m_right->Evaluate(date, event);
	default: ;
	}
	throw logic_error("LogicalComparisonNode: invalid LogicalOperation value");
}
