#include <iostream>
#include <exception>
#include <string>
#include <system_error>
using namespace std;

string AskTimeServer() {
	throw system_error(EDOM, generic_category(), "hello world");
    /* Для тестирования повставляйте сюда код, реализующий различное поведение этой функии:
       * нормальный возврат строкового значения
       * выброс исключения system_error
       * выброс другого исключения с сообщением.
    */
	return "1:1:1";
}

class TimeServer {
public:
    string GetCurrentTime() {
		string ret;
		try {
			ret = AskTimeServer();
		}
		catch (system_error&) {
			return LastFetchedTime;
		}
		catch (...) {
			throw;
		}
		LastFetchedTime = ret;
		return ret;
}
private:
    string LastFetchedTime = "00:00:00";
};

int main() {
    TimeServer ts;
    try {
        cout << ts.GetCurrentTime() << endl;
    } catch (exception& e) {
        cout << "Exception got: " << e.what() << endl;
    }
    return 0;
}
