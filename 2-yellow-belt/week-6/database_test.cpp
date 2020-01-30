#include "database.h"
#include "test_runner.h"

#include <sstream>

void TestDatabase_Add() {
	{
		Database db;
		db.Add({ 1111, 11, 15 }, "Wed");
		db.Add({ 1111, 11, 16 }, "Thu");
		db.Add({ 1111, 11, 17 }, "Fri");

		ostringstream os;
		db.Print(os);

		AssertEqual(os.str(),
			"1111-11-15 Wed\n"
			"1111-11-16 Thu\n"
			"1111-11-17 Fri\n", "TestDatabase_Add: add events");
	}
	{
		Database db;
		db.Add({ 1111, 3, 1 }, "01.03");
		db.Add({ 1111, 2, 1 }, "01.02");
		db.Add({ 1111, 1, 1 }, "01.01");

		ostringstream os;
		db.Print(os);

		AssertEqual(os.str(),
			"1111-01-01 01.01\n"
			"1111-02-01 01.02\n"
			"1111-03-01 01.03\n", "TestDatabase_Add: events date sorted");
	}
	{
		Database db;
		db.Add({ 1111, 3, 1 }, "* 1");
		db.Add({ 1111, 3, 4 }, "+ 1");
		db.Add({ 1111, 3, 1 }, "* 2");
		db.Add({ 1111, 3, 1 }, "* 3");
		db.Add({ 1111, 3, 4 }, "+ 2");

		ostringstream os;
		db.Print(os);

		AssertEqual(os.str(),
			"1111-03-01 * 1\n"
			"1111-03-01 * 2\n"
			"1111-03-01 * 3\n"
			"1111-03-04 + 1\n"
			"1111-03-04 + 2\n", "TestDatabase_Add: multiple events");
	}
	{
		Database db;
		db.Add({ 1111, 1, 1 }, "1");
		db.Add({ 1111, 1, 1 }, "2");
		db.Add({ 1111, 1, 1 }, "1");
		db.Add({ 1111, 1, 1 }, "1");

		ostringstream os;
		db.Print(os);

		AssertEqual(os.str(),
			"1111-01-01 1\n"
			"1111-01-01 2\n", "TestDatabase_Add: ignore identical events");
	}
}

void TestDatabase_Find() {
	{
		Database db;
		db.Add({ 1111, 11, 15 }, "Wed");
		db.Add({ 1111, 11, 16 }, "Thu");
		db.Add({ 1111, 11, 17 }, "Fri");

		AssertEqual(db.FindIf([](const Date&, const string&) { return true; }),
			vector<pair<Date, string>> {
				{{ 1111, 11, 15 }, "Wed"},
				{{ 1111, 11, 16 }, "Thu"},
				{{ 1111, 11, 17 }, "Fri"},
			}, "TestDatabase_Find: all");
	}
	{
		Database db;
		db.Add({ 1111, 11, 15 }, "Wed");
		db.Add({ 1111, 11, 16 }, "Thu");
		db.Add({ 1111, 11, 17 }, "Fri");

		AssertEqual(db.FindIf([](const Date&, const string&) { return false; }),
			vector<pair<Date, string>> {}, "TestDatabase_Find: none");
	}
	{
		Database db;
		db.Add({ 1, 1, 1 }, "Fri");
		db.Add({ 2, 1, 1 }, "Thu");
		db.Add({ 3, 1, 1 }, "Tue");
		db.Add({ 4, 1, 1 }, "Mon");

		AssertEqual(db.FindIf(
			[](const Date& date, const string& event) {
				return date.GetYear() == 1 || event == "Mon"; }),
			vector<pair<Date, string>> {
				{{ 1, 1, 1 }, "Fri"},
				{{ 4, 1, 1 }, "Mon"},
			}, "TestDatabase_Find: complex");
	}
}

void TestDatabase_Remove() {
	{
		Database db;
		db.Add({ 1111, 11, 15 }, "Wed");
		db.Add({ 1111, 11, 16 }, "Thu");
		db.Add({ 1111, 11, 17 }, "Fri");

		AssertEqual(db.RemoveIf(
			[](const Date&, const string&) { return true; }),
			3, "TestDatabase_Remove: all #1");

		ostringstream os;
		db.Print(os);

		AssertEqual(os.str(), "", "TestDatabase_Remove: all #2");
	}
	{
		Database db;
		db.Add({ 1, 1, 1 }, "Fri");
		db.Add({ 2, 1, 1 }, "Thu");
		db.Add({ 3, 1, 1 }, "Tue");

		AssertEqual(db.RemoveIf(
			[](const Date&, const string&) { return false; }),
			0, "TestDatabase_Remove: none #1");

		ostringstream os;
		db.Print(os);

		AssertEqual(os.str(),
			"0001-01-01 Fri\n"
			"0002-01-01 Thu\n"
			"0003-01-01 Tue\n",
			"TestDatabase_Remove: none #2");
	}
	{
		Database db;
		db.Add({ 1, 1, 1 }, "Fri");
		db.Add({ 2, 1, 1 }, "Thu");
		db.Add({ 3, 1, 1 }, "Tue");
		db.Add({ 4, 1, 1 }, "Mon");

		auto complexCondition = [](const Date& date, const string& event) {
			return date.GetYear() == 2016 || event == "Monday";
		};
		AssertEqual(db.RemoveIf(
			[](const Date& date, const string& event) {
				return date.GetYear() == 1 || event == "Mon";
			}), 2, "TestDatabase_Remove: complex #1");

		ostringstream os;
		db.Print(os);

		AssertEqual(os.str(),
			"0002-01-01 Thu\n"
			"0003-01-01 Tue\n",
			"TestDatabase_Remove: complex #2");
	}
}

void TestDatabase_Last() {
	Database db;
	db.Add({ 1, 1, 1 }, "Mon");
	db.Add({ 1, 1, 1 }, "Tue");
	db.Add({ 1, 2, 1 }, "Wed");
	db.Add({ 1, 2, 2 }, "Thu");
	db.Add({ 2, 1, 1 }, "Fri");

	AssertEqual(db.Last({ 1, 1, 1 }), pair<Date, string>{ { 1, 1, 1 }, "Tue" }, "TestDatabase_Last: #1");
	AssertEqual(db.Last({ 1, 2, 1 }), pair<Date, string>{ { 1, 2, 1 }, "Wed" }, "TestDatabase_Last: #2");
	AssertEqual(db.Last({ 3, 3, 3 }), pair<Date, string>{ { 2, 1, 1 }, "Fri" }, "TestDatabase_Last: #3");

	bool caught_invalid_argument = false;
	try { db.Last({ 0, 0, 0 }); } catch (invalid_argument&) { caught_invalid_argument = true; }
	Assert(caught_invalid_argument, "TestDatabase_Last: #5");

	caught_invalid_argument = false;
	try { db.Last({ 3, 3, 3 }); } catch (invalid_argument&) { caught_invalid_argument = true; }
	Assert(!caught_invalid_argument, "TestDatabase_Last: #6");
}
