#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Human {
public:

    Human(const string& name, const string& type) :
        Name_(name), Type_(type) {}

    const string& Name() const {
        return Name_;
    }
    const string& Type() const {
        return Type_;
    }

    virtual void Walk(const string& destination) const {
        cout << Type_ << ": " << Name_ << " walks to: " << destination << endl;
    }

    string TypeSuffixed(const string& suffix = ": ") const {
        return Type_ + suffix;
    }

private:

    const string Name_;
    const string Type_;
};


class Student : public Human {
public:

    Student(const string& name, const string& favouriteSong) :
        Human(name, "Student"),
        FavouriteSong(favouriteSong) { }

    void Learn() const {
        cout << TypeSuffixed() << Name() << " learns" << endl;
    }

    void Walk(const string& destination) const override {
        Human::Walk(destination);
        SingSong();
    }

    void SingSong() const {
        cout << TypeSuffixed() << Name() << " sings a song: " << FavouriteSong << endl;
    }

public:

    const string FavouriteSong;
};


class Teacher : public Human {
public:

    Teacher(string name, string subject) :
        Human(name, "Teacher"),
        Subject(subject) { }

    void Teach() const {
        cout << TypeSuffixed() << Name() << " teaches: " << Subject << endl;
    }

public:

    const string Subject;
};


class Policeman : public Human {
public:

    Policeman(const string& name) :
        Human(name, "Policeman") {}

    void Check(const Human& p) const {
        cout << TypeSuffixed() << Name() << " checks " << p.Type()
            << ". " << p.Type() << "'s name is: " << p.Name() << endl;
    }
};


void VisitPlaces(const Human& person, const vector<string>& places) {
    for (auto p : places) {
        person.Walk(p);
    }
}


int main() {
    Teacher t("Jim", "Math");
    Student s("Ann", "We will rock you");
    Policeman p("Bob");

    VisitPlaces(t, {"Moscow", "London"});
    p.Check(s);
    VisitPlaces(s, {"Moscow", "London"});
    return 0;
}

