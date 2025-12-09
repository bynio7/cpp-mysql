#include <iostream>
#include <fstream>
#include <streambuf>
#include <memory>

#include <yaml-cpp/yaml.h>

#include <mysqlx/xdevapi.h> // MySQL Connector/C++

using namespace std;

int main() {
	try {
		// Wczytanie config.yaml
		YAML::Node cfg = YAML::LoadFile("config.yaml");

		string host   = cfg["db"]["host"].as<string>();
		string user   = cfg["db"]["user"].as<string>();
		string pass   = cfg["db"]["pass"].as<string>();
		string schema = cfg["db"]["schema"].as<string>();

		string query_name = cfg["query"].as<string>();
		int limit = cfg["limit"].as<int>();

		// Wczytanie pliku SQL
		ifstream t("queries/" + query_name + ".sql");
		if (!t) {
			cerr << "Nie mogę otworzyć pliku SQL.\n";
			return 1;
		}

		string sql((istreambuf_iterator<char>(t)),
		                 istreambuf_iterator<char>());

		// Połączenie z MySQL
		mysqlx::Session sess(host, 33060, user, pass);
		sess.sql("USE " + schema).execute();

		// Przygotowanie zapytania
		auto result = sess.sql(sql).bind(limit).execute();

		// Wyświetlenie wyników
		for (auto row : result) {
			cout << row[0].get<uint64_t>() << " | "
			          << row[1].get<string>() << " | "
			          << row[2].get<string>() << "\n";
		}
	}
	catch (const exception &ex) {
		cerr << "Błąd: " << ex.what() << "\n";
	}

	return 0;
}
